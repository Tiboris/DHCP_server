#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>

#define MAX_OCTET_NUM 255
#define USAGE "\tusage: ./dserver -p <network_address/CIRD> [-e <ip_address_list>]\n"
#define ERR_NO_ARGS "./dserver: at least option -- 'p' is needed\n"
#define ERR_MULTIPLE_OPT "./dserver: not allowed multiple usage of option -- "
#define ERR_CHAR "Unknown option character has been given"
#define ERR_ARG_1 "./dserver: option's -- '"
#define ERR_ARG_2 "' argument in wrong format\n"
#define ERR_OPT "./dserver: invalid option(s) "
#define ERR_IP_FORMAT "Wrong IP address format: "

using namespace std;

typedef struct scope_settings_struct
{
    u_int32_t network_addr;
    u_int32_t dhcp_srv_addr;
    u_int32_t mask = UINT32_MAX;
    vector<u_int32_t> exclude_list;
    u_int32_t first_addr;
    u_int32_t broadcast;
}scope_settings;
// check whether options given to dserver are valid or not
bool opt_err(int argc, char** argv, scope_settings* args);
// checks option arguments validity
bool arg_err(char option, string optarg_val, scope_settings* args);
// converts string to unsigned int
u_int32_t mystrtoui(string optarg_val);
// converts char* to unsigned int format address used in ip_addr struct
u_int32_t strtoip(const char* ip_in);
// cuts char* string
void cut(char* src, size_t from, size_t to, char* dst);

#endif
