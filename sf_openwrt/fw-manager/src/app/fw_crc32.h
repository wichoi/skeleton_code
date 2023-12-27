#ifndef __FIRMWARE_CRC32_H__
#define __FIRMWARE_CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif

unsigned int calcCRC(char *mem, unsigned int size, unsigned int CRC, unsigned int *table);
unsigned int getFileCRC(const char* fileName);

#ifdef __cplusplus
}
#endif

#endif
