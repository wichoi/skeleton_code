#ifndef __FIRMWARE_MANAGER_H__
#define __FIRMWARE_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

// Header size
#define FW_HEADER_SIZE              1024
#define FW_HEADER_NAME_SIZE         128
#define FW_HEADER_INT_SIZE          4
#define FW_HEADER_INFO_SIZE         132 // header_info(128) + file_cnt(4)

// Firmware define
#define FW_FILE_MAX                 10
#define FW_FILENAME_SIZE            48
#define FW_CRC32_SIZE               16

typedef enum tagFwFileCryption
{
    FW_FILE_NORMAL = 0,
    FW_FILE_CRYPTION = 1,
} FwCryptoType;

typedef struct tagFwFileInfo
{
    char            file_name[FW_FILENAME_SIZE];
    char            crc32[FW_CRC32_SIZE];
    unsigned int    file_size;     //  Network Byte Order
} FwFileInfo;

typedef struct tagFwHeaderInfo
{
    unsigned char   header_info[FW_HEADER_NAME_SIZE];
    unsigned int    file_cnt; //  Network Byte Order
    unsigned char   fw_files[FW_HEADER_SIZE - FW_HEADER_INFO_SIZE]; // FwFileInfo * FW_FILE_MAX
} FwHeaderInfo;

int fw_Init(void);
int fw_Append(const char *file_name);
int fw_Merge(const char *file_name, FwCryptoType crypt);

void fw_sp_init(const char* strPath);
bool fw_Splite(const char* strFileName, int rm_option);
FwHeaderInfo* fw_header_get(void);

#ifdef __cplusplus
}
#endif

#endif
