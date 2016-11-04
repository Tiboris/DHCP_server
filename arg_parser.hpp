#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <cstdio>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "structures.hpp"

#define MAX_OCTET_NUM 255
#define USAGE "\tusage: ./dserver -p <network_address/CIRD> [-e <ip_address_list>]\n"
#define ERR_NO_ARGS "./dserver: at least option -- 'p' is needed\n"
#define ERR_MULTIPLE_OPT "./dserver: not allowed multiple usage of option -- "
#define ERR_CHAR "Unknown option character has been given"
#define ERR_ARG_1 "./dserver: option's -- '"
#define ERR_ARG_2 "' argument in wrong format\n"
#define ERR_OPT "./dserver: invalid option(s) "
#define ERR_IP_FORMAT "Wrong IP address format: "
#define ERR_NET "' is NOT valid network address.\n"

using namespace std;

// check whether options given to dserver are valid or not
bool opt_err(int argc, char** argv, scope_settings* args);
// checks option arguments validity
bool arg_err(char option, string optarg_val, scope_settings* args);
// converts string to unsigned int
uint32_t mystrtoui(string optarg_val);
// converts char* to unsigned int format address used in ip_addr struct
uint32_t strtoip(const char* ip_in);
// cuts char* string
void cut(char* src, size_t from, size_t to, char* dst);

#endif
