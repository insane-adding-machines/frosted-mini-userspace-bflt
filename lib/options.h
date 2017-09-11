/* wolfssl options.h
 * generated from configure options
 *
 * Copyright (C) 2006-2015 wolfSSL Inc.
 *
 * This file is part of wolfSSL. (formerly known as CyaSSL)
 *
 */

#ifndef WOLFSSL_OPTIONS_H
#define WOLFSSL_OPTIONS_H


#ifdef __cplusplus
extern "C" {
#endif

#undef  OPENSSL_EXTRA
#define OPENSSL_EXTRA

#ifndef WOLFSSL_OPTIONS_IGNORE_SYS
#undef  _POSIX_THREADS
#define _POSIX_THREADS
#endif

#undef  HAVE_THREAD_LS
#define HAVE_THREAD_LS

#undef  TFM_TIMING_RESISTANT
#define TFM_TIMING_RESISTANT

#undef  ECC_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT

#undef  WC_RSA_BLINDING
#define WC_RSA_BLINDING

#undef  PERSIST_SESSION_CACHE
#define PERSIST_SESSION_CACHE

#undef  PERSIST_CERT_CACHE
#define PERSIST_CERT_CACHE

#undef  HAVE_AESGCM
#define HAVE_AESGCM

#undef  WOLFSSL_SHA512
#define WOLFSSL_SHA512

#undef  WOLFSSL_SHA384
#define WOLFSSL_SHA384

#undef  SESSION_CERTS
#define SESSION_CERTS

#undef  WOLFSSL_KEY_GEN
#define WOLFSSL_KEY_GEN

#undef  WOLFSSL_CERT_GEN
#define WOLFSSL_CERT_GEN

#undef  NO_DSA
#define NO_DSA

#undef  HAVE_ECC
#define HAVE_ECC

#undef  TFM_ECC256
#define TFM_ECC256

#undef  ECC_SHAMIR
#define ECC_SHAMIR

#undef  WOLFSSL_ALLOW_SSLV3
#define WOLFSSL_ALLOW_SSLV3

#undef  WOLFSSL_BASE64_ENCODE
#define WOLFSSL_BASE64_ENCODE

#undef  NO_RC4
#define NO_RC4

#undef  HAVE_WEBSERVER
#define HAVE_WEBSERVER

#undef  NO_HC128
#define NO_HC128

#undef  NO_RABBIT
#define NO_RABBIT

#undef  WOLFSSL_SHA224
#define WOLFSSL_SHA224

#undef  WOLFSSL_SHA3
#define WOLFSSL_SHA3

#undef  HAVE_POLY1305
#define HAVE_POLY1305

#undef  HAVE_ONE_TIME_AUTH
#define HAVE_ONE_TIME_AUTH

#undef  HAVE_CHACHA
#define HAVE_CHACHA

#undef  HAVE_HASHDRBG
#define HAVE_HASHDRBG

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_CERTIFICATE_STATUS_REQUEST
#define HAVE_CERTIFICATE_STATUS_REQUEST

#undef  HAVE_OCSP
#define HAVE_OCSP

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_CERTIFICATE_STATUS_REQUEST_V2
#define HAVE_CERTIFICATE_STATUS_REQUEST_V2

#undef  HAVE_CRL
#define HAVE_CRL

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SESSION_TICKET
#define HAVE_SESSION_TICKET

#undef  HAVE_EXTENDED_MASTER
#define HAVE_EXTENDED_MASTER

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SNI
#define HAVE_SNI

#undef  HAVE_MAX_FRAGMENT
#define HAVE_MAX_FRAGMENT

#undef  HAVE_TRUNCATED_HMAC
#define HAVE_TRUNCATED_HMAC

#undef  HAVE_ALPN
#define HAVE_ALPN

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES

#undef  WOLFSSL_HAPROXY
#define WOLFSSL_HAPROXY

#undef  WOLFSSL_ALWAYS_VERIFY_CB
#define WOLFSSL_ALWAYS_VERIFY_CB

#undef  WOLFSSL_ALWAYS_KEEP_SNI
#define WOLFSSL_ALWAYS_KEEP_SNI

#undef  KEEP_OUR_CERT
#define KEEP_OUR_CERT

#undef  KEEP_PEER_CERT
#define KEEP_PEER_CERT

#undef  HAVE_EXT_CACHE
#define HAVE_EXT_CACHE

#undef  HAVE_EX_DATA
#define HAVE_EX_DATA

#undef  HAVE_STUNNEL
#define HAVE_STUNNEL

#undef  WOLFSSL_ALWAYS_VERIFY_CB
#define WOLFSSL_ALWAYS_VERIFY_CB

#undef  WOLFSSL_ALWAYS_KEEP_SNI
#define WOLFSSL_ALWAYS_KEEP_SNI

#undef  HAVE_EX_DATA
#define HAVE_EX_DATA

#undef  WC_NO_ASYNC_THREADING
#define WC_NO_ASYNC_THREADING

#undef  HAVE___UINT128_T
#define HAVE___UINT128_T


#ifdef __cplusplus
}
#endif


#endif /* WOLFSSL_OPTIONS_H */
