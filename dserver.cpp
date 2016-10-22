
#include <csignal>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <algorithm>

#include "argparser.cpp"

#define PORT 1167
#define MAX_OCTET_CNT 4
#define MAX_OCTET_NUM 255

using namespace std;

#define MAX_DHCP_CHADDR_LENGTH           16
#define MAX_DHCP_SNAME_LENGTH            64
#define MAX_DHCP_FILE_LENGTH             128
#define MAX_DHCP_OPTIONS_LENGTH          312

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
    unsigned char chaddr [MAX_DHCP_CHADDR_LENGTH];      /* hardware address of client machine */
    char sname [MAX_DHCP_SNAME_LENGTH];    /* name of DHCP server */
    char file [MAX_DHCP_FILE_LENGTH];      /* boot file name (used for diskless booting?) */
	char options [MAX_DHCP_OPTIONS_LENGTH];  /* options */
}dhcp_packet;

/*
*   Main
*/
u_int32_t get_ip_addr(scope_settings* scope, u_int32_t ip);
void sig_handler(int signal);

int main(int argc, char** argv)
{
    signal(SIGINT, sig_handler);
    scope_settings scope;
    if ( opt_err(argc, argv, &scope) )
    {
        return EXIT_FAILURE;
    }

    struct in_addr ip_addr;
    ip_addr.s_addr = scope.network_addr ;
    printf("The NW is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.dhcp_srv_addr ;
    printf("The IP is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.mask ;
    printf("The MS is %s\n", inet_ntoa(ip_addr));
    if (scope.exclude)
    {
        cout<< "IP EXCLUDE LIST:\n";
        for (auto item=scope.exclude_list.begin(); item<scope.exclude_list.end(); item++)
        {
            ip_addr.s_addr = *item;
            printf("\t%s\n", inet_ntoa(ip_addr));
        }
    }
    ip_addr.s_addr = scope.first_addr ;
    printf("The FR is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.broadcast ;
    printf("The BR is %s\n", inet_ntoa(ip_addr));

    ip_addr.s_addr = get_ip_addr(&scope, scope.first_addr);
    printf("Offers: %s\n", inet_ntoa(ip_addr));

    if (scope.exclude)
    {
        cout<< "IP EXCLUDE LIST:\n";
        for (auto item=scope.exclude_list.begin(); item<scope.exclude_list.end(); item++)
        {
            ip_addr.s_addr = *item;
            printf("\t%s\n", inet_ntoa(ip_addr));
        }
    }
    while (true)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

void sig_handler(int signal)
{
    cout << "\nInterrupt signal (" << signal << ") received.\n";
    exit(signal);
}

u_int32_t get_ip_addr(scope_settings* scope, u_int32_t ip)
{
    u_int32_t offered_ip = ip;
    if (ip == scope->broadcast) //
    {
        return UINT32_MAX;
    }
    else if (scope->exclude_list.empty())
    {
        scope->exclude = true;
        scope->exclude_list.insert(scope->exclude_list.end(), offered_ip);
        return offered_ip;
    }
    else if (find(scope->exclude_list.begin(), scope->exclude_list.end(), offered_ip) != scope->exclude_list.end())
    {
        offered_ip = htonl(offered_ip);
        offered_ip++;
        offered_ip = htonl(offered_ip);
        return get_ip_addr(scope, offered_ip);
    }
    else
    {
        scope->exclude_list.insert(scope->exclude_list.end(), offered_ip);
        return offered_ip;
    }
}
