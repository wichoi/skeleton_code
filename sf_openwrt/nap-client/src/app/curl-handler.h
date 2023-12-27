#ifndef __CURL_HANDLER_H__
#define __CURL_HANDLER_H__

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

    typedef enum
    {
        HEADER_NONE     = 0,
        HEADER_CA       = 1,
        HEADER_ENROL    = 2,
        HEADER_TOKEN    = 3,
        HEADER_RMS      = 4,
    }header_e;

    class response
    {
    public:
        long code;
        double size;
        string body;

        response()
        {
            code = 0;
            size = 0;
            body = "";
        };
    };

private:
    static size_t curl_callback(void *ptr, size_t size, size_t count, void *stream);
public:
    int curl_request(string &url, string &body, method_e method, header_e header_type, response &resp);

private:
    static size_t curl_file_callback(void *ptr, size_t size, size_t count, void *stream);
public:
    int curl_file_download(string &fw_url, string &fw_name, response &resp);
};

#endif
