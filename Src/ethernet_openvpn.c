#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernet_openvpn.h"
#include <string.h>
#include "cmsis_os.h"

/* Private define ------------------------------------------------------------*/
/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT ( portMAX_DELAY )
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE ( ETH_MAX_PACKET_SIZE * 2 )

/* Network interface name */
#define IFNAME0 'v'
#define IFNAME1 'p'

struct vpn_buffer {
	uint16_t len;
	uint8_t buffer[ETH_MAX_PACKET_SIZE];
};

struct netif gvpnif;
ip4_addr_t vpn_ipaddr;
ip4_addr_t vpn_netmask;
ip4_addr_t vpn_gw;

xQueueHandle qTx;
xQueueHandle qRx;
osThreadId ethInTask;

int openvpnif_create(void)
{
	//vpn_ipaddr.addr = 0x0100A8C0;
	//vpn_netmask.addr = 0x00FFFFFF;
	//vpn_gw.addr = 0;
	
  netif_add(&gvpnif, &vpn_ipaddr, &vpn_netmask, &vpn_gw, NULL, &openvpnif_init, &tcpip_input);

  if (netif_is_link_up(&gvpnif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gvpnif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gvpnif);
  }
	
	dhcp_start(&gvpnif);
	
	return MEMP_NUM_NETCONN-1;
}

void openvpnif_dispose(int fd)
{
	netif_remove(&gvpnif);
	xQueueReset(qRx);
	xQueueReset(qTx);
	vTaskDelete(ethInTask);
}

int openvpnif_write(int fd, uint8_t * buffer, uint16_t len)
{
	struct vpn_buffer vpn_buffer;
	//memcpy(vpn_buffer.buffer, buffer, len);
	for(int i = 0; i < len; i++)
		vpn_buffer.buffer[i] = buffer[i];
	
	vpn_buffer.len = len;
	xQueueSend(qTx, (void*)&vpn_buffer, portMAX_DELAY);
	
	return len;
}

int openvpnif_read(int fd, uint8_t * buffer, uint16_t len)
{
	struct vpn_buffer vpn_buffer;
  if(xQueueReceive(qRx, &vpn_buffer, 0) == errQUEUE_EMPTY)
		return 0;
	
	if(vpn_buffer.len > len)
		return -1;
	
	//memcpy(buffer, vpn_buffer.buffer, vpn_buffer.len);
	for(int i = 0; i < vpn_buffer.len; i++)
		buffer[i] = vpn_buffer.buffer[i];
	
	return vpn_buffer.len;
}
static void low_level_init(struct netif *netif)
{ 
  
/* Init ETH */

  uint8_t MACAddr[6];
	
	
  MACAddr[0] = 0x90;
  MACAddr[1] = 0xFF;
  MACAddr[2] = 0xDE;
  MACAddr[3] = 0xAD;
  MACAddr[4] = 0xDE;
  MACAddr[5] = 0xAD;

  netif->flags |= NETIF_FLAG_LINK_UP;

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;
  
  /* set MAC hardware address */
  netif->hwaddr[0] =  MACAddr[0];
  netif->hwaddr[1] =  MACAddr[1];
  netif->hwaddr[2] =  MACAddr[2];
  netif->hwaddr[3] =  MACAddr[3];
  netif->hwaddr[4] =  MACAddr[4];
  netif->hwaddr[5] =  MACAddr[5];
  
  /* maximum transfer unit */
  netif->mtu = 1500;
	
	if(qRx == NULL)
		qRx = xQueueCreate(32, sizeof(struct vpn_buffer));
	
	if(qTx == NULL)
		qTx = xQueueCreate(32, sizeof(struct vpn_buffer));
  
  /* Accept broadcast address and ARP traffic */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  
  osThreadDef(VpnIf, openvpnif_input, osPriorityRealtime, 0, INTERFACE_THREAD_STACK_SIZE);
  ethInTask = osThreadCreate (osThread(VpnIf), netif);

  
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t openvpnif_low_level_output(struct netif *netif, struct pbuf *p)
{
  err_t errval;
  struct pbuf *q;
	struct vpn_buffer vpn_buffer;
  uint8_t *buffer = vpn_buffer.buffer;
  uint32_t framelength = 0;
  uint32_t bufferoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t payloadoffset = 0;
  bufferoffset = 0;
	
  /* copy frame from pbufs to driver buffers */
  for(q = p; q != NULL; q = q->next)
    {
      /* Get bytes in current lwIP buffer */
      byteslefttocopy = q->len;
      payloadoffset = 0;
    
      /* Copy the remaining bytes */
      memcpy( (uint8_t*)((uint8_t*)buffer + bufferoffset), (uint8_t*)((uint8_t*)q->payload + payloadoffset), byteslefttocopy );
      bufferoffset = bufferoffset + byteslefttocopy;
      framelength = framelength + byteslefttocopy;
    }
  
	vpn_buffer.len = framelength;
		
  errval = ERR_OK;
		
	if(xQueueSend(qRx, &vpn_buffer, portMAX_DELAY) == errQUEUE_FULL)
	{
		errval = ERR_BUF;
	}
  
  
  
  return errval;
}
/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
   */
static struct pbuf * openvpnif_low_level_input(struct netif *netif, struct vpn_buffer vpn_buffer)
{
  struct pbuf *p = NULL;
  struct pbuf *q = NULL;
	
  uint16_t len = vpn_buffer.len;
  uint8_t *buffer = vpn_buffer.buffer;
	
  uint32_t bufferoffset = 0;
  uint32_t payloadoffset = 0;
  uint32_t byteslefttocopy = 0;
  
  if (len > 0)
  {
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  }
  
  if (p != NULL)
  {
    bufferoffset = 0;
    for(q = p; q != NULL; q = q->next)
    {
      byteslefttocopy = q->len;
      payloadoffset = 0;
      
      memcpy( (uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)buffer + bufferoffset), byteslefttocopy);
      bufferoffset = bufferoffset + byteslefttocopy;
    }
  }  
  
  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void openvpnif_input( void const * argument ) 
{
  struct pbuf *p;
  struct netif *netif = (struct netif *) argument;
	struct vpn_buffer vpn_buffer;
  
  for( ;; )
  {
		if(xQueueReceive(qTx, &vpn_buffer, portMAX_DELAY) == pdPASS)
		{ 
        p = openvpnif_low_level_input(netif, vpn_buffer);
        if   (p != NULL)
        {
          if (netif->input( p, netif) != ERR_OK )
          {
            pbuf_free(p);
          }
        }
    }
  }
}


/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t openvpnif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "STM32F7_VPN";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
  netif->output = etharp_output;
#else
  /* The user should write ist own code in low_level_output_arp_off function */
  netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */
 
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

  netif->linkoutput = openvpnif_low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

__weak void openvpnif_tx_callback(uint8_t * buffer, uint16_t len)
{
  /* NOTE : This is function could be implemented in user file 
            when the callback is needed,
  */

}

/* USER CODE BEGIN 6 */


/* USER CODE END 6 */

/* USER CODE BEGIN 7 */

/* USER CODE END 7 */

#if LWIP_NETIF_LINK_CALLBACK
/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
* @param  netif: The network interface
  * @retval None
  */
void openvpnif_update_config(struct netif *netif)
{
  __IO uint32_t tickstart = 0;
  uint32_t regvalue = 0;
  
  if(netif_is_link_up(netif))
  { 
    
  }
  else
  {
		
  }

  openvpnif_notify_conn_changed(netif);
}

/* USER CODE BEGIN 8 */
/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
__weak void openvpnif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function could be implemented in user file 
            when the callback is needed,
  */

}

/* USER CODE END 8 */ 
#endif /* LWIP_NETIF_LINK_CALLBACK */

/* USER CODE BEGIN 9 */

/* USER CODE END 9 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


