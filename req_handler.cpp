#include "req_handler.hpp"
/*
 *  request handler for dserver.cpp as school project to ISA class
 */
using namespace std;

bool handle_request(scope_settings* scope, int* s)
{
    if ((*s = create_socket()) == -1)
    {
        cerr<< "ERR creating socket\n";
        return EXIT_FAILURE;
    }
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);
    vector<record> records;
    record rec;
    uint32_t offered_ip = UINT32_MAX;
    while (true)
    {
        dhcp_packet p;
        int r = recvfrom(*s, &p, sizeof(p), 0, (struct sockaddr*)&c_addr, &c_len);
        if (r < MIN_DHCP_PCK_LEN)
        {
            cerr << "ERR on recv\n";
            continue;
        }
        int message_type = get_info(p.options, 1, MSG);
        if (message_type == MAX_DHCP_OPTIONS_LENGTH)
            continue;
        else if (message_type == DHCPDISCOVER || message_type == DHCPREQUEST)
        {// reply with DHCPOFFER
            int resp_type = DHCPACK;
            if (message_type == DHCPDISCOVER)
            {
                resp_type = DHCPOFFER;
                uint32_t desired_ip = get_info(p.options, 4, REQIP);
                if (! from_scope(desired_ip, scope))
                    offered_ip = get_ip_addr(scope, scope->first_addr);
                else
                    offered_ip = desired_ip; //take first available address from scope
                if (offered_ip == UINT32_MAX)
                    continue;
            } // REQUEST potrebujem zistit ci mam zaznam pre
            else
            {
                memcpy(&rec.chaddr, &p.chaddr, MAX_DHCP_CHADDR_LENGTH);
                size_t id = record_position(rec, records, true);
                if (id != records.size()) // there is record for this mac address
                {
                    offered_ip = rec.host_ip = records[id].host_ip;
                    delete_record(rec, records);
                }
                uint32_t desired_ip = p.ciaddr;
                cout << desired_ip << endl;
                if (desired_ip == 0)                 // SELECTING
                {
                    desired_ip = get_info(p.options, 4, REQIP);
                    if (! from_scope(desired_ip, scope))
                        continue;
                    if (item_in_list(desired_ip, scope->exclude_list))
                        desired_ip = get_ip_addr(scope, scope->first_addr);
                    if (offered_ip == UINT32_MAX)
                        continue;
                    rec.host_ip = offered_ip = desired_ip;
                }
                else if (from_scope(desired_ip, scope))              // client address is set and from scope RENEW
                {
                    offered_ip = desired_ip;
                    rec.host_ip = offered_ip;
                    cout << inet_ntoa(*(struct in_addr*)&offered_ip) <<  " <- renew \n";
                    cout <<" pr eco\n\n"<<endl;
                }
                else
                {
                    cout <<"nema sa stat"<<endl;
                    continue;
                }
                rec.reserv_start = time(nullptr);
                rec.reserv_end = rec.reserv_start + HOUR;
                cout << offered_ip << " <> " << p.ciaddr << endl;
            }
            set_resp(scope, &p, offered_ip, resp_type);
            struct sockaddr_in br_addr;
            br_addr.sin_family = AF_INET;                       // set IPv4 addressing
            br_addr.sin_addr.s_addr = scope->broadcast;         //UINT32_MAX;               // broadcast address not working
            br_addr.sin_port = htons(CLI_PORT);                 // the client listens on this port
            int on = 1;
            if ((setsockopt(*s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on))) == -1)
            {
                return_ip_addr(scope, offered_ip);
                continue;
            }
            r = sendto(*s, &p, sizeof(p), 0, (struct sockaddr*)&br_addr, sizeof(br_addr));
            if (r < 0)
            {
                cerr << "ERR on sendto\n";
                return_ip_addr(scope, offered_ip);
                continue;
            }
            if (message_type != DHCPDISCOVER)
            {
                offered_ip = UINT32_MAX;
                printrecord(rec);
                delete_record(rec, records);
                records.insert(records.end(), rec);
            }
        }
        else if (message_type == DHCPRELEASE)
        {
            cout << "to co toto zabijem sa !!!\n";
            for (size_t i = 0; i < MAX_DHCP_OPTIONS_LENGTH; i++) {
                printf("| %u |\n", p.options[i] );
            }
            memcpy(&rec.chaddr, &p.chaddr, MAX_DHCP_CHADDR_LENGTH);
            delete_record(rec, records);
        }
        for (auto item : records)
        {
            cout << "Zaznam pre : "<< inet_ntoa(*(struct in_addr*) &item.host_ip)<<endl;
        }
    }
    return EXIT_SUCCESS;
}

void printrecord(record out)
{
    for (size_t i = 0; i < MAC_SIZE; i++)
    {
        char c='\0';
        (i == MAC_SIZE - 1) ? c=' ' : c=':';
        cout << setw(2) << setfill ('0') << hex << +out.chaddr[i] << c << dec;
    }
    //c8:0a:a9:cd:7d:81 192.168.0.101 2016-09-29_13:45 2016-09-29_15:45
    cout << inet_ntoa(*(struct in_addr*) &out.host_ip);
    struct tm * timeinfo;
    timeinfo = localtime (&out.reserv_start);
    char buffer [80];
    strftime (buffer,sizeof(buffer)," %F_%H:%M",timeinfo);
    cout << buffer;
    timeinfo = localtime (&out.reserv_end);
    strftime (buffer,sizeof(buffer)," %F_%H:%M",timeinfo);
    cout << buffer << endl;
}

bool from_scope(uint32_t desired_ip, scope_settings* scope)
{
    uint32_t min = scope->srv_addr;
    uint32_t max = scope->broadcast;
    //cout << inet_ntoa(*(struct in_addr*)&desired_ip) <<  "<-input project \n";
    uint32_t tmp = desired_ip;
    // cout << inet_ntoa(*(struct in_addr*)&tmp) <<  "<-project \n";
    // cout << inet_ntoa(*(struct in_addr*)&min) <<  "<-project \n";
    // cout << inet_ntoa(*(struct in_addr*)&max) <<  "<-project \n";
    // cout << (min < tmp && tmp < max) << " zevraj \n\n\n";
    return (min < tmp && tmp < max);
}

uint32_t get_info(uint8_t* options, uint8_t info_len, uint32_t info_type)
{// jump = 2 size of magic coockie 4Bytes
    int cookie[COOKIE_SIZE] = {99, 130, 83, 99};
    for (size_t i = 0; i < COOKIE_SIZE; i++)
    {
        if (options[i]!=cookie[i])
            return MAX_DHCP_OPTIONS_LENGTH;
    }
    int pos = COOKIE_SIZE;
    uint32_t result = UINT32_MAX;
    while( pos < MAX_DHCP_OPTIONS_LENGTH - info_len)
    {
        //printf ("|%u|%u|%u|\n",options[pos],options[pos+1], options[pos+2]);
        if (options[pos] == info_type && options[pos+1] == info_len)
        {
            if (info_type == MSG)
                result = options[pos + 2];
            else
                memcpy (&result, &options[pos + 2] , info_len);
            //cout << "result is : "<< inet_ntoa(*(struct in_addr*)&result)<<endl;
            return result;
        }
        else if (options[pos] == 0)
            pos++;
        else
            pos = pos + options[pos+1] + 2;
    }
    return UINT32_MAX;
}

void set_resp(scope_settings* scope, dhcp_packet* p, u_int32_t offr_ip, int t)
{// funtion for creating packet structure before sending to client
    response r;
    p->op = BOOTREPLY;
    p->hops = ZERO;
    p->secs = ZERO;
    p->ciaddr = ZERO;
    p->yiaddr = offr_ip;
    p->siaddr = scope->srv_addr;
    memset(&p->sname, ZERO, MAX_DHCP_SNAME_LENGTH);
    size_t pos = ZERO;
    memcpy(&p->options[pos], &r.magic_cookie, sizeof(r.magic_cookie));
    pos += sizeof(r.magic_cookie);
    r.msg_type = t;
    memcpy(&p->options[pos], &r.msg_type_opt, sizeof(r.msg_type_opt));
    pos += sizeof(r.msg_type_opt);
    memcpy(&p->options[pos], &r.msg_type, sizeof(r.msg_type));
    pos += sizeof(r.msg_type);
    memcpy(&p->options[pos], &r.mask_type, sizeof(r.mask_type));
    pos += sizeof(r.mask_type);
    memcpy(&p->options[pos], &scope->mask, sizeof(scope->mask));
    pos += sizeof(&scope->mask);
    memcpy(&p->options[pos], &r.srv_identif, sizeof(r.srv_identif));
    pos += sizeof(r.srv_identif);
    memcpy(&p->options[pos], &scope->srv_addr, sizeof(scope->srv_addr));
    pos += sizeof(&scope->srv_addr);
    if (r.msg_type == DHCPNAK)
    {   // ACK message does not contain yiaddr record
        p->yiaddr = ZERO;
    }
    else
    {   // ACK message does not contain lease time option too
        memcpy(&p->options[pos], &r.lease_time_opt, sizeof(r.lease_time_opt));
        pos += sizeof(r.lease_time_opt);
        r.lease_time = htonl(r.lease_time); // change endian
        memcpy(&p->options[pos], &r.lease_time, sizeof(r.lease_time));
        pos += sizeof(r.lease_time);
    }
    p->options[pos]=255;
    //printf("packet type %u:\t", t);
    for (size_t i = 0; i <= pos; i++)
    {
        //printf("%u|", p->options[i] );
    }
    cout<<endl;
    return;
}

int create_socket()
{// copied from my IPK project2 and edited
    int sockfd;
    struct sockaddr_in addr;
    // First call socket() function
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return -1;
    // Initialize socket structure
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    // Binding socket
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        return -1;
    // Returning binded socket
    return sockfd;
}

uint32_t get_ip_addr(scope_settings* scope, uint32_t ip)
{// returns first free address from scope
    uint32_t offered_ip = ip;
    if (ip == scope->broadcast)
    {   // when free address is broadcast we are out of addresses in scope
        return UINT32_MAX;
    }
    else if (item_in_list(offered_ip, scope->exclude_list))
    {   // when address is already in use or in exclude_list we try again other
        offered_ip = htonl(offered_ip);
        offered_ip++; // next address might be usable
        offered_ip = htonl(offered_ip);
        return get_ip_addr(scope, offered_ip);
    }
    else
    {   // return first usable address and then add it to exclude_list
        scope->exclude_list.insert(scope->exclude_list.end(), offered_ip);
        return offered_ip;
    }
}

size_t record_position(record item, vector<record> list, bool mac)
{
    size_t index = 0;
    for ( auto i = list.begin(); i < list.end(); i++, index++)
    {
        if(mac)
        {
            if (memcmp(item.chaddr, i->chaddr, MAX_DHCP_CHADDR_LENGTH) == 0)
                break;
        }
        else
        {
            if (memcmp(item.chaddr, i->chaddr, MAX_DHCP_CHADDR_LENGTH) != 0)
                continue;
            if(item.host_ip != i->host_ip)
                continue;
            if(item.reserv_start != i->reserv_start)
                continue;
            if(item.reserv_end != i->reserv_end)
                continue;
            else
                break;
        }
    }
    return index;
}

void delete_record(record item, vector<record> &list)
{
    size_t pos;
    while ( (pos = record_position(item, list, false)) != list.size())
    {
        list.erase(list.begin() + pos);
    }
    return;
}

void return_ip_addr(scope_settings* scope, uint32_t ip)
{// https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
    if (item_in_list(ip, scope->exclude_list))
        scope->exclude_list.erase(remove(scope->exclude_list.begin(), scope->exclude_list.end(), ip), scope->exclude_list.end());
    return;
}

bool item_in_list(uint32_t item, vector<uint32_t> list)
{// returns true when item in list
    return (find(list.begin(), list.end(), item) != list.end());
}
