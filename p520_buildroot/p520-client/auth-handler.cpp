#include <openssl/opensslconf.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/err.h>

#include "log.h"
#include "utils.h"
#include "config-manager.h"
#include "auth-handler.h"

#define BLOCK_SIZE 16
#define READ_COUNT 4096
#define IV_SIZE 16

#define KEY_1_POS_1     6
#define KEY_1_POS_2     7
#define KEY_1_POS_3     8
#define KEY_1_POS_4     9

#define KEY_2_POS_1     4
#define KEY_2_POS_2     5
#define KEY_2_POS_3     10
#define KEY_2_POS_4     11

#define KEY_3_POS_1     2
#define KEY_3_POS_2     3
#define KEY_3_POS_3     11
#define KEY_3_POS_4     12

#define KEY_4_POS_1     4
#define KEY_4_POS_2     5
#define KEY_4_POS_3     9
#define KEY_4_POS_4     10

typedef struct
{
    char key1[16];
    char key2[16];
    char key3[16];
    char key4[16];
} board_info_t;

auth_handler::auth_handler() :
    _factory_key()
{
    log_d("%s\n", __func__);
}

auth_handler::~auth_handler()
{
    log_d("%s\n", __func__);
}

int auth_handler::init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    memset(_factory_key, 0, sizeof(_factory_key));
    return ret_val;
}

int auth_handler::deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    return ret_val;
}

unsigned char* auth_handler::get_factory_key(void)
{
    board_info_t info;

    _factory_key[0] = info.key1[KEY_1_POS_1];
    _factory_key[1] = info.key1[KEY_1_POS_2];
    _factory_key[2] = info.key1[KEY_1_POS_3];
    _factory_key[3] = info.key1[KEY_1_POS_4];

    _factory_key[4] = info.key2[KEY_2_POS_1];
    _factory_key[5] = info.key2[KEY_2_POS_2];
    _factory_key[6] = info.key2[KEY_2_POS_3];
    _factory_key[7] = info.key2[KEY_2_POS_4];

    _factory_key[8] = info.key3[KEY_3_POS_1];
    _factory_key[9] = info.key3[KEY_3_POS_2];
    _factory_key[10] = info.key3[KEY_3_POS_3];
    _factory_key[11] = info.key3[KEY_3_POS_4];

    _factory_key[12] = info.key4[KEY_4_POS_1];
    _factory_key[13] = info.key4[KEY_4_POS_2];
    _factory_key[14] = info.key4[KEY_4_POS_3];
    _factory_key[15] = info.key4[KEY_4_POS_4];

    return _factory_key;
}

int auth_handler::ase128_encrypt(string &dest, string &src, string &key)
{
    int ret_val = RET_ERROR;
    int len = src.length();
    AES_KEY en_key;

    if(len > READ_COUNT)
    {
        log_i("%s buf size !!!\n", __func__);
        return ret_val;
    }

    dest.clear();
    if(AES_set_encrypt_key((const unsigned char*)key.c_str(), 128, &en_key) >= 0)
    {
        unsigned char iv[IV_SIZE] = {0x0,};
        unsigned char buf[READ_COUNT + IV_SIZE] = {0,};
        int padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
        memcpy(buf, src.c_str(), len);
        memset(buf + len, padding_len, padding_len);
        AES_cbc_encrypt(buf, buf, len + padding_len, &en_key, iv, AES_ENCRYPT);
        dest = (char*)buf;
        ret_val = RET_OK;
    }

    log_d("%s ret_val[%d\\n", __func__, ret_val);
    return ret_val;
}

int auth_handler::ase128_decrypt(string &dest, string &src, string &key)
{
    int ret_val = RET_ERROR;
    int len = src.length();
    AES_KEY de_key;

    if(len > READ_COUNT)
    {
        log_i("%s buf size !!!\n", __func__);
        return ret_val;
    }

    dest.clear();
    if(AES_set_decrypt_key((const unsigned char*)key.c_str(), 128, &de_key) >= 0)
    {
        unsigned char iv[IV_SIZE] = {0x0,};
        unsigned char buf[READ_COUNT] = {0,};
        memcpy(buf, src.c_str(), len);
        AES_cbc_encrypt(buf, buf, len, &de_key, iv, AES_DECRYPT);
        dest = (char*)buf;
        ret_val = RET_OK;
    }

    log_d("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int auth_handler::ase128_encrypt_base64(string &dest, string &src, string &key)
{
    int ret_val = RET_ERROR;
    int len = src.length();
    AES_KEY en_key;

    if(len > READ_COUNT)
    {
        log_i("%s buf size !!!\n", __func__);
        return ret_val;
    }

    dest.clear();
    if(AES_set_encrypt_key((const unsigned char*)key.c_str(), 128, &en_key) >= 0)
    {
        unsigned char iv[IV_SIZE] = {0x0,};
        unsigned char buf[READ_COUNT + IV_SIZE] = {0,};
        int padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
        memcpy(buf, src.c_str(), len);
        memset(buf + len, padding_len, padding_len);
        AES_cbc_encrypt(buf, buf, len + padding_len, &en_key, iv, AES_ENCRYPT);
        utils::encode_base64(dest, buf, len + padding_len);
        ret_val = RET_OK;
    }

    log_d("%s ret_val[%d\\n", __func__, ret_val);
    return ret_val;
}

int auth_handler::ase128_decrypt_base64(string &dest, string &src, string &key)
{
    int ret_val = RET_ERROR;
    int len = src.length();
    AES_KEY de_key;

    if(len > READ_COUNT)
    {
        log_i("%s buf size !!!\n", __func__);
        return ret_val;
    }

    dest.clear();
    if(AES_set_decrypt_key((const unsigned char*)key.c_str(), 128, &de_key) >= 0)
    {
        unsigned char iv[IV_SIZE] = {0x0,};
        unsigned char buf[READ_COUNT] = {0,};
        utils::decode_base64((char*)buf, &len, src);
        AES_cbc_encrypt(buf, buf, len, &de_key, iv, AES_DECRYPT);
        dest.append((char*)buf, len);
        ret_val = RET_OK;
    }

    log_d("%s ret_val[%d][%s]\n", __func__, ret_val, dest.c_str());
    return ret_val;
}

int auth_handler::sha256_hex_string(string &dest, string &src)
{
    int ret_val = RET_OK;
    unsigned char hash[SHA256_DIGEST_LENGTH] = {0,}; // 32 byte

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, src.c_str(), src.length());
    SHA256_Final(hash, &sha256);

    int i = 0;
    char buf[SHA256_DIGEST_LENGTH * 2 + 1] = {0,};
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
      sprintf(buf + (i * 2), "%02X", hash[i]);
    }
    dest.clear();
    dest = buf;

    log_i("%s ret_val[%d] hash[%s]\n", __func__, ret_val, dest.c_str());
    return ret_val;
}

