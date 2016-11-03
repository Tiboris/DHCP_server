#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <vector>

using namespace std;

#define PORT                    67
#define MAX_DHCP_CHADDR_LENGTH  16
#define MAX_DHCP_SNAME_LENGTH   64
#define MAX_DHCP_FILE_LENGTH    128
#define MAX_DHCP_OPTIONS_LENGTH 312

typedef struct scope_settings_struct
{
    uint32_t net_addr;
    uint32_t srv_addr;
    uint32_t mask = UINT32_MAX;
    vector<uint32_t> exclude_list;
    uint32_t first_addr;
    uint32_t broadcast;
}scope_settings;

typedef struct struct_dhcp_packet
{
    uint8_t  op;                   /* packet type */
    uint8_t  htype;                /* type of hardware address for client machine */
    uint8_t  hlen;                 /* length of client hardware address */
    uint8_t  hops;                 /* hops */
    uint32_t xid;                  /* random transaction id number - chosen by client */
    uint16_t secs;                 /* seconds used in timing */
    uint16_t flags;                /* flags */
    uint32_t ciaddr;               /* IP address of client machine (if client already have one) */
    uint32_t yiaddr;               /* IP address of client machine (offered by this the DHCP server) */
    uint32_t siaddr;               /* IP address of this DHCP server */
    uint32_t giaddr;               /* IP address of DHCP relay */
    uint8_t chaddr [MAX_DHCP_CHADDR_LENGTH];    /* hardware address of client machine */
    uint8_t sname [MAX_DHCP_SNAME_LENGTH];        /* name of DHCP server */
    uint8_t file [MAX_DHCP_FILE_LENGTH];          /* boot file name (used for diskless booting?) */
	uint8_t options [MAX_DHCP_OPTIONS_LENGTH];    /* options */
} __attribute__ ((packed)) dhcp_packet;

#endif
