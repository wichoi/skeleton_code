#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>

#include "fw_dbg.h"
#include "fw_crc32.h"
#include "fw_manager.h"

#define FW_READ_BUF 2048

static unsigned int fw_cnt = 0;
static char fw_name[FW_FILE_MAX][FW_FILENAME_SIZE] = {{0x0,}};

int fw_Init(void)
{
    fw_cnt = 0;
    memset(fw_name, 0x0, sizeof(fw_name));
    log_print(LOG_V, "fw_Init\n");
    return 0;
}

int fw_Append(const char *file_name)
{
    log_print(LOG_V, "file_name [%s] \n", file_name);
    if (fw_cnt > FW_FILE_MAX)
        return -1;

    snprintf((char*)&fw_name[fw_cnt][0], FW_FILENAME_SIZE - 1, "%s", file_name);
    log_print(LOG_V, "[%d] %s \n", fw_cnt, fw_name[fw_cnt]);
    fw_cnt++;
    return 0;
}

int fw_Merge(const char* file_name, FwCryptoType crypt)
{
    FILE            *fw_file = NULL;
    FwHeaderInfo    stHeaderInfo;
    FwFileInfo      *pFirmwareFileInfo;
    unsigned int    index = 0;
    if (fw_cnt == 0)
    {
        log_print(LOG_E, "fw_cnt 0 \n");
        return -1;
    }

    fw_file = fopen(file_name, "wb");

    if (fw_file == NULL)
    {
        log_print(LOG_E, "fopen failed !!! [%s]\n", file_name);
        return -1;
    }

    memset((char*)&stHeaderInfo, 0x00, FW_HEADER_SIZE);
    if(crypt == FW_FILE_NORMAL)
    {
        strcpy((char*)stHeaderInfo.header_info, FW_HEADER_STR);
    }
    else
    {
        strcpy((char*)stHeaderInfo.header_info, FW_CRYPTO_HEADER_STR);
    }

    stHeaderInfo.file_cnt  = 0;

    // 펌웨어 정보 저장할 포인터를 설정
    pFirmwareFileInfo = (FwFileInfo*)stHeaderInfo.fw_files;

    fwrite(&stHeaderInfo, FW_HEADER_SIZE, 1, fw_file);

    for (index = 0; index < fw_cnt; index++)
    {
        FILE            *read_fd;
        char            read_buf[FW_READ_BUF];
        unsigned int    file_size = 0;

        read_fd = fopen((char*)&fw_name[index][0], "rb");
        if (read_fd == NULL)
        {
            log_print(LOG_E, "fopen failed !!! [%s]\n", fw_name[index]);
            if(fw_file != NULL) fclose(fw_file);
            fw_file = NULL;
            return -1;
        }

        while (1)
        {
            int nReadSize = fread(read_buf, 1, FW_READ_BUF, read_fd);

            if (nReadSize == 0)
                break;

            file_size += nReadSize;

            fwrite(read_buf, nReadSize, 1, fw_file);

            if (nReadSize != FW_READ_BUF)
                break;
        }

        fclose(read_fd);
        read_fd = NULL;

        strcpy(pFirmwareFileInfo->file_name, (char*)&fw_name[index][0]);
        pFirmwareFileInfo->file_size  = htonl(file_size);
        snprintf(pFirmwareFileInfo->crc32, 16, "%u", getFileCRC((char*)&fw_name[index][0]));

        log_print(LOG_I, "[%d] fw_name[%s] size[%d] crc[%s] \n",
            index, pFirmwareFileInfo->file_name,file_size, pFirmwareFileInfo->crc32);

        stHeaderInfo.file_cnt++;
        pFirmwareFileInfo++;
    }

    stHeaderInfo.file_cnt = htonl(stHeaderInfo.file_cnt);

    fseek(fw_file, 0L, 0);
    fwrite((char*)&stHeaderInfo, 1, FW_HEADER_SIZE, fw_file);
    if(fw_file != NULL) fclose(fw_file);
    fw_file = NULL;
    return 0;
}


