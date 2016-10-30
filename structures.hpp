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
    u_int32_t net_addr;
    u_int32_t srv_addr;
    u_int32_t mask = UINT32_MAX;
    vector<u_int32_t> exclude_list;
    u_int32_t first_addr;
    u_int32_t broadcast;
}scope_settings;

typedef struct struct_dhcp_packet
{
    u_int8_t  op;                   /* packet type */
    u_int8_t  htype;                /* type of hardware address for client machine */
    u_int8_t  hlen;                 /* length of client hardware address */
    u_int8_t  hops;                 /* hops */
    u_int32_t xid;                  /* random transaction id number - chosen by client */
    u_int16_t secs;                 /* seconds used in timing */
    u_int16_t flags;                /* flags */
    u_int32_t ciaddr;               /* IP address of client machine (if client already have one) */
    u_int32_t yiaddr;               /* IP address of client machine (offered by this the DHCP server) */
    u_int32_t siaddr;               /* IP address of this DHCP server */
    u_int32_t giaddr;               /* IP address of DHCP relay */
    u_int8_t chaddr [MAX_DHCP_CHADDR_LENGTH];    /* hardware address of client machine */
    u_int8_t sname [MAX_DHCP_SNAME_LENGTH];        /* name of DHCP server */
    u_int8_t file [MAX_DHCP_FILE_LENGTH];          /* boot file name (used for diskless booting?) */
	u_int8_t options [MAX_DHCP_OPTIONS_LENGTH];    /* options */
} __attribute__ ((packed)) dhcp_packet;

#endif
