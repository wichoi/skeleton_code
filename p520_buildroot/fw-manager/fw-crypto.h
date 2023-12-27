#ifndef __FIRMWARE_CRYPTO_H__
#define __FIRMWARE_CRYPTO_H__

#ifdef __cplusplus
extern "C" {
#endif

bool AproEncryptAes(const char *in_file, const char *out_file);
bool AproDecryptAes(const char *in_file, const char *out_file);
#if 0
bool fw_encrypto_aes(const char *in_file, const char *out_file);
bool fw_decrypto_aes(const char *in_file, const char *out_file);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FIRMWARE_CRYPTO_H__ */
