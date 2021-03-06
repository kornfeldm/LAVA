/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_SELF_TEST_H
# define OPENSSL_SELF_TEST_H

# include <openssl/core.h> /* OSSL_CALLBACK */

# ifdef __cplusplus
extern "C" {
# endif

/* The test event phases */
# define OSSL_SELF_TEST_PHASE_NONE     "None"
# define OSSL_SELF_TEST_PHASE_START    "Start"
# define OSSL_SELF_TEST_PHASE_CORRUPT  "Corrupt"
# define OSSL_SELF_TEST_PHASE_PASS     "Pass"
# define OSSL_SELF_TEST_PHASE_FAIL     "Fail"

/* Test event categories */
# define OSSL_SELF_TEST_TYPE_NONE               "None"
# define OSSL_SELF_TEST_TYPE_MODULE_INTEGRITY   "Module_Integrity"
# define OSSL_SELF_TEST_TYPE_INSTALL_INTEGRITY  "Install_Integrity"
# define OSSL_SELF_TEST_TYPE_PCT                "Pairwise_Consistency_Test"
# define OSSL_SELF_TEST_TYPE_KAT_CIPHER         "KAT_Cipher"
# define OSSL_SELF_TEST_TYPE_KAT_DIGEST         "KAT_Digest"
# define OSSL_SELF_TEST_TYPE_KAT_SIGNATURE      "KAT_Signature"
# define OSSL_SELF_TEST_TYPE_KAT_KDF            "KAT_KDF"
# define OSSL_SELF_TEST_TYPE_KAT_KA             "KAT_KA"
# define OSSL_SELF_TEST_TYPE_DRBG               "DRBG"

/* Test event sub categories */
# define OSSL_SELF_TEST_DESC_NONE           "None"
# define OSSL_SELF_TEST_DESC_INTEGRITY_HMAC "HMAC"
# define OSSL_SELF_TEST_DESC_PCT_RSA_PKCS1  "RSA"
# define OSSL_SELF_TEST_DESC_PCT_ECDSA      "ECDSA"
# define OSSL_SELF_TEST_DESC_PCT_DSA        "DSA"
# define OSSL_SELF_TEST_DESC_CIPHER_AES_GCM "AES_GCM"
# define OSSL_SELF_TEST_DESC_CIPHER_TDES    "TDES"
# define OSSL_SELF_TEST_DESC_MD_SHA1        "SHA1"
# define OSSL_SELF_TEST_DESC_MD_SHA2        "SHA2"
# define OSSL_SELF_TEST_DESC_MD_SHA3        "SHA3"
# define OSSL_SELF_TEST_DESC_SIGN_DSA       "DSA"
# define OSSL_SELF_TEST_DESC_SIGN_RSA       "RSA"
# define OSSL_SELF_TEST_DESC_SIGN_ECDSA     "ECDSA"
# define OSSL_SELF_TEST_DESC_DRBG_CTR       "CTR"
# define OSSL_SELF_TEST_DESC_DRBG_HASH      "HASH"
# define OSSL_SELF_TEST_DESC_DRBG_HMAC      "HMAC"
# define OSSL_SELF_TEST_DESC_KA_ECDH        "ECDH"
# define OSSL_SELF_TEST_DESC_KA_ECDSA       "ECDSA"
# define OSSL_SELF_TEST_DESC_KDF_HKDF       "HKDF"

# ifdef __cplusplus
}
# endif

void OSSL_SELF_TEST_set_callback(OPENSSL_CTX *libctx, OSSL_CALLBACK *cb,
                                 void *cbarg);
void OSSL_SELF_TEST_get_callback(OPENSSL_CTX *libctx, OSSL_CALLBACK **cb,
                                 void **cbarg);

#endif /* OPENSSL_SELF_TEST_H */
