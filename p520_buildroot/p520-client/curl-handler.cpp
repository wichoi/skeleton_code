#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>

#include "log.h"
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

size_t curl_handler::curl_header_cb(void *ptr, size_t size, size_t count, void *stream)
{
    log_d("curl_header_cb[%s]\n", (char*)ptr);

    string *header = (string*)stream;
    if(header)
    {
        header->append((char*)ptr, (int)(size * count));
    }
    return size * count;
}

size_t curl_handler::curl_body_cb(void *ptr, size_t size, size_t count, void *stream)
{
    log_d("curl_body_cb[%s]\n", (char*)ptr);

    string *body = (string*)stream;
    if(body)
    {
        body->append((char*)ptr, (int)(size * count));
    }
    return size * count;
}

int curl_handler::curl_request(string &url, string &body, method_e method, response &resp, string server_token)
{
    log_i("%s url[%s]\n", __func__, url.c_str());
    log_d("%s body len[%u]\n%s\n", __func__, body.length(), body.c_str());

    int result = RET_ERROR;

    CURL *curl;
    curl = curl_easy_init();
    if (curl)
    {
        CURLcode curl_result;
        curl_result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "User-Agent: client/1.0");
        list = curl_slist_append(list, "Content-Type: application/json");
        if(server_token.length() > 0)
        {
            string authorization = "Authorization: ";
            authorization.append(server_token);
            list = curl_slist_append(list, authorization.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        // TLS verify
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        // curl_easy_setopt(curl, CURLOPT_CAINFO, "/data/ca-certificates.crt"); // "ca-certificates.crt"
        // curl_easy_setopt(curl, CURLOPT_CAPATH, "/data"); // "./"

        if(method == CMD_POST)
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L); //POST options
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.length());
        }

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30); // 30sec
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); // 30sec

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header_cb);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)&resp.header);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_body_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&resp.body);

        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        curl_result = curl_easy_perform(curl);
        resp.curl_result = curl_result;
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

    return result;
}

size_t curl_handler::curl_file_callback(void *ptr, size_t size, size_t count, void *stream)
{
    //log_d("curl_file_callback [%d]\n", (int)(size * count));
    //log_d("curl_file_callback[%s]\n", (char*)ptr);
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
        list = curl_slist_append(list, "User-Agent: client/1.0");

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

