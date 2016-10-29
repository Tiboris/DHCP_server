#include <csignal>
#include <iostream>

#include "structures.hpp"
#include "arg_parser.cpp"
#include "req_handler.cpp"

using namespace std;

int srv_socket = -1; // socket has to be closed after SIGINT;

void sig_handler(int signal);

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

    return handle_request(&scope, &srv_socket);
}

void sig_handler(int signal)
{
    cout << "\nInterrupt signal (" << signal << ") received...\n";
    if (srv_socket != -1)
    {
        cout << "Closing server socket...\n";
        close(srv_socket);
    }
    exit(EXIT_SUCCESS);
}



// // into main to print info
// struct in_addr ip_addr;
// ip_addr.s_addr = scope.network_addr ;
// printf("The NW is %s\n", inet_ntoa(ip_addr));
// ip_addr.s_addr = scope.dhcp_srv_addr ;
// printf("The IP is %s\n", inet_ntoa(ip_addr));
// ip_addr.s_addr = scope.mask ;
// printf("The MS is %s\n", inet_ntoa(ip_addr));
// if (scope.exclude_list.begin()!=scope.exclude_list.end())
// {
//     cout<< "IP EXCLUDE LIST:\n";
//     for (auto item=scope.exclude_list.begin(); item<scope.exclude_list.end(); item++)
//     {
//         ip_addr.s_addr = *item;
//         printf("\t%s\n", inet_ntoa(ip_addr));
//     }
// }
// ip_addr.s_addr = scope.first_addr ;
// printf("The FR is %s\n", inet_ntoa(ip_addr));
// ip_addr.s_addr = scope.broadcast ;
// printf("The BR is %s\n", inet_ntoa(ip_addr));
//
// ip_addr.s_addr = get_ip_addr(&scope, scope.first_addr);
// printf("Offers: %s\n", inet_ntoa(ip_addr));
//
// if (scope.exclude_list.begin()!=scope.exclude_list.end())
// {
//     cout<< "IP EXCLUDE LIST:\n";
//     for (auto item=scope.exclude_list.begin(); item<scope.exclude_list.end(); item++)
//     {
//         ip_addr.s_addr = *item;
//         printf("\t%s\n", inet_ntoa(ip_addr));
//     }
// }
// printf("\nBEGIN LISTEN:\n\n");
