#ifndef __APP_HEADER_H__
#define __APP_HEADER_H__

#define CRC32_KEY       0xE1E1E1E1

typedef struct header_info_tag
{
    u8 in_file[256];
    u8 model[256];
    u8 ver_major;
    u8 ver_minor;
    u8 ver_build;
    u8 is_crypto;
    u32 offset;
    u8 out_file[256];
} info_t;

u32 app_proc(info_t *info);
u32 app_crc32(u8 const * p_data, u32 size, u32 const * p_crc);
u32 app_encrypt(char *in_file, char *out_file);
u32 app_decrypt(char *in_file, char *out_file);

#endif /* __APP_HEADER_H__ */
