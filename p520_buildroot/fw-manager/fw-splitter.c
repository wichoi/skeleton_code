#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>

#include "fw-dbg.h"
#include "fw-crc32.h"
#include "fw-manager.h"
#include "fw-crypto.h"

#define FW_BUF_SIZE     2048
#define FW_PATH_LEN     128

static bool             is_fw = false;
static bool             is_header = false;
static bool             is_crypto = false;

static FwHeaderInfo     fw_header = {0,};
static FwFileInfo       *fw_file = NULL;
static int              fw_index = 0;

static int              tot_size = 0;
static unsigned char    parse_buf[FW_BUF_SIZE] = {0x0,};

static char             dest_path[FW_PATH_LEN] = {0x0,};
static char             fw_file_name[FW_PATH_LEN] = {0x0,};

static FILE             *fw_fd = NULL;
static int              write_size = 0;

void fw_sp_init(const char* strPath)
{
    log_print(LOG_D, " fw splitter init\n");
    is_fw       = true;
    is_header   = false;
    is_crypto   = false;

    tot_size    = 0;

    memset(dest_path, 0x0, sizeof(dest_path));
    snprintf((char*)dest_path, FW_PATH_LEN - 1, "%s", strPath);
    memset((char*)&fw_file, 0x0, sizeof(fw_file));
}

static bool fw_ReceiveData(unsigned char *pBuffer, int size)
{
    char    strSource[FW_CRC32_SIZE] = {0x0,};
    char    strTarget[FW_CRC32_SIZE] = {0x0,};
    int nPacketCount;
    int nOffset;
    int nIndex;

    if (is_fw == false)
        return false;

    if (size == 0)
        return true;

    // 모든 파일 쓰기 끝
    if (is_header == true && (unsigned int) fw_index == fw_header.file_cnt)
        return true;

    nPacketCount = (size / 1024) + 1;

    if ((size % 1024) == 0)
        nPacketCount--;

    nOffset = 0;

    for (nIndex = 0; nIndex < nPacketCount; nIndex++)
    {
        int nCopySize = 1024;

        if (nOffset + 1024 > size)
            nCopySize = size - nOffset;

        memset(parse_buf, 0, sizeof(parse_buf));
        memcpy(parse_buf + tot_size, pBuffer + nOffset, nCopySize);

        nOffset += nCopySize;
        tot_size += nCopySize;

        if (is_header == false)
        {
            if (tot_size >= FW_HEADER_SIZE)
            {
                if (memcmp(parse_buf, FW_HEADER_STR, strlen(FW_HEADER_STR)) == 0)
                {
                    is_crypto = false;
                }
                else if (memcmp(parse_buf, FW_CRYPTO_HEADER_STR, strlen(FW_CRYPTO_HEADER_STR)) == 0)
                {
                    is_crypto = true;
                }
                else
                {
                    log_print(LOG_E, "invalid header info\n");
                    is_fw = false;
                    return false;
                }

                memcpy(&fw_header, parse_buf, FW_HEADER_SIZE);

                fw_header.file_cnt  = ntohl(fw_header.file_cnt);
                fw_file             = (FwFileInfo*)fw_header.fw_files;
                fw_file->file_size  = ntohl(fw_file->file_size);
                memset(fw_file_name, 0x0, sizeof(fw_file_name));
                snprintf(fw_file_name, FW_PATH_LEN - 1, "%s%s", dest_path, fw_file->file_name);
                log_print(LOG_V, "file_cnt[%d] file_size[%d] file_name[%s]\n",
                    fw_header.file_cnt, fw_file->file_size, fw_file_name);

                fw_fd = fopen(fw_file_name, "wb");

                if (fw_fd == NULL)
                {
                    is_fw = false;
                    return false;
                }

                is_header   = true;
                write_size  = 0;
                fw_index    = 0;
                nCopySize = tot_size - FW_HEADER_SIZE;
                memcpy(parse_buf, parse_buf + FW_HEADER_SIZE, nCopySize);
                tot_size = nCopySize;
            }
        }

Loop:
        if (is_header == true && tot_size > 0)
        {
            if ((unsigned int)(write_size + tot_size) >= fw_file->file_size)
            {
                nCopySize = fw_file->file_size - write_size;

                if (nCopySize > 0 && fw_fd != NULL)
                {
                    fwrite(parse_buf, nCopySize, 1, fw_fd);
                    write_size += nCopySize;
                    fclose(fw_fd);
                    fw_fd = NULL;
                }

#if 1
                snprintf(strSource, FW_CRC32_SIZE - 1, "%u", getFileCRC(fw_file_name));
                snprintf(strTarget, FW_CRC32_SIZE - 1, "%s", fw_file->crc32);
                log_print(LOG_I, "filename: %s\n", fw_file_name);
                log_print(LOG_I, "CRC source:%s target:%s\n", strSource, strTarget);
                if (strncmp(strSource, strTarget, strlen(strSource)) != 0)
                {
                    unlink(fw_file_name);
                    log_print(LOG_E, "splite crc Error!!\n");
                    is_fw = false;
                    return false;
                }
#endif

                tot_size -= nCopySize;
                memcpy(parse_buf, parse_buf + nCopySize, tot_size);

                fw_index++;
                fw_file++;

                // 모든 파일 쓰기 끝
                if ((unsigned int)fw_index == fw_header.file_cnt)
                {
                    return true;
                }

                write_size = 0;

                fw_file->file_size = ntohl(fw_file->file_size);
                memset(fw_file_name, 0x0, sizeof(fw_file_name));
                snprintf(fw_file_name, FW_PATH_LEN - 1, "%s%s", dest_path, fw_file->file_name);

                fw_fd = fopen(fw_file_name, "wb");
                if (fw_fd == NULL)
                {
                    is_fw = false;
                    return false;
                }
                goto Loop;
            }
            else
            {
                fwrite(parse_buf, tot_size, 1, fw_fd);
                write_size += tot_size;
                tot_size = 0;
            }
        }
    }

    return true;
}

bool fw_Splite(const char* strFileName, int rm_option)
{
    FILE            *read_fd = NULL;
    unsigned char   pBuffer[FW_BUF_SIZE] = {0x0,};
    int             nReadSize = sizeof(pBuffer);

    read_fd = fopen(strFileName, "rb");
    if (read_fd == NULL)
    {
        log_print(LOG_E, "fopen failed !!! \n");
        return false;
    }

    while (1)
    {
        int size = fread(pBuffer, 1, nReadSize, read_fd);

        if (size == 0)
            break;

        if (fw_ReceiveData(pBuffer, size) == false)
            break;

        if (size != nReadSize) // 마지막 자투리 size를 읽는 경우를 대비해 넣은 것 같은데 중간에 size만큼 못 읽으면?
            break;
    }

    if(read_fd != NULL) fclose(read_fd);
    read_fd = NULL;

    if(fw_fd != NULL) fclose(fw_fd);
    fw_fd = NULL;

    if(is_fw == false)
    {
        log_print(LOG_E, "splite failed !!! \n");
        return false;
    }

    if(rm_option == 1) // 0(not remove), 1(remove)
    {
        remove(strFileName);
    }

    if (is_header == true && (unsigned int) fw_index == fw_header.file_cnt)
    {
        if(is_crypto == true)
        {
            int i = 0;
            FwFileInfo *p_header = (FwFileInfo*)fw_header.fw_files;
            for(i = 0; i < fw_header.file_cnt; i++)
            {
                char out_name[64] ={0,};
                char out_path[64] ={0,};
                char in_path[64] = {0,};
                memcpy(out_name, p_header->file_name, strlen(p_header->file_name) - strlen("@crypto"));
                snprintf(out_path, sizeof(out_path), "%s%s", dest_path, out_name);
                snprintf(in_path, sizeof(in_path), "%s%s", dest_path, p_header->file_name);
                if(AproDecryptAes(in_path, out_path) == false)
                {
                    return false;
                }
                p_header++;

                if(rm_option == 1) // 0(not remove), 1(remove)
                {
                    remove(in_path);
                }
            }
        }

        return true;
    }

    return false;
}

FwHeaderInfo* fw_header_get(void)
{
    return &fw_header;
}

