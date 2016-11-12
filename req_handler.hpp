#ifndef REQ_HANDLER_HPP
#define REQ_HANDLER_HPP

#include <iostream>
#include <cstdio>
#include <ctime>
#include <regex>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <cstdint>
#include <string.h>
#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "structures.hpp"

#define ZERO                0

#define BOOTREPLY           2
#define COOKIE_SIZE         4
#define IP_SIZE             4
#define MAC_SIZE            6
//DHCP Message Types:
#define DHCPDISCOVER        1
#define DHCPOFFER           2
#define DHCPREQUEST         3
#define DHCPACK             5
#define DHCPNAK             6
#define DHCPRELEASE         7

#define REQIP               50
#define MSG                 53
#define MIN_DHCP_PCK_LEN    300             // minimal length of DHCP packet
#define MASK_T              1025            // 1 4
#define MSG_T               309             // 53 1
#define LEASE_T             1075            // 51 4
#define SRV_ID              1078            // 54 4
#define COOKIE              1666417251      // MAGIC COOKIE 99 130 83 99

#define HOUR                3600            // hour in seconds

typedef struct response_struct
{
    uint32_t magic_cookie   = COOKIE;
    uint16_t msg_type_opt   = MSG_T;
    uint8_t  msg_type       = DHCPOFFER;
    uint16_t lease_time_opt = LEASE_T;
    uint32_t lease_time     = 60;
    uint16_t mask_type      = MASK_T;
    uint16_t srv_identif    = SRV_ID;
} response;

using namespace std;

int create_socket();

void printrecord(record out);

//bool err_file(scope_settings* scope);

bool handle_request(scope_settings* scope, int* s);

void delete_record(record item, vector<record> &list);

bool item_in_list(uint32_t item, vector<uint32_t> list);

uint32_t get_ip_addr(scope_settings* scope, uint32_t ip);

void return_ip_to_scope(uint32_t ip, scope_settings* scope);

bool from_scope(uint32_t desired_ip, scope_settings* scope);

size_t record_position(record item, vector<record> list, int by);

dhcp_packet save_request(scope_settings* scope, uint8_t* packet);

void delete_expired(vector<record> &records, scope_settings* scope);

uint32_t get_info(uint8_t* options, uint8_t info_len, uint32_t info_type);

void set_resp(scope_settings* scope, dhcp_packet* p, uint32_t offr_ip, int t);

#endif
