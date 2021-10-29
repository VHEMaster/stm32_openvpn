#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"

#define TCP 0
#define UDP 1

#define ER_OK 0
#define ER_FAIL -1
#define ER_DNSFAIL -2
#define ER_TIMEOUT -3

typedef void (*connect_t)(int fd, char * remip, uint16_t remport);


void net_init(void);
int net_listen(int *fd, uint16_t port);
int net_accept(int socket_fd, int *accept_fd);
int net_writeString(int fd, char * string, ...);
int net_writeChar(int fd, char chr);
int net_writeBuffer(int fd, char * buffer, uint32_t lng);
int net_readChar(int fd, char * chr);
int net_readBuffer(int fd, char * buffer, uint32_t lng);
int net_readLine(int fd, char * buffer, uint16_t * lng);
int net_setDNSServer(uint8_t id, char * ipaddr);
int net_getHostByName(char * host, char * ipaddr);
int net_close(int fd);
int net_connect(int *id, char * host, uint16_t port, uint32_t timeout, uint8_t protocol);
void net_getRemoteIP(int fd, char * ip);
