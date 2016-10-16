#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#define MAX_OCTET_CNT 4
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

struct settings
{
    string network;
    unsigned int cmask;
    bool exclude = false;
    vector<string> exclude_list;
};

bool opt_err(int argc, char** argv, settings* args);

bool arg_err(char option, string optarg_val, settings* args);

unsigned int mystrtoui(string optarg_val);

bool wrong_ipaddr_format(string ip);

#endif
