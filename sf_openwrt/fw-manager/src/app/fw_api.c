#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fw_main.h"
#include "fw_dbg.h"
#include "fw_manager.h"
#include "fw_crypto.h"
#include "fw_api.h"

#define STR_CRYPTO      "@crypto"

int fwmgr_splite(char *fw_path, char *fw_name, fw_header_t *p_header, fw_opt_e rm_opt)
{
    int ret_val = 0;

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
    for(int i = 0; i < fw_info->file_cnt; i++)
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

