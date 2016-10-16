
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

#include "argparser.cpp"

#define PORT 1167
#define MAX_OCTET_CNT 4
#define MAX_OCTET_NUM 255

using namespace std;
/*
* Prototypes of functions
*/


// typedef struct dhcp_packet_struct{
//     u_int8_t  op;                   /* packet type */
//     u_int8_t  htype;                /* type of hardware address for this machine (Ethernet, etc) */
//     u_int8_t  hlen;                 /* length of hardware address (of this machine) */
//     u_int8_t  hops;                 /* hops */
//     u_int32_t xid;                  /* random transaction id number - chosen by this machine */
//     u_int16_t secs;                 /* seconds used in timing */
//     u_int16_t flags;                /* flags */
//     struct in_addr ciaddr;          /* IP address of this machine (if we already have one) */
//     struct in_addr yiaddr;          /* IP address of this machine (offered by the DHCP server) */
//     struct in_addr siaddr;          /* IP address of DHCP server */
//     struct in_addr giaddr;          /* IP address of DHCP relay */
//     unsigned char chaddr [MAX_DHCP_CHADDR_LENGTH];      /* hardware address of this machine */
//     char sname [MAX_DHCP_SNAME_LENGTH];    /* name of DHCP server */
//     char file [MAX_DHCP_FILE_LENGTH];      /* boot file name (used for diskless booting?) */
// 	char options[MAX_DHCP_OPTIONS_LENGTH];  /* options */
// }dhcp_packet;
//
//
// typedef struct dhcp_offer_struct{
// 	struct in_addr server_address;   /* address of DHCP server that sent this offer */
// 	struct in_addr offered_address;  /* the IP address that was offered to us */
// 	u_int32_t lease_time;            /* lease time in seconds */
// 	u_int32_t renewal_time;          /* renewal time in seconds */
// 	u_int32_t rebinding_time;        /* rebinding time in seconds */
// 	struct dhcp_offer_struct *next;
// }dhcp_offer;
//
//
//

void sig_handler(int signal);

/*
*   Main
*/
#include <err.h>
#include <arpa/inet.h>
#define BUFFER	(1024)   // length of the receiving buffer


int main(int argc, char** argv)
{
    settings args;
    if (opt_err(argc, argv, &args))
    {
        return EXIT_FAILURE;
    }
    cout<<"Network: \t"<<args.network<<"\nCIRD Mask:\t"<< args.cmask<<endl;
    if (args.exclude)
    {
        cout<< "IP EXCLUDE LIST:";
        for (auto it=args.exclude_list.begin(); it<args.exclude_list.end(); it++)
        {
            cout  <<*it << "\n\t\t";
        }
    }
    cout<<endl;
    signal(SIGINT, sig_handler);

    int fd;                           // an incoming socket descriptor
    struct sockaddr_in server;        // server's address structure
    int n, r;
    char buffer[BUFFER];              // receiving buffer
    struct sockaddr_in client;        // client's address structure
    socklen_t length;

    server.sin_family = AF_INET;                     // set IPv4 addressing
    server.sin_addr.s_addr = htonl(INADDR_ANY);      // the server listens to any interface
    server.sin_port = htons(PORT);                   // the server listens on this port

    printf("opening UDP socket(...)\n");
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) // create the server UDP socket
        err(1, "socket() failed");

    printf("binding to the port %d (%d)\n",PORT,server.sin_port);
    if (bind(fd, (struct sockaddr *)&server, sizeof(server)) == -1) // binding with the port
        err(1, "bind() failed");
    length = sizeof(client);



    while ((n= recvfrom(fd, buffer, BUFFER, 0, (struct sockaddr *)&client, &length)) >= 0)
    {
        printf("Request received from %s, port %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        for (r = 0; r < n; r++)
            if (islower(buffer[r]))
                buffer[r] = toupper(buffer[r]);
            else if (isupper(buffer[r]))
                buffer[r] = tolower(buffer[r]);
            r = sendto(fd, buffer, n, 0, (struct sockaddr *)&client, length); // send the answer
            if (r == -1)
                err(1, "sendto()");
            else if (r != n)
                errx(1, "sendto(): Buffer written just partially");
            else
                printf("data \"%.*s\" sent from port %d to %s, port %d\n",r-1,buffer,ntohs(server.sin_port), inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    }
    printf("closing socket\n");
    close(fd);
    return EXIT_SUCCESS;
}
void sig_handler(int signal)
{
    cout << "Interrupt signal (" << signal << ") received.\n";
    // cleanup and close up stuff here
    // terminate program
    exit(signal);
}
