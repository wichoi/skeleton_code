#include "fw_dbg.h"
#include "fw_crc32.h"

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
    char buf [32768];
    unsigned int CRC = 0;
    unsigned int table[256];
    size_t len;

    if (fileName == NULL)
    {
        log_print(LOG_E, "[Firm crc] Error file Name Error!!\n");
        return 0;
    }

    fd = fopen(fileName, "rb");
    if (fd == NULL)
    {
        log_print(LOG_E, "[Firm crc] Error file open Error!!\n");
        return 0;
    }

    makeCRCtable(table, 0xEDB88320);

    while ( (len = fread(buf, 1, sizeof(buf), fd)) > 0 )
        CRC = calcCRC(buf, (unsigned int) len, CRC, table);

    fclose(fd);
    log_print(LOG_V, "CRC [%d]\n", CRC);
    return CRC;
}


