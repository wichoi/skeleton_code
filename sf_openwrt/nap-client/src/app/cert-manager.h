#ifndef __CERT_MANAGER_H__
#define __CERT_MANAGER_H__

#include "common.h"

class cert_manager
{
public:
    cert_manager();
    ~cert_manager();
    int init(void);
    int deinit(void);

public:
    int nap_key_read(void);
    int nap_key_generate(void);
    int nap_key_delete(void);
    int nap_key_print(void);
    int nap_csr_body(string &body);
    int nap_crt_read(void);
    int nap_crt_write(string &crt);
    int nap_crt_delete(void);

    int auth_key_read(void);
    int auth_key_generate(void);
    int auth_ca_read(void);
    int auth_ca_write(string &body);
    int auth_ca_delete(void);
    int auth_crt_read(void);
    int auth_crt_write(string &body);
    int auth_crt_delete(void);
    int auth_csr_body(string &body); // Certificate Signature Request
    int auth_token_body(string &src_data, string &dest_data);

    int grpc_crt_valid(void);
    int grpc_crt_read(string &cert);
    int grpc_crt_delete(void);
    int grpc_crt_access(void);

    int read_file(string &file_name, string &data);
    int write_file(string &file_name, string &data);
};

#endif
