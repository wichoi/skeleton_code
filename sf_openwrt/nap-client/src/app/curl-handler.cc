#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "cert-manager.h"
#include "curl-handler.h"

curl_handler::curl_handler()
{
}

curl_handler::~curl_handler()
{
    log_d("%s\n", __func__);
}

int curl_handler::init(void)
{
    int ret_val = RET_OK;
    log_d("curl_handler::%s \n", __func__);
    return ret_val;
}

int curl_handler::deinit(void)
{
    log_d("%s\n", __func__);
    return RET_OK;
}

size_t curl_handler::curl_callback(void *ptr, size_t size, size_t count, void *stream)
{
    log_d("curl_callback[%s]\n", (char*)ptr);

    string *body = (string*)stream;
    if(body)
    {
        body->append((char*)ptr, (int)(size * count));
    }
    return size * count;
}

int curl_handler::curl_request(string &url, string &body, method_e method, header_e header_type, response &resp)
{
    log_d("%s url[%s] body len[%u]\n%s\n",
            __func__, url.c_str(), body.length(), body.c_str());

    int result = RET_ERROR;

    CURL *curl;
    curl = curl_easy_init();
    if (curl)
    {
        CURLcode curl_result;
        curl_result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "User-Agent: skeleton client/1.0");
        switch(header_type)
        {
        case HEADER_CA:
            break;
        case HEADER_ENROL:
            {
                list = curl_slist_append(list, "Content-Type: application/pkcs10");
                list = curl_slist_append(list, "Content-Transfer-Encoding: base64");
                string enrol_srv = config_manager::instance()->get_auth_url();
                curl_easy_setopt(curl, CURLOPT_SSLCERT, NAP_CRT);
                curl_easy_setopt(curl, CURLOPT_SSLKEY, NAP_KEY);
            }
            break;
        case HEADER_TOKEN:
            {
                list = curl_slist_append(list, "Content-Type: application/x-www-form-urlencoded");

                curl_easy_setopt(curl, CURLOPT_SSLCERT, AUTH_CRT);
                curl_easy_setopt(curl, CURLOPT_SSLKEY, AUTH_KEY);
            }
            break;
        case HEADER_RMS:
            {
                list = curl_slist_append(list, "Content-Type: application/json");
                if(config_manager::instance()->get_rms_token().length() > 0)
                {
                    string token = "x-auth-token: ";
                    token.append(config_manager::instance()->get_rms_token());
                    list = curl_slist_append(list, token.c_str());
                }
            }
            break;
        default:
            break;
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        // TLS verify
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        if(method == CMD_POST)
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L); //POST options
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.length());
        }

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30); // 30sec
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); // 30sec
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);

        resp.body = "";
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&resp.body);

        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        curl_result = curl_easy_perform(curl);
        if (curl_result == CURLE_OK)
        {
            result = RET_OK;
            long req;
            curl_result = curl_easy_getinfo(curl, CURLINFO_REQUEST_SIZE, &req);
            if (!curl_result)
            {
                log_d("Request size: %ld bytes\n", req);
            }
        }
        else
        {
            log_e("curl perform error ret(%d) \n", curl_result);
            result = RET_ERR_CURL;
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.code);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &resp.size);
        //log_d("%s code[%d] size[%.0fbytes]\n", __func__, resp.code, resp.size);

        curl_slist_free_all(list);
        curl_easy_cleanup(curl);
    }

    return result;
}

size_t curl_handler::curl_file_callback(void *ptr, size_t size, size_t count, void *stream)
{
    //log_d("curl_callback [%d]\n", (int)(size * count));
    //log_d("curl_callback[%s]\n", (char*)ptr);
    FILE *fp = (FILE*)stream;
    if(fp)
    {
        fwrite((char*)ptr, 1, (int)(size * count), fp);
    }
    return size * count;
}

int curl_handler::curl_file_download(string &fw_url, string &fw_name, response &resp)
{
    log_i("%s url[%s] file[%s]\n", __func__, fw_url.c_str(), fw_name.c_str());
    int result = RET_ERROR;

    FILE *fp = fopen(fw_name.c_str(), "w");
    if(fp == NULL)
    {
        log_w("%s file open failed !!! [%s]\n", __func__, fw_name.c_str());
        return result;
    }

    CURL *curl;
    curl = curl_easy_init();
    if (curl)
    {
        CURLcode curl_result;
        curl_result = curl_easy_setopt(curl, CURLOPT_URL, fw_url.c_str());

        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "User-Agent: Sigfox NAP client/1.0");
        if(config_manager::instance()->get_rms_token().length() > 0)
        {
            string token = "x-auth-token: ";
            token.append(config_manager::instance()->get_rms_token());
            list = curl_slist_append(list, token.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        // TLS verify
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60 * 30); // 30 min
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); // 30sec
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_file_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)fp);

        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        curl_result = curl_easy_perform(curl);
        if (curl_result == CURLE_OK)
        {
            result = RET_OK;
            long req;
            curl_result = curl_easy_getinfo(curl, CURLINFO_REQUEST_SIZE, &req);
            if (!curl_result)
            {
                log_d("Request size: %ld bytes\n", req);
            }
        }
        else
        {
            log_e("curl perform error ret(%d) \n", curl_result);
            result = RET_ERR_CURL;
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.code);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &resp.size);
        log_i("%s code[%d] size[%.0fbytes]\n", __func__, resp.code, resp.size);

        curl_slist_free_all(list);
        curl_easy_cleanup(curl);
    }

    fclose(fp);
    return result;
}

