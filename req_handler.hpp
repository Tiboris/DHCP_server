#ifndef REQ_HANDLER_HPP
#define REQ_HANDLER_HPP

#define BOOTREPLY 2
#define ZERO 0

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "structures.hpp"

using namespace std;

u_int32_t get_ip_addr(scope_settings* scope, u_int32_t ip);

bool item_in_list(u_int32_t item, vector<u_int32_t> list);

bool err_set_offer(scope_settings* scope, dhcp_packet* p);

int create_socket();

bool handle_request(scope_settings* scope, int* s, int* cs);

dhcp_packet save_request(scope_settings* scope, u_int8_t* packet);

#endif
