#ifndef REQ_HANDLER_HPP
#define REQ_HANDLER_HPP

#define BOOTREPLY 2
//DHCP Message Types:
#define DHCPOFFER 2
#define DHCPACK 5
#define DHCPNAK 6

#define MASK_T  1025        // 1 4
#define MSG_T   309         // 53 1
#define LEASE_T 1075        // 51 4
#define SRV_I   1078        // 54 4
#define COOKIE  1666417251  // MAGIC COOKIE 99 130 83 99

#define HOUR 2160
#define MINUTE 15360

#define ZERO 0

typedef struct response_struct
{
    u_int32_t magic_cookie = COOKIE;
    u_int16_t msg_type_opt = MSG_T;
    u_int8_t  msg_type = DHCPOFFER;
    u_int16_t lease_time_opt = LEASE_T;
    u_int32_t lease_time = HOUR;
    u_int16_t mask_type = MASK_T;
    u_int16_t srv_identif = SRV_I;
}response;

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

void set_resp(scope_settings* scope, dhcp_packet* p, u_int32_t offr_ip, int t);

int create_socket();

bool handle_request(scope_settings* scope, int* s, int* cs);

dhcp_packet save_request(scope_settings* scope, u_int8_t* packet);

#endif
