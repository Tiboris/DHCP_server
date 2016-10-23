
#include <csignal>

#include <iostream>

#include <fstream>

#include <string>

#include <string.h>

#include <strings.h>

#include <regex>

#include <netinet/in.h>

#include <unistd.h>

#include <netdb.h>

#include <stdio.h>

#include <stdlib.h>

#include <arpa/inet.h>

#include <algorithm>

#include <thread>

#include "argparser.cpp"

#define PORT 67
#define MAX_OCTET_CNT 4
#define MAX_OCTET_NUM 255

using namespace std;
#define SIZE 9
#define MAX_CLIENTS 8
#define MAX_DHCP_CHADDR_LENGTH           16
#define MAX_DHCP_SNAME_LENGTH            64
#define MAX_DHCP_FILE_LENGTH             128
#define MAX_DHCP_OPTIONS_LENGTH          312

int sock = -1; // socket has to be closed after SIGINT;

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
    unsigned char chaddr [MAX_DHCP_CHADDR_LENGTH];  /* hardware address of client machine */
    char sname [MAX_DHCP_SNAME_LENGTH];        /* name of DHCP server */
    char file [MAX_DHCP_FILE_LENGTH];          /* boot file name (used for diskless booting?) */
	char options [MAX_DHCP_OPTIONS_LENGTH];    /* options */
}dhcp_packet;

u_int32_t get_ip_addr(scope_settings* scope, u_int32_t ip);
bool item_in_list(u_int32_t item, vector<u_int32_t> list);
void sig_handler(int signal);
bool listen(scope_settings scope);
int create_socket();bool
begin_listen(scope_settings scope, int* sock);
void handle_request(int cli_socket);
/*
*   Main
*/
int main(int argc, char** argv)
{
    signal(SIGINT, sig_handler);
    scope_settings scope;
    if ( opt_err(argc, argv, &scope) )
    {// if there are wrong arguments program fails
        return EXIT_FAILURE;
    }

    struct in_addr ip_addr;
    ip_addr.s_addr = scope.network_addr ;
    printf("The NW is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.dhcp_srv_addr ;
    printf("The IP is %s\n", inet_ntoa(ip_addr));
    ip_addr.s_addr = scope.mask ;
    printf("The MS is %s\n", inet_ntoa(ip_addr));
    if (scope.exclude_list.begin()!=scope.exclude_list.end())
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

    if (scope.exclude_list.begin()!=scope.exclude_list.end())
    {
        cout<< "IP EXCLUDE LIST:\n";
        for (auto item=scope.exclude_list.begin(); item<scope.exclude_list.end(); item++)
        {
            ip_addr.s_addr = *item;
            printf("\t%s\n", inet_ntoa(ip_addr));
        }
    }
    printf("\nBEGIN LISTEN:\n\n");
    return begin_listen(scope, &sock);
}

void handle_request(int cli_socket)
{
    exit(0);
}

bool begin_listen(scope_settings scope, int* sock)
{
    *sock = create_socket();
    if (*sock == -1)
    {
        cerr<< "ERR creating socket\n";
        return EXIT_FAILURE;
    }
    struct sockaddr_in c_addr;
    // Start listening to clients max 10 9 in queue on socket s
    listen(*sock,MAX_CLIENTS);
    socklen_t c_len = sizeof(c_addr);
    char buff[SIZE];
    // handling server run
    while (true)
    {
        int cli_socket = recvfrom(*sock, buff, SIZE, 0, (struct sockaddr *)&c_addr, &c_len);
        if (cli_socket >= 0)
        {
            printf("Request received from %s, port %d\n",
            inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
        }
        else
        {
            cerr << "ERR on recv\n";
            continue;
        }
        printf("Message: \"%.*s\"\n",cli_socket,buff);
        if (strncmp(buff,"END.",4) == 0)
        {    // "END." string exits the application
            printf("closing socket\n");
            close(*sock);
        }
        // let thread handle client
        //thread t (handle_request, cli_socket);
        //t.detach();
    }
    return EXIT_SUCCESS;
}

int create_socket()
{// copied from my IPK project2 and edited
    int sockfd;
    struct sockaddr_in server_addr;
    // First call socket() function
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        return -1;
    }
    // Initialize socket structure
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    // Binding socket
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    }
    // Returning binded socket
    return sockfd;
}


void sig_handler(int signal)
{
    cout << "\nInterrupt signal (" << signal << ") received...\n";
    if (sock != -1 )
    {
        cout << "Closing socket...\n";
        close(sock);
    }
    exit(EXIT_SUCCESS);
}

u_int32_t get_ip_addr(scope_settings* scope, u_int32_t ip)
{
    u_int32_t offered_ip = ip;
    if (ip == scope->broadcast)
    {   // when free address is broadcast we are out of addresses in scope
        return UINT32_MAX;
    }
    else if (item_in_list(offered_ip, scope->exclude_list))
    {   // when address is already in use or in exclude_list we try again other
        offered_ip = htonl(offered_ip);
        offered_ip++; // next address might be usable
        offered_ip = htonl(offered_ip);
        return get_ip_addr(scope, offered_ip);
    }
    else
    {   // return first usable address and then add it to exclude_list
        scope->exclude_list.insert(scope->exclude_list.end(), offered_ip);
        return offered_ip;
    }
}

bool item_in_list( u_int32_t item, vector<u_int32_t> list)
{// returns true when item in list
    return (find(list.begin(), list.end(), item) != list.end());
}
