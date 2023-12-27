#ifndef __CURL_HANDLER_H__
#define __CURL_HANDLER_H__

#include <string>

#include "common.h"

class curl_handler
{
public:
    curl_handler();
    ~curl_handler();
    int init(void);
    int deinit(void);

public:
    typedef enum
    {
        CMD_GET     = 0,
        CMD_POST    = 1,
    }method_e;

    class response
    {
    public:
        string header;
        string body;
        long code;
        double size;
        int curl_result;

        response()
        {
            header = "";
            body = "";
            code = 0;
            size = 0;
            curl_result = 0;
        };
    };

private:
    static size_t curl_header_cb(void *ptr, size_t size, size_t count, void *stream);
    static size_t curl_body_cb(void *ptr, size_t size, size_t count, void *stream);
public:
    int curl_request(string &url, string &body, method_e method, response &resp, string server_token = "");

private:
    static size_t curl_file_callback(void *ptr, size_t size, size_t count, void *stream);
public:
    int curl_file_download(string &fw_url, string &fw_name, response &resp);
};

#endif
