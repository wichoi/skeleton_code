#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
#include <io.h>

#include <openssl/aes.h>

#include "typedef.h"
#include "app.h"

// header 128byte
#define SZ_MODEL        32      // 32 byte
#define SZ_VER_MAJOR    1       // 1  byte
#define SZ_VER_MINOR    1       // 1  byte
#define SZ_VER_BUILD    1       // 1  byte
#define SZ_CRYPTO       1       // 1  byte
#define SZ_FW           4       // 4  byte
#define SZ_CRC32        4       // 4  byte
#define SZ_MAGIC        1       // 1  byte
#define SZ_RESERVED     83      // 83 byte


#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 128 // 256
#define IV_SIZE 16
#define RW_SIZE 1
#define HEADER_SZ 128

static const unsigned char g_crypto_key32[32]=
{
    'A',
};

u32 app_crc32(u8 const * p_data, u32 size, u32 const * p_crc)
{
    u32 crc = 0, i = 0, j = 0;
    crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
    for (i = 0; i < size; i++)
    {
        crc = crc ^ p_data[i];
        for (j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}

u32 app_encrypt(char *in_file, char *out_file)
{
    u32 ret_val = RET_SUCCESS;

    if( _access( in_file, 0 ) == -1 )
    {
        // file doesn't exist
        printf("[ERROR] file open failed !!! [%s] \n", in_file);
        ret_val = RET_ERROR;
        return ret_val;
    }

    FILE *rfp = fopen(in_file, "rb");
    FILE *wfp = fopen(out_file, "wb");

    if((rfp != NULL) || (wfp != NULL))
    {
        int len = 0;
        int padding_len = 0;
        char buf[FREAD_COUNT + BLOCK_SIZE] = {0x0,};

        AES_KEY aes_ks3 = {0x0,};
        AES_set_encrypt_key(g_crypto_key32, KEY_BIT, &aes_ks3);
        while(len = fread(buf, RW_SIZE, FREAD_COUNT, rfp))
        {
            if(FREAD_COUNT != len)
            {
                break;
            }

            unsigned char iv[IV_SIZE] = {0x0,};
            AES_cbc_encrypt(buf ,buf, len, &aes_ks3, iv, AES_ENCRYPT);
            fwrite(buf, RW_SIZE, len, wfp);
        }

        padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
        printf("enc padding len:%d\n", padding_len);
        memset(buf + len, padding_len, padding_len);

        unsigned char iv[IV_SIZE] = {0x0,};
        AES_cbc_encrypt(buf, buf, len + padding_len, &aes_ks3, iv, AES_ENCRYPT);
        fwrite(buf, RW_SIZE, len + padding_len, wfp);
    }
    else
    {
        printf("[ERROR] file open failed !!!\n");
        ret_val = RET_ERROR;
    }

    if(rfp != NULL) fclose(rfp);
    if(wfp != NULL) fclose(wfp);

    printf("%s return : %d \n", __func__, ret_val);
    return ret_val;
}

u32 app_decrypt(char *in_file, char *out_file)
{
    u32 ret_val = RET_SUCCESS;

    if(_access( in_file, 0 ) == -1 )
    {
        // file doesn't exist
        printf("[ERROR] file open failed !!! [%s] \n", in_file);
        ret_val = RET_ERROR;
        return ret_val;
    }

    FILE *rfp = fopen(in_file, "rb");
    FILE *wfp = fopen(out_file, "wb");

    if((rfp != NULL) || (wfp != NULL))
    {
        int len = 0;
        int total_size = 0;
        int save_len = 0;
        int w_len = 0;
        char buf[FREAD_COUNT + BLOCK_SIZE] = {0x0,};

        AES_KEY aes_ks3 = {0x0,};
        AES_set_decrypt_key(g_crypto_key32, KEY_BIT, &aes_ks3);

        fseek(rfp, 0, SEEK_END);
        total_size = ftell(rfp);
        fseek(rfp, 0, SEEK_SET);
        printf("total_size %d\n", total_size);

        while(len = fread(buf, RW_SIZE, FREAD_COUNT, rfp))
        {
            if(len == 0)
            {
                break;
            }
            save_len += len;
            w_len = len;

            unsigned char iv[IV_SIZE] = {0x0,};
            AES_cbc_encrypt(buf, buf, len, &aes_ks3, iv, AES_DECRYPT);
            if(save_len == total_size) // check last block
            {
                w_len = len - buf[len - 1];
                printf("dec padding size %d\n", buf[len - 1]);
            }

            fwrite(buf, RW_SIZE, w_len, wfp);
        }
    }
    else
    {
        printf("[ERROR] file open failed !!!\n");
        ret_val = RET_ERROR;
    }

    if(rfp != NULL) fclose(rfp);
    if(wfp != NULL) fclose(wfp);

    printf("%s return : %d \n", __func__, ret_val);
    return ret_val;
}

u32 app_proc(info_t *info)
{
    u32 ret_val = RET_SUCCESS;

    if(_access( info->in_file, 0 ) == -1 )
    {
        // file doesn't exist
        printf("[ERROR] file open failed !!! [%s] \n", info->in_file);
        ret_val = RET_ERROR;
        return ret_val;
    }

    u8 *in_file = info->in_file;
    u8 crypt_file[256] = {0,};
    if(info->is_crypto == 1)
    {
        snprintf(crypt_file, sizeof(crypt_file), "%s.en", info->in_file);
        app_encrypt(info->in_file, crypt_file);
        in_file = crypt_file;
    }

    FILE *rfp = fopen(in_file, "rb");
    FILE *wfp = fopen(info->out_file, "wb");
    u32 crc_key = CRC32_KEY;
    int fw_size = 0;

    if((rfp != NULL) || (wfp != NULL))
    {
        int len = 0;
        char buf[FREAD_COUNT] = {0x0,};

        fseek(wfp, HEADER_SZ, SEEK_SET);
        while(len = fread(buf, RW_SIZE, FREAD_COUNT, rfp))
        {
            if(len == 0)
            {
                break;
            }

            crc_key = app_crc32(buf, len, &crc_key);
            fwrite(buf, RW_SIZE, len, wfp);
            fw_size += len;
        }

        fseek(wfp, 0, SEEK_SET);
        int i = 0;
        len = 0;
        for(i = 0; i < SZ_MODEL; i++)
        {
            buf[len++] = info->model[i];
        }
        buf[len++] = info->ver_major;
        buf[len++] = info->ver_minor;
        buf[len++] = info->ver_build;
        buf[len++] = info->is_crypto;
        buf[len++] = (fw_size >> 24) & 0xFF;
        buf[len++] = (fw_size >> 16) & 0xFF;
        buf[len++] = (fw_size >> 8) & 0xFF;
        buf[len++] = (fw_size) & 0xFF;
        buf[len++] = (info->offset >> 24) & 0xFF;
        buf[len++] = (info->offset >> 16) & 0xFF;
        buf[len++] = (info->offset >> 8) & 0xFF;
        buf[len++] = (info->offset) & 0xFF;
        buf[len++] = (crc_key >> 24) & 0xFF;
        buf[len++] = (crc_key >> 16) & 0xFF;
        buf[len++] = (crc_key >> 8) & 0xFF;
        buf[len++] = (crc_key) & 0xFF;
        buf[len++] = 0xE1;
        fwrite(buf, RW_SIZE, len, wfp);
    }
    else
    {
        printf("[ERROR] file open failed !!!\n");
        ret_val = RET_ERROR;
    }

    if(rfp != NULL) fclose(rfp);
    if(wfp != NULL) fclose(wfp);

    if(ret_val == RET_SUCCESS)
    {
        printf("%s result[%u] tot[%u] fw[%u] crc32[0x%08X] \n",
                __func__, ret_val, fw_size + HEADER_SZ, fw_size, crc_key);
    }
    else
    {
        printf("%s make image ERROR !!! \n", __func__);
    }
    return ret_val;
}
