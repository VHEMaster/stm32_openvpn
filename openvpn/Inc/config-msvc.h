#include "config-msvc-version.h"
#include "basic.h"

#ifndef OPENVPN_CONFIG_EXEC
#define OPENVPN_CONFIG_EXEC

#define CONFIGURE_DEFINES "N/A"

#define ENABLE_DEF_AUTH 1
//#define ENABLE_PF 1
#define ENABLE_CRYPTO 1
#define ENABLE_CRYPTO_MBEDTLS 1
#define ENABLE_DEBUG 1
#define ENABLE_EUREPHIA 1
#define ENABLE_FRAGMENT 1
#define ENABLE_HTTP_PROXY 1
//#define ENABLE_ASYNC_PUSH 1
//#define ENABLE_LZ4 1
//#define NEED_COMPAT_LZ4 1
#define ENABLE_MANAGEMENT 1
#define ENABLE_MULTIHOME 1
#define ENABLE_PORT_SHARE 1
#define ENABLE_SOCKS 1
//#define ENABLE_CLIENT_CR 1

#define HAVE_ERRNO_H 1
#define HAVE_CTYPE_H 1
#define HAVE_STDARG_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRDUP 1
#define HAVE_STRERROR 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_LIMITS_H 1
#define HAVE_TIME 1
#define HAVE_TIME_H 1
#define HAVE_VSNPRINTF 1

#define HAVE_ACCESS 1
#define HAVE_CHDIR 1
#define HAVE_CHSIZE 1
#define HAVE_CPP_VARARG_MACRO_ISO 1
#define HAVE_CTIME 1
#define HAVE_EVP_CIPHER_CTX_SET_KEY_LENGTH 1
//#define HAVE_IN_PKTINFO 1
#define HAVE_MEMSET 1
#define HAVE_PUTENV 1
//#define HAVE_STAT 1

#define HAVE_GETTIMEOFDAY_NANOSECONDS 1

#define HAVE_SOCKET 1
#define HAVE_RECV 1
#define HAVE_RECVFROM 1
#define HAVE_SEND 1
#define HAVE_SENDTO 1
#define HAVE_LISTEN 1
#define HAVE_ACCEPT 1
#define HAVE_CONNECT 1
#define HAVE_BIND 1
#define HAVE_SELECT 1
#define HAVE_GETHOSTBYNAME 1
#define HAVE_INET_NTOA 1
//#define HAVE_SETSOCKOPT 1
//#define HAVE_GETSOCKOPT 1
#define HAVE_GETSOCKNAME 1
//#define HAVE_POLL 1

#define HAVE_OPENSSL_ENGINE 1

#define PATH_SEPARATOR     '\\'
#define PATH_SEPARATOR_STR "\\"

#ifndef __cplusplus
#define inline __inline
#endif

#define EMPTY_ARRAY_SIZE 1

#define HAVE_DECL_SO_MARK 0

#define strncasecmp wal_strnicmp
#define strcasecmp wal_stricmp

static int wal_stricmp(const char *a, const char *b) {
  int ca, cb;
  do {
     ca = (unsigned char) *a++;
     cb = (unsigned char) *b++;
     ca = (ca >= 'a' && ca <= 'z') ? ca - ('a'-'A') : ca;
     cb = (cb >= 'a' && cb <= 'z') ? cb - ('a'-'A') : cb;
   } while (ca == cb && ca != '\0');
   return ca - cb;
}

static int wal_strnicmp(const char *a, const char *b, size_t count) {
  int ca, cb;
	int length = 0;
  do {
		 if(++length > count) return 1;
     ca = (unsigned char) *a++;
     cb = (unsigned char) *b++;
     ca = (ca >= 'a' && ca <= 'z') ? ca - ('a'-'A') : ca;
     cb = (cb >= 'a' && cb <= 'z') ? cb - ('a'-'A') : cb;
   } while (ca == cb && ca != '\0');
   return ca - cb;
}

#define S_IRUSR 0
#define S_IWUSR 0
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0

#define SIGHUP    1
#define SIGINT    2
#define SIGUSR1   10
#define SIGUSR2   12
#define SIGTERM   15


#define HAVE_INET_NTOP
#define HAVE_INET_PTON


#endif
