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
#define ERR_CHAR "Unknown option character `\\x%x'\n"
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
    bool exclude = false;
    vector<u_int32_t> exclude_list;
    u_int32_t first_addr;
    u_int32_t broadcast;
}scope_settings;

bool opt_err(int argc, char** argv, scope_settings* args);

bool arg_err(char option, string optarg_val, scope_settings* args);

u_int32_t mystrtoui(string optarg_val);

u_int32_t strtoip(const char* ip_in);

void cut(char* src, size_t from, size_t to, char* dst);

#endif
