#ifndef __FIRMWARE_MANAGER_API_H__
#define __FIRMWARE_MANAGER_API_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fw_file_tag
{
    char            name[64];
    char            version[32];
    unsigned int    crc32;
    unsigned int    length;
} fw_file_t;

typedef struct fw_header_tag
{
    char            header_info[128];
    unsigned int    file_cnt;
    fw_file_t       fw_file[10];
} fw_header_t;

typedef enum fw_rm_option_tag
{
    FW_NOT_REMOVE           = 0,
    FW_REMOVE               = 1,
} fw_opt_e;

int fwmgr_splite(char *fw_path, char *fw_name, fw_header_t *p_header, fw_opt_e rm_opt);

#ifdef __cplusplus
}
#endif

#endif
