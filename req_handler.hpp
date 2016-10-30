#ifndef REQ_HANDLER_HPP
#define REQ_HANDLER_HPP

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdint>
#include <string.h>
#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "structures.hpp"

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
    uint32_t magic_cookie = COOKIE;
    uint16_t msg_type_opt = MSG_T;
    uint8_t  msg_type = DHCPOFFER;
    uint16_t lease_time_opt = LEASE_T;
    uint32_t lease_time = HOUR;
    uint16_t mask_type = MASK_T;
    uint16_t srv_identif = SRV_I;
}response;

using namespace std;

uint32_t get_ip_addr(scope_settings* scope, uint32_t ip);

bool item_in_list(uint32_t item, vector<uint32_t> list);

void set_resp(scope_settings* scope, dhcp_packet* p, uint32_t offr_ip, int t);

int create_socket();

bool handle_request(scope_settings* scope, int* s);

dhcp_packet save_request(scope_settings* scope, uint8_t* packet);

#endif
