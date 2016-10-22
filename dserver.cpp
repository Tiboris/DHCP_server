
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

typedef struct struct_ip_addr_scope_in
{
    u_int32_t network_addr;
    u_int32_t dhcp_srv_addr;
    u_int32_t mask = UINT32_MAX;
    bool exclude = false;
    vector<u_int32_t> exclude_list;
    u_int32_t free_addr;
    u_int32_t broadcast;
}scope_in;

//scope_in set_scope(settings scope);
/*
*   Main
*/

void sig_handler(int signal);

int main(int argc, char** argv)
{
    signal(SIGINT, sig_handler);
    scope_settings scope;
    if ( opt_err(argc, argv, &scope) )
    {
        return EXIT_FAILURE;
    }
    printf("lalala\n" );
    //scope = set_scope(scope);
    cout<<"Network: \t"<<scope.network_addr<<"\nCIRD Mask:\t"<< scope.mask<<endl;
    if (scope.exclude)
    {
        cout<< "IP EXCLUDE LIST:";
        for (auto it=scope.exclude_list.begin(); it<scope.exclude_list.end(); it++)
        {
            cout  <<*it << "\n\t\t";
            //printf("%s\n", inet_ntoa(*it));
        }
    }

    struct in_addr ip_addr;
    ip_addr.s_addr = scope.network_addr ;
    printf("\nThe NW is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.dhcp_srv_addr ;
    printf("The IP is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.mask ;
    printf("The MS is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.exclude ;
    printf("The E is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.free_addr ;
    printf("The FR is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.broadcast ;
    printf("The BR is %s\n", inet_ntoa(ip_addr));

    while (true)
    {
        /* code */
    }
    return EXIT_SUCCESS;
}

void sig_handler(int signal)
{
    cout << "\nInterrupt signal (" << signal << ") received.\n";
    exit(signal);
}
