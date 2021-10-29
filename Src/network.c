#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f7xx_hal.h"
#include "opt.h"
#include "network.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "dns.h"
#include "inet.h"
#include "sockets.h"

void net_init(void)
{
	MX_LWIP_Init();
}

void dnsResoloved(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
	volatile int32_t * resoloved;
	resoloved = callback_arg;
	if(ipaddr->u_addr.ip4.addr == 0 || ipaddr == NULL)
		*resoloved = -1;
	else *resoloved = ipaddr->u_addr.ip4.addr;
}


int net_setDNSServer(uint8_t id, char * ipaddr)
{
	ip_addr_t IP;

	inet_aton(ipaddr,&IP);
	dns_setserver(id,&IP);

	return ER_OK;
}

int net_writeString(int fd, char * string, ...)
{
	uint8_t i = 0;
	uint32_t arg[10];
	char str[512];
	va_list ap;
	va_start(ap, string);
	vsprintf(str, string, ap);

	return lwip_send(fd, (char*)str,strlen((char*)str),MSG_WAITALL);
}

int net_writeChar(int fd, char chr)
{
	char c = chr;
	return send(fd, &c,1,MSG_WAITALL);
}
int net_readChar(int fd, char * chr)
{
	return lwip_read(fd, chr, 1);
}
int net_readLine(int fd, char * buffer, uint16_t * lng)
{
	int retval;
	char chr;
	uint16_t i = 0;

	while(1)
	{
		if((retval = lwip_read(fd, &chr, 1)) <= 0) { buffer[0] = 0; lng = 0; break; }
		if(chr == 0x0D) continue;
		if(chr == 0x0A) break;
		buffer[i] = chr;
		i++;
	}
	buffer[i] = 0;
	if(lng != NULL) *lng = i;

	return retval;
}

int net_writeBuffer(int fd, char * buffer, uint32_t lng)
{
	return send(fd, buffer,lng,MSG_WAITALL);
}

int net_readBuffer(int fd, char * buffer, uint32_t lng)
{
	return lwip_read(fd, buffer, lng);
}

int net_close(int fd)
{
	return lwip_close(fd);
}

int net_accept(int socket_fd, int *accept_fd)
{
	int addr_size;
	struct sockaddr_in isa;
	addr_size = sizeof(isa);
	*accept_fd = accept(socket_fd, (struct sockaddr*)&isa,(socklen_t*)&addr_size);
	return ER_OK;
}

int net_listen(int *fd, uint16_t port)
{

	  int addr_size;
	  struct sockaddr_in sa;

	  int socket_fd = socket(PF_INET, SOCK_STREAM, 0);


	  if ( socket_fd < 0 )
		  return ER_FAIL;

	  memset(&sa, 0, sizeof(struct sockaddr_in));
	  sa.sin_family = AF_INET;
	  sa.sin_addr.s_addr = inet_addr("0.0.0.0");
	  sa.sin_port = htons(port);

	  if(bind(socket_fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == -1)
	  {
		  lwip_close(socket_fd);
		  return ER_FAIL;
	  }




	listen(socket_fd,5);
	*fd = socket_fd;
	return ER_OK;
}

int net_getHostByName(char * host, char * ipaddr)
{
	ip_addr_t IP;
	volatile int32_t resoloved = 0;
	switch(dns_gethostbyname(host, &IP, dnsResoloved, (void *)(&resoloved)))
	{
	case ERR_INPROGRESS:
		while(resoloved == 0) osDelay(1);
		if(resoloved == -1) return ER_DNSFAIL;
		IP.u_addr.ip4.addr = resoloved;
		break;
	case ERR_TIMEOUT:
		return ER_DNSFAIL;
	default: break;
	}
	char ipadd[16];
	sprintf(ipadd,"%d.%d.%d.%d",IP.u_addr.ip4.addr&0xFF,(IP.u_addr.ip4.addr>>8)&0xFF,(IP.u_addr.ip4.addr>>16)&0xFF,(IP.u_addr.ip4.addr>>24)&0xFF);
	strcpy(ipaddr,ipadd);
	return ER_OK;
}

void net_getRemoteIP(int fd, char * ip)
{
	struct sockaddr name;
	socklen_t len = 15;
	lwip_getpeername(fd,&name,&len);
	uint32_t remip = ((struct sockaddr_in*)&name)->sin_addr.s_addr;

	sprintf(ip,"%d.%d.%d.%d",remip&0xFF,(remip>>8)&0xFF,(remip>>16)&0xFF,(remip>>24)&0xFF);

}


int net_connect(int *id, char * host, uint16_t port, uint32_t timeout, uint8_t protocol)
{
	ip_addr_t IP;

	volatile int32_t progress = 0;
	switch(dns_gethostbyname(host, &IP, dnsResoloved, (void *)(&progress)))
	{
	case ERR_INPROGRESS:
		while(progress == 0) osDelay(1);
		if(progress == -1) return ER_DNSFAIL;
		IP.u_addr.ip4.addr = progress;
		break;
	case ERR_TIMEOUT:
		return ER_DNSFAIL;
	}

	 printf("Connecting: %d.%d.%d.%d... \r\n",IP.u_addr.ip4.addr&0xFF,(IP.u_addr.ip4.addr>>8)&0xFF,(IP.u_addr.ip4.addr>>16)&0xFF,(IP.u_addr.ip4.addr>>24)&0xFF);


	 struct sockaddr_in ra;
	 int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	 if(socket_fd < 0)
	 	  return ER_FAIL;
	 memset(&ra, 0, sizeof(struct sockaddr_in));
	 ra.sin_family = AF_INET;
	 ra.sin_addr.s_addr = IP.u_addr.ip4.addr;
	 ra.sin_port = htons(port);
	 if(lwip_connect(socket_fd,(struct sockaddr*)&ra,sizeof(struct sockaddr_in)) < 0)
	 {
		 lwip_close(socket_fd);
		 return ER_FAIL;
	 }
	 *id = socket_fd;

	//tcp_connect(*pcb, &IP, port, NULL);



	return ER_OK;
}
