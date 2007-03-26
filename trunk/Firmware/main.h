
#define MIN(x,y) ((x > y) ? y : x)

void UDP_RX_process(void);

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#define IPBUF ((struct ethip_hdr *)&uip_buf[0])   
#define UWSDR_UDP_FRAMELEN   (1200 + sizeof(t_codec_hdr))
#define NTOHS(x) (((x[0] & 0xFF) << 8) | ((x[1] & 0xFF) >> 8) )
