#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"

err_t openvpnif_init(struct netif *netif);

void openvpnif_input( void const * argument );
void openvpnif_update_config(struct netif *netif);
void openvpnif_notify_conn_changed(struct netif *netif);

int openvpnif_read(int fd, uint8_t * buffer, uint16_t len);
int openvpnif_write(int fd, uint8_t * buffer, uint16_t len);
int openvpnif_create(void);
void openvpnif_dispose(int fd);

