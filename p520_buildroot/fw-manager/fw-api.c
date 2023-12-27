#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "fw-main.h"
#include "fw-dbg.h"
#include "fw-manager.h"
#include "fw-crc32.h"
#include "fw-crypto.h"
#include "fw-api.h"

#define STR_CRYPTO      "@crypto"

int fwmgr_splite(char *fw_path, char *fw_name, fw_header_t *p_header, fw_opt_e rm_opt)
{
    int ret_val = 0;
    int i = 0;

    debug_init(DBG_DISABLE, LOG_I);

    log_print(LOG_I, "splite path [%s]\n", fw_path);
    fw_sp_init(fw_path);
    if(fw_Splite(fw_name, (int)rm_opt) == false)
    {
        log_print(LOG_I, "firmware splite failed \n");
        ret_val = 1;
        return ret_val;
    }

    FwHeaderInfo *fw_info = fw_header_get();
    FwFileInfo *fw_item = (FwFileInfo*)&(fw_info->fw_files);
    memcpy(p_header->header_info, fw_info->header_info, strlen(fw_info->header_info));
    p_header->file_cnt = fw_info->file_cnt;
    log_print(LOG_I, "fw_header [%s] cnt[%d]\n", p_header->header_info, p_header->file_cnt);
    for(i = 0; i < fw_info->file_cnt; i++)
    {
        // fw-name
        char f_name[64] = {0,};
        char *ptr = strstr(fw_item->file_name, STR_CRYPTO);
        if(ptr == NULL)
        {
            memcpy(f_name, fw_item->file_name, strlen(fw_item->file_name));
        }
        else
        {
            memcpy(f_name, fw_item->file_name, strlen(fw_item->file_name) - strlen(STR_CRYPTO));
        }
        snprintf(p_header->fw_file[i].name, 64, "%s%s", fw_path, f_name);

        // fw-version
        ptr = NULL;
        char *next_ptr = NULL;
        ptr = strtok_r((char*)f_name, "_", &next_ptr);
        ptr = strtok_r(NULL, "_", &next_ptr);
        snprintf(p_header->fw_file[i].version, 32, "%s", ptr);

        // fw-crc32
        p_header->fw_file[i].crc32 = (unsigned int)atoi(fw_item->crc32);

        // fw-size
        p_header->fw_file[i].length = fw_item->file_size;

        fw_item++;
        log_print(LOG_I, "[%d] name[%s] version[%s] crc32[%d] len[%d]\n", i,
                                            p_header->fw_file[i].name,
                                            p_header->fw_file[i].version,
                                            p_header->fw_file[i].crc32,
                                            p_header->fw_file[i].length);
    }

    log_print(LOG_I, "fwmgr_splite %s", (ret_val == 0) ? "success" : "fail");
    return ret_val;
}

static void *getMemInFile(char *filename, int offset, int len)
{
    void *result = NULL;
    FILE *fp = NULL;

    fp = fopen(filename, "r");
    if(fp != NULL)
    {
        fseek(fp, offset, SEEK_SET);
        result = malloc(sizeof(unsigned char) * len);
        if(result != NULL)
        {
            if(fread(result, 1, len, fp) != len)
            {
                free(result);
                result = NULL;
            }
        }
        fclose(fp);
    }

    return result;
}

int fwmgr_upgrade(char *filename, int file_begin, char *err_msg)
{
    int header_offset = file_begin;
    file_begin += FW_HEADER_SIZE;

    // step 1: read firmware header info (128 byte)
    char *fw_name = (char*)getMemInFile(filename, header_offset, FW_HEADER_NAME_SIZE);
    header_offset += FW_HEADER_NAME_SIZE;
    if(strncmp(fw_name, FW_HEADER_STR, strlen(FW_HEADER_STR)) != 0)
    {
        sprintf(err_msg, "error: FW_HEADER_ERROR");
        free(fw_name);
        return -1;
    }
    log_print(LOG_I, "fw_name[%s] \n", fw_name);
    free(fw_name);

    // step 2: read firmware cnt (4byte)
    unsigned int *p_cnt = (unsigned int*)getMemInFile(filename, header_offset, FW_HEADER_INT_SIZE);
    unsigned int fw_cnt = htonl(*p_cnt);
    header_offset += FW_HEADER_INT_SIZE;
    log_print(LOG_I, "p_cnt[%d] \n", fw_cnt);
    free(p_cnt);
    if(fw_cnt == 0 || fw_cnt > FW_FILE_MAX)
    {
        sprintf(err_msg, "error: FW_COUNT_ERROR [%d]", fw_cnt);
        return -1;
    }

    // step 3: read fw files(fw_name(48 byte), fw_crc(16 byte), fw_size(4 byte))
    int i;
    for(i = 0; i < fw_cnt; i++)
    {
        char fw_img = 0; // 0(none), 1(router), 2(modem)
        char *p_name = (char*)getMemInFile(filename, header_offset, FW_FILENAME_SIZE);
        header_offset += FW_FILENAME_SIZE;
        if(strncmp(p_name, "router_", strlen("router_")) == 0)
        {
            log_print(LOG_I, "router image found [%s] \n", p_name);
            fw_img = 1;
        }
        else if(strncmp(p_name, "modem_", strlen("modem_")) == 0)
        {
            log_print(LOG_I, "modem image found [%s] \n", p_name);
            fw_img = 2;
        }
        free(p_name);

        if(fw_img == 0)
        {
            sprintf(err_msg, "error: UNSUPPORTED_FW_FILE");
            return -1;
        }

        // crc check
        char *p_crc = (char*)getMemInFile(filename, header_offset, FW_CRC32_SIZE);
        unsigned int fw_crc = (unsigned int)strtoul(p_crc, NULL, 10);
        header_offset += FW_CRC32_SIZE;
        log_print(LOG_I, "fw_crc[%u] \n", fw_crc);
        free(p_crc);

        // fw size
        unsigned int *p_len = (unsigned int*)getMemInFile(filename, header_offset, FW_HEADER_INT_SIZE);
        unsigned int fw_len = htonl(*p_len);
        header_offset += FW_HEADER_INT_SIZE;
        log_print(LOG_I, "fw_len[%u] \n", fw_len);
        free(p_len);

        if(fw_len == 0)
        {
            sprintf(err_msg, "error: INVALID_FILE_LENGTH [%u]", fw_len);
            return -1;
        }

        int file_end = file_begin + (int)fw_len;

        unsigned int cal_crc = getFileCrcOffset(filename, file_begin, file_end);
        if(fw_crc != cal_crc)
        {
            log_print(LOG_E, "invalid crc fw_img[%d] fw_crc[%u] cal_crc[%u] \n", fw_img, fw_crc, cal_crc);
            sprintf(err_msg, "error: crc fw_img[%d] fw_crc[%u] cal_crc[%u] \n", fw_img, fw_crc, cal_crc);
            return -1;
        }

        switch(fw_img)
        {
        case 1: // router
            if(routerFwUpgrade(filename, file_begin, file_end, err_msg) == -1)
            {
                return -1;
            }
            break;
        case 2: // modem
            if(modemFwUpgrade(filename, file_begin, file_end, err_msg) == -1)
            {
                return -1;
            }
            break;
        default:
            sprintf(err_msg, "error: UNSUPPORTED_FW_FILE");
            break;
        }

        file_begin = file_end;
    }

    return 0;
}

