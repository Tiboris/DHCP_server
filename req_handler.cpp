#include "req_handler.hpp"
/*
 *  request handler for dserver.cpp as school project to ISA class
 */
using namespace std;

bool handle_request(scope_settings* scope, int* s)
{
    if ((*s = create_socket()) == -1)
    {
        cerr<< "ERR creating socket\n";
        return EXIT_FAILURE;
    }
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);
    // handling server run
    while (true)
    {
        int r;
        dhcp_packet p;
        r = recvfrom(*s, &p, sizeof(p), 0, (struct sockaddr*)&c_addr, &c_len);
        if (r < 0)
        {
            cerr << "ERR on recv\n";
            continue;
        }
        u_int8_t client_chaddr [MAX_DHCP_CHADDR_LENGTH];
        memcpy(client_chaddr, p.chaddr, MAX_DHCP_CHADDR_LENGTH);
        cout<<client_chaddr[0]<<endl;
        if (err_set_offer(scope, &p))
        {
            cerr << "ERR on send\n";
            continue;
        }
        struct sockaddr_in br_addr;
        if (true)
        {
            br_addr.sin_family = AF_INET;                     // set IPv4 addressing
            br_addr.sin_addr.s_addr = scope->broadcast;       // broadcast addrs
            br_addr.sin_port = htons(PORT+1);                 // the client listens on this port
            int on = 1;
            if ((setsockopt(*s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on))) == -1)
                return EXIT_FAILURE;
            else
                printf("OK BR is set\n");
        }
        r = sendto(*s, &p, sizeof(p), 0, (struct sockaddr*)&br_addr, c_len);
        printf("%u\n",p.op );
        if (r < 0)
        {
            cerr << "ERR on sendto\n";
            continue;
        }
        struct in_addr ip_addr;
        ip_addr.s_addr = p.yiaddr ;
        printf("IP %s, port %d\n",
        inet_ntoa(ip_addr),ntohs(c_addr.sin_port));
        //return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
// typedef struct struct_dhcp_packet
// {
//     u_int8_t  op;                   /* packet type */
//     u_int8_t  htype;                /* type of hardware address for client machine */
//     u_int8_t  hlen;                 /* length of client hardware address */
//     u_int8_t  hops;                 /* hops */
//     u_int32_t xid;                  /* random transaction id number - chosen by client */
//     u_int16_t secs;                 /* seconds used in timing */
//     u_int16_t flags;                /* flags */
//     u_int32_t ciaddr;               /* IP address of client machine (if client already have one) */
//     u_int32_t yiaddr;               /* IP address of client machine (offered by this the DHCP server) */
//     u_int32_t siaddr;               /* IP address of this DHCP server */
//     u_int32_t giaddr;               /* IP address of DHCP relay */
//     u_int8_t chaddr [MAX_DHCP_CHADDR_LENGTH];    /* hardware address of client machine */
//     int8_t sname [MAX_DHCP_SNAME_LENGTH];        /* name of DHCP server */
//     int8_t file [MAX_DHCP_FILE_LENGTH];          /* boot file name (used for diskless booting?) */
// 	int8_t options [MAX_DHCP_OPTIONS_LENGTH];       /* options */
// } __attribute__((__packed__)) dhcp_packet;


//  OPT Code   Len   Option Codes
//   +-----+-----+-----+-----+---
//   |  55 |  n  |  c1 |  c2 | ...
//   +-----+-----+-----+-----+---


bool err_set_offer(scope_settings* scope, dhcp_packet* p)
{

    p->op = BOOTREPLY;
    p->hops = ZERO;
    p->secs = ZERO;
    p->ciaddr = ZERO;
    p->yiaddr = get_ip_addr(scope, scope->first_addr);
    p->siaddr = scope->dhcp_srv_addr;
    memset(&p->sname, 0, MAX_DHCP_SNAME_LENGTH);

    //memcpy(&p->options[0], cookie, 32);


    // This option is used to convey the type of the DHCP message.  The code
    //    for this option is 53, and its length is 1.  Legal values for this
    //    option are:
    //
    //            Value   Message Type
    //            -----   ------------
    //              1     DHCPDISCOVER
    //              2     DHCPOFFER
    //              3     DHCPREQUEST
    //              4     DHCPDECLINE
    //              5     DHCPACK
    //              6     DHCPNAK
    //              7     DHCPRELEASE
    //
    //     Code   Len  Type
    //    +-----+-----+-----+
    //    |  53 |  1  | 1-7 |
    //    +-----+-----+-----+
    // message type(53) offer (2)
    p->options[4]=53;
    p->options[5]=1;
    p->options[6]=2;
    // mask(1) size(4*8)
    p->options[7]=1;
    p->options[8]=4;
    memcpy(&p->options[9], &scope->mask, 32);
    p->options[13]=51;

    p->options[14]=4;
    p->options[15]=0;
    p->options[16]=0;
    p->options[17]=0;
    p->options[18]=120;
    p->options[19]=54;
    p->options[20]=4;
    memcpy(&p->options[21], &scope->dhcp_srv_addr, 32);



    int last=25;
    p->options[last]=255;




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
    //bzero((char *) &server_addr, sizeof(server_addr));
    memset(&server_addr, 0, sizeof(server_addr));
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

u_int32_t get_ip_addr(scope_settings* scope, u_int32_t ip)
{// returns first free address from scope
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
