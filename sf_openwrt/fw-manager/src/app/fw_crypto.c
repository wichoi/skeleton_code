#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>         // O_WRONLY
#include <unistd.h>        // write(), close()

#include <openssl/aes.h>

#include "fw_dbg.h"

#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 256
#define IV_SIZE 16
#define RW_SIZE 1

#define KEY_LENGTH      (16)

static const unsigned char g_crypto_key32[32]=
    {'t','e','s','t','1','2','3','4',
    't','e','s','t','1','2','3','4',
    't','e','s','t','1','2','3','4',
    't','e','s','t','1','2','3','4'};

bool AproEncryptAes(char *in_file, char *out_file)
{
    bool ret_val = true;
    FILE *rfp = fopen(in_file, "rb");
    FILE *wfp = fopen(out_file, "wb");

    if((rfp != NULL) || (wfp != NULL))
    {
        int len = 0;
        int padding_len = 0;
        unsigned char buf[FREAD_COUNT + BLOCK_SIZE] = {0x0,};

        AES_KEY aes_ks3 = {0x0,};
        unsigned char iv[IV_SIZE] = {0x0,};

        unsigned char crypto_key[KEY_LENGTH + 1] = { 0, };
        AES_set_encrypt_key(g_crypto_key32, KEY_BIT, &aes_ks3);

        while(len = fread(buf, RW_SIZE, FREAD_COUNT, rfp))
        {
            if(FREAD_COUNT != len)
            {
                break;
            }

            AES_cbc_encrypt((unsigned char*)buf ,buf, len, &aes_ks3, iv, AES_ENCRYPT);
            fwrite(buf, RW_SIZE, len, wfp);
        }

        padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
        log_print(LOG_I, "enc padding len:%d\n", padding_len);
        memset(buf + len, padding_len, padding_len);

        AES_cbc_encrypt((unsigned char*)buf, buf, len + padding_len, &aes_ks3, iv, AES_ENCRYPT); // AES_ecb_encrypt
        fwrite(buf, RW_SIZE, len + padding_len, wfp);
    }
    else
    {
        log_print(LOG_I, "[ERROR] file open failed !!!");
        ret_val = false;
    }

    if(rfp != NULL) fclose(rfp);
    if(wfp != NULL) fclose(wfp);

    log_print(LOG_I, "AproEncryptAes() return : %d \n", ret_val);
    return ret_val;
}
 
bool AproDecryptAes(char *in_file, char *out_file)
{
    bool ret_val = true;
    FILE *rfp = fopen(in_file, "rb");
    FILE *wfp = fopen(out_file, "wb");

    if((rfp != NULL) || (wfp != NULL))
    {
        int len = 0;
        int total_size = 0;
        int save_len = 0;
        int w_len = 0;
        unsigned char buf[FREAD_COUNT + BLOCK_SIZE] = {0x0,};

        AES_KEY aes_ks3 = {0x0,};
        unsigned char iv[IV_SIZE] = {0x0,};

        unsigned char crypto_key[KEY_LENGTH + 1] = { 0, };
        AES_set_decrypt_key(g_crypto_key32, KEY_BIT, &aes_ks3);

        fseek(rfp, 0, SEEK_END);
        total_size = ftell(rfp);
        fseek(rfp, 0, SEEK_SET);
        log_print(LOG_I, "total_size %d\n", total_size);

        while(len = fread(buf, RW_SIZE, FREAD_COUNT, rfp))
        {
            if(len == 0)
            {
                break;
            }
            save_len += len;
            w_len = len;

            AES_cbc_encrypt((unsigned char*)buf, buf, len, &aes_ks3, iv, AES_DECRYPT); // AES_ecb_encrypt
            if(save_len == total_size) // check last block
            {
                w_len = len - buf[len - 1];
                log_print(LOG_I, "dec padding size %d\n", buf[len - 1]);
            }

            fwrite(buf, RW_SIZE, w_len, wfp);
        }
    }
    else
    {
        log_print(LOG_I, "[ERROR] file open failed !!!");
        ret_val = false;
    }

    if(rfp != NULL) fclose(rfp);
    if(wfp != NULL) fclose(wfp);

    log_print(LOG_I, "AproDecryptAes() return : %d \n", ret_val);
    return ret_val;
}

#if 0
bool fw_encrypto_aes(const char *in_file, const char *out_file)
{
    bool ret_val = true;
    int rfd = open(in_file, O_RDONLY);
    int wfd = open(out_file, O_WRONLY | O_CREAT, 0777); // 644

    if((rfd != -1) || (wfd != -1))
    {
        int len = 0;
        int padding_len = 0;
        unsigned char buf[FREAD_COUNT + BLOCK_SIZE] = {0x0,};

        AES_KEY aes_ks3 = {0x0,};
        unsigned char iv[IV_SIZE] = {0x0,};

        unsigned char crypto_key[KEY_LENGTH + 1] = { 0, };
        AES_set_encrypt_key(g_crypto_key32, KEY_BIT, &aes_ks3);

        while(len = read(rfd, buf, FREAD_COUNT))
        {
            if(FREAD_COUNT != len)
            {
                break;
            }

            AES_cbc_encrypt((unsigned char*)buf ,buf, len, &aes_ks3, iv, AES_ENCRYPT);
            write(wfd, buf, len);
        }

        padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
        log_print(LOG_I, "enc padding len:%d\n", padding_len);
        memset(buf + len, padding_len, padding_len);

        AES_cbc_encrypt((unsigned char*)buf, buf, len + padding_len, &aes_ks3, iv, AES_ENCRYPT);
        write(wfd, buf, len + padding_len);
    }
    else
    {
        log_print(LOG_I, "[ERROR] file open failed !!!");
        ret_val = false;
    }

    if(rfd != -1) close(rfd);
    if(wfd != -1) close(wfd);

    log_print(LOG_I, "fw_encrypto_aes() return : %d \n", ret_val);
    return ret_val;
}

bool fw_decrypto_aes(char *in_file, char *out_file)
{
    bool ret_val = true;
    int rfd = open(in_file, O_RDONLY);
    int wfd = open(out_file, O_WRONLY | O_CREAT, 0777); // 644

    if((rfd != -1) || (wfd != -1))
    {
        int len = 0;
        int total_size = 0;
        int save_len = 0;
        int w_len = 0;
        unsigned char buf[FREAD_COUNT + BLOCK_SIZE] = {0x0,};

        AES_KEY aes_ks3 = {0x0,};
        unsigned char iv[IV_SIZE] = {0x0,};

        unsigned char crypto_key[KEY_LENGTH + 1] = { 0, };
        AES_set_decrypt_key(g_crypto_key32, KEY_BIT, &aes_ks3);

        total_size = lseek(rfd, 0, SEEK_END);
        lseek(rfd, 0, SEEK_SET);
        log_print(LOG_I, "total_size %d\n", total_size);

        while(len = read(rfd, buf, FREAD_COUNT))
        {
            if(len == 0)
            {
                break;
            }
            save_len += len;
            w_len = len;

            AES_cbc_encrypt((unsigned char*)buf, buf, len, &aes_ks3, iv, AES_DECRYPT);
            if(save_len == total_size) // check last block
            {
                w_len = len - buf[len - 1];
                log_print(LOG_I, "dec padding size %d\n", buf[len - 1]);
            }

            write(wfd, buf, w_len);
        }
    }
    else
    {
        log_print(LOG_I, "[ERROR] file open failed !!!");
        ret_val = false;
    }

    if(rfd != -1) close(rfd);
    if(wfd != -1) close(wfd);

    log_print(LOG_I, "fw_decrypto_aes() return : %d \n", ret_val);
    return ret_val;
}
#endif

