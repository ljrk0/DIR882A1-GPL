#ifndef __LIBOPENSSL_INTERNAL_H__
#define __LIBOPENSSL_INTERNAL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif 
#endif 

int libopenssl_initialize(void);
int libopenssl_terminate(void);

int libopenssl_dgst_compute_signature(char* dgstalgorithm, char* privatekeyname, char *passwd, char* datafile, char* signaturefile);
int libopenssl_dgst_compute_digest(char* dgstalgorithm, char* datafile, char* signaturefile);
int libopenssl_dgst_verify_signature(char* dgstalgorithm, char* resultfile, char* publickeyname, char* signaturefile, char* datafile);


int libopenssl_pkcs12_output_clcerts(char* filename, char* passwd, char* dstClCert, char* hnbId);
int libopenssl_pkcs12_output_clcerts_with_name(char* filename, char* passwd, char* dstClCert);
int libopenssl_pkcs12_output_cacerts(char* filename, char* passwd, char* dstCaCert, char* hnbId);
int libopenssl_pkcs12_output_cacerts_with_name(char* filename, char* passwd, char* dstCaCert);
int libopenssl_pkcs12_output_keys(char* filename, char* passwd, char* dstPriKey, char* hnbId, char* passwdout);
int libopenssl_pkcs12_output_keys_with_name(char* filename, char* passwd, char* dstPriKey, char* passwdout);
int libopenssl_pkcs12_output(char* filename, char* passwd, char* dstCert, char* dstKey, char* dstCa, char* hnbId, char* passwdout);
int libopenssl_pkcs12_create(char* certFileName, char* privateKeyFileName, char* caFileName, char* p12FileName, char* passwdIn, char* passwdOut);

int libopenssl_req_generate_self_signed_ceritficate(char *pCaKeyPath, unsigned int keyLength, char *pCaCertPath, char *pConfigFile, unsigned int daysUsed);
int libopenssl_req_generate_ceritficate_request(char *pPrivKeyPath, unsigned int keyLength, char *pCertPath, char *pConfigFile);

int libopenssl_genrsa_generate_private_key(char* privatekeyname, int length, char* encrypt, char* passwdOut, char* randfilename);

int libopenssl_rsa_generate_public_key(char* privatekeyname, char* publickeyname, char* passwdin);

int libopenssl_rand_generate_random_file(char* randomFile, int length);

int libopenssl_verify_verify_certificate_chains(char* caname, char* certname);

int libopenssl_x509_output_certificate_info(char* certname, char*infofile);

int libopenssl_enc_encrypt_file_by_aes256cbc(char* aeskey_hex, char* infile, char* outfile);
int libopenssl_enc_decrypt_file_by_aes256cbc(char* aeskey_hex, char* infile, char* outfile);

#ifdef __cplusplus
#if __cplusplus
        }
#endif 
#endif/* __cpluscplus */


#endif /* __LIBOPENSSL_INTERNAL_H__ */

