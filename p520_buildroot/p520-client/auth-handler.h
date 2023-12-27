#ifndef __AUTH_HANDLER_H__
#define __AUTH_HANDLER_H__

#include "common.h"

class auth_handler
{
public:
    auth_handler();
    ~auth_handler();
    int init(void);
    int deinit(void);

public:
    unsigned char* get_factory_key(void);
    int ase128_encrypt(string &dest, string &src, string &key);
    int ase128_decrypt(string &dest, string &src, string &key);
    int ase128_encrypt_base64(string &dest, string &src, string &key);
    int ase128_decrypt_base64(string &dest, string &src, string &key);
    int sha256_hex_string(string &dest, string &src);

private:
    unsigned char _factory_key[16 + 1];
};

#endif
