#include "fw-dbg.h"
#include "fw-crc32.h"

unsigned int calcCRC(char *mem, unsigned int size, unsigned int CRC, unsigned int *table)
{
    CRC = ~CRC;
    while(size--)
        CRC = table[(CRC ^ *(mem++)) & 0xFF] ^ (CRC >> 8);
    return ~CRC;
}

static void makeCRCtable(unsigned int *table, unsigned int id)
{
    unsigned int i, j, k;
    for(i = 0; i < 256; ++i)
    {
        k = i;
        for(j = 0; j < 8; ++j)
        {
            if (k & 1)
                k = (k >> 1) ^ id;
            else
                k >>= 1;
        }
        table[i] = k;
    }
}

unsigned int getFileCRC(const char* fileName)
{
    FILE *fd = NULL;
    char buf[32768] = {0,};
    unsigned int CRC = 0;
    unsigned int table[256] = {0,};
    unsigned int read_offset = 0;
    unsigned int read_len = 0;
    unsigned int result_len = 0;
    unsigned int file_end = 0;

    if(fileName == NULL)
    {
        log_print(LOG_E, "[Firm crc] Error file Name Error!!\n");
        return 0;
    }

    fd = fopen(fileName, "rb");
    if(fd == NULL)
    {
        log_print(LOG_E, "[Firm crc] Error file open Error!!\n");
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    file_end = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    makeCRCtable(table, 0xEDB88320);

    while(1)
    {
        if(read_offset + sizeof(buf) < file_end)
        {
            read_len = sizeof(buf);
        }
        else
        {
            read_len = file_end - read_offset;
        }

        if((result_len = fread(buf, 1, read_len, fd)) > 0)
        {
            CRC = calcCRC(buf, result_len, CRC, table);
        }

        read_offset += read_len;
        if(read_offset >= file_end)
        {
            break;
        }
    }

    fclose(fd);
    log_print(LOG_V, "CRC [%d]\n", CRC);
    return CRC;
}

unsigned int getFileCrcOffset(const char* fileName, int file_begin, int file_end)
{
    FILE *fd = NULL;
    char buf[32768] = {0,};
    unsigned int CRC = 0;
    unsigned int table[256] = {0,};
    unsigned int read_offset = file_begin;
    unsigned int read_len = 0;
    unsigned int result_len = 0;

    if(fileName == NULL)
    {
        log_print(LOG_E, "[Firm crc] Error file Name Error!!\n");
        return 0;
    }

    fd = fopen(fileName, "rb");
    if(fd == NULL)
    {
        log_print(LOG_E, "[Firm crc] Error file open Error!!\n");
        return 0;
    }

    fseek(fd, file_begin, SEEK_SET);

    makeCRCtable(table, 0xEDB88320);

    while(1)
    {
        if(read_offset + sizeof(buf) < file_end)
        {
            read_len = sizeof(buf);
        }
        else
        {
            read_len = file_end - read_offset;
        }

        if((result_len = fread(buf, 1, read_len, fd)) > 0)
        {
            CRC = calcCRC(buf, result_len, CRC, table);
        }

        read_offset += read_len;
        if(read_offset >= file_end)
        {
            break;
        }
    }

    fclose(fd);
    log_print(LOG_V, "CRC [%d]\n", CRC);
    return CRC;
}

