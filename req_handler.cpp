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
        int message_type = get_message_type(p.options);

        if (message_type == MAX_DHCP_OPTIONS_LENGTH)
        {
            continue;
        }
        else if (message_type == DHCPDISCOVER)
        {// reply with DHCPOFFER
            offered_ip = get_ip_addr(scope, scope->first_addr); //take first available address from scope
            if (offered_ip == UINT32_MAX)
            {
                continue;
            }
            rec.host_ip = offered_ip;
            memcpy(&rec.chaddr, &p.chaddr, MAX_DHCP_CHADDR_LENGTH);
            rec.reserv_start = time(nullptr);
            rec.reserv_end = rec.reserv_start + HOUR;

            cout << ctime(&rec.reserv_start) << rec.reserv_start << " seconds since the Epoch\n";
            cout << ctime(&rec.reserv_end) << rec.reserv_end << " +3600 seconds from the Epoch\n";
            for (size_t i = 0; i < 6; i++)
            {
                char c='\0';
                (i == 6-1) ? c='\n' : c=':';
                cout << setw(2) << setfill ('0') << hex << +rec.chaddr[i] << c << dec;
            }
            set_resp(scope, &p, offered_ip, DHCPOFFER);
            struct sockaddr_in br_addr;
            br_addr.sin_family = AF_INET;                       // set IPv4 addressing
            br_addr.sin_addr.s_addr = scope->broadcast;//UINT32_MAX;               // broadcast address not working
            br_addr.sin_port = htons(CLI_PORT);                   // the client listens on this port
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
        }
        else if (message_type == DHCPREQUEST && offered_ip != UINT32_MAX)
        {
            set_resp(scope, &p, offered_ip, DHCPACK);
            for (size_t i = 0; i < 6; i++)
            {
                char c='\0';
                (i == 6-1) ? c='\n' : c=':';
                cout << setw(2) << setfill ('0') << hex << +p.chaddr[i] << c << dec;
            }
            //return EXIT_SUCCESS;
            struct sockaddr_in br_addr;
            br_addr.sin_family = AF_INET;                       // set IPv4 addressing
            br_addr.sin_addr.s_addr = scope->broadcast;//UINT32_MAX;               // broadcast address not working
            br_addr.sin_port = htons(CLI_PORT);                   // the client listens on this port
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
            records.insert(records.end(), rec);
        }
        else if (message_type == DHCPRELEASE && offered_ip != UINT32_MAX)
        {
            cout << "to co to zabijem sa\n";
            return EXIT_SUCCESS;
        }
        cout << records.size()<< " pocet zaznamov\n";
    }
    return EXIT_SUCCESS;
}

int get_message_type(uint8_t* options)
{// jump = 2 size of magic coockie 4Bytes
    int cookie[COOKIE_SIZE] = {99, 130, 83, 99};
    for (size_t i = 0; i < COOKIE_SIZE; i++)
    {
        if (options[i]!=cookie[i])
        {
            return MAX_DHCP_OPTIONS_LENGTH;
        }
    }
    int pos = COOKIE_SIZE;
    while( pos < MAX_DHCP_OPTIONS_LENGTH - 2)
    {
        printf ("|%u|%u|%u|\n",options[pos],options[pos+1], options[pos+2]);
        if (options[pos] == MSG && options[pos+1] == 1)
        {
            return options[pos+2];
        }
        else if (options[pos] == 0)
        {
            pos++;
        }
        else
        {
            pos = pos + options[pos+1] + 2;
        }
    }
    return MAX_DHCP_OPTIONS_LENGTH;
}

void set_resp(scope_settings* scope, dhcp_packet* p, u_int32_t offr_ip, int t)
{

    response r;
    p->op = BOOTREPLY;
    p->hops = ZERO;
    p->secs = ZERO;
    p->ciaddr = ZERO;
    p->yiaddr = offr_ip;
    p->siaddr = scope->srv_addr;
    memset(&p->sname, ZERO, MAX_DHCP_SNAME_LENGTH);
    size_t pos = ZERO;

    r.msg_type = t;


    memcpy(&p->options[pos], &r.magic_cookie, sizeof(r.magic_cookie));
    pos += sizeof(r.magic_cookie);


    memcpy(&p->options[pos], &r.msg_type_opt, sizeof(r.msg_type_opt));
    pos += sizeof(r.msg_type_opt);


    memcpy(&p->options[pos], &r.msg_type, sizeof(r.msg_type));
    pos += sizeof(r.msg_type);


    memcpy(&p->options[pos], &r.mask_type, sizeof(r.mask_type));
    pos += sizeof(r.mask_type);


    memcpy(&p->options[pos], &scope->mask, sizeof(scope->mask));
    pos += sizeof(&scope->mask);


    memcpy(&p->options[pos], &r.lease_time_opt, sizeof(r.lease_time_opt));
    pos += sizeof(r.lease_time_opt);

    r.lease_time = htonl(r.lease_time);
    memcpy(&p->options[pos], &r.lease_time, sizeof(r.lease_time));
    pos += sizeof(r.lease_time);


    memcpy(&p->options[pos], &r.srv_identif, sizeof(r.srv_identif));
    pos += sizeof(r.srv_identif);


    memcpy(&p->options[pos], &scope->srv_addr, sizeof(scope->srv_addr));
    pos += sizeof(&scope->srv_addr);


    p->options[pos]=255;
    cout<<"packet:\t";
    for (size_t i = 0; i <= pos; i++)
    {
        printf("%u|", p->options[i] );
    }
    cout<<endl;
    return;
}

int create_socket()
{// copied from my IPK project2 and edited
    int sockfd;
    struct sockaddr_in server_addr;
    // First call socket() function
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        return -1;
    }
    // Initialize socket structure
    //bzero((char *) &server_addr, sizeof(server_addr));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    // Binding socket
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    }
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

void return_ip_addr(scope_settings* scope, uint32_t ip)
{// https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
    if (item_in_list(ip, scope->exclude_list))
    {
        scope->exclude_list.erase(remove(scope->exclude_list.begin(), scope->exclude_list.end(), ip), scope->exclude_list.end());
    }
    return;
}

bool item_in_list( uint32_t item, vector<uint32_t> list)
{// returns true when item in list
    return (find(list.begin(), list.end(), item) != list.end());
}
