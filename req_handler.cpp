#include "req_handler.hpp"
/*
 *  request handler for dserver.cpp as school project to ISA class
 */
using namespace std;

// 00:0b:82:01:fc:42 192.168.0.99
// c8:0a:a9:cd:7d:81 192.168.0.101

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
    record new_record;
    response info;
    uint32_t offered_ip = UINT32_MAX;
    if (scope->static_reserv && err_parse_file(records, scope))
        return EXIT_FAILURE;
    while (true)
    {
        size_t id;
        dhcp_packet p;
        delete_expired(records, scope);
        int r = recvfrom(*s, &p, sizeof(p), 0, (struct sockaddr*)&c_addr, &c_len);
        if (r < MIN_DHCP_PCK_LEN)
        {
            cerr << "ERR on recvfrom\n";
            continue;
        }
        uint32_t message_type = get_info(p.options, 1, MSG);
        if (message_type == UINT32_MAX)
            continue;
        else if (message_type == DHCPDISCOVER || message_type == DHCPREQUEST)
        {// reply with DHCPOFFER
            uint32_t resp_type = DHCPACK;
            memcpy(&new_record.chaddr, &p.chaddr, MAX_DHCP_CHADDR_LENGTH);
            if (message_type == DHCPDISCOVER)
            {
                resp_type = DHCPOFFER;
                id = record_position(new_record, records, MAC_SIZE);
                if (id != records.size()) // there is record for mac use this ip
                {// use already bound ip and delete records for mac from vector
                    new_record.host_ip = records[id].host_ip;
                    delete_record(new_record, records);
                }
                else
                {// take ip from scope and offer it to host
                    new_record.host_ip = get_ip_addr(scope, scope->first_addr);
                    if (! from_scope(new_record.host_ip, scope))
                        continue; // when out of addresses then ignore
                }
                offered_ip = new_record.host_ip;
            }
            else // DHCPREQUEST
            {
                new_record.host_ip = p.ciaddr;
                id = record_position(new_record, records, MAC_SIZE);
                if (id != records.size()) // there is record for this mac
                {// use already bound ip and delete records for mac from vector
                    new_record.host_ip = records[id].host_ip;
                    new_record.permanent = records[id].permanent;
                    delete_record(new_record, records);
                }
                else if (p.ciaddr == 0)
                {
                    new_record.host_ip = get_info(p.options, 4, REQIP); // parse desired ip from options
                    id = record_position(new_record, records, IP_SIZE);
                    if (id != records.size()  && records[id].host_ip != new_record.host_ip && p.siaddr == scope->srv_addr)
                        resp_type = DHCPNAK;
                }
                else if ((id = record_position(new_record, records, IP_SIZE)) != records.size())
                {
                    if (records[id].chaddr != new_record.chaddr)
                        resp_type = DHCPNAK;
                }
                else if (item_in_list(new_record.host_ip, scope->exclude_list))
                {
                    resp_type = DHCPNAK;
                }
                if (! from_scope(new_record.host_ip, scope) || p.giaddr != 0)
                    resp_type = DHCPNAK;
                if (! item_in_list(new_record.host_ip, scope->exclude_list) && resp_type != DHCPNAK)
                    scope->exclude_list.insert(scope->exclude_list.end(), new_record.host_ip);
                new_record.reserv_start = time(nullptr);
                if (new_record.permanent)
                    new_record.reserv_end = UINT32_MAX;
                else
                    new_record.reserv_end = new_record.reserv_start + info.lease_time;
                offered_ip = new_record.host_ip;
            }
            struct sockaddr_in br_addr;
            br_addr.sin_family = AF_INET;                       // set IPv4 addressing
            br_addr.sin_addr.s_addr = scope->broadcast;         // broadcast address not working
            br_addr.sin_port = htons(CLI_PORT);                 // the client listens on this port
            int on = 1;
            if (p.ciaddr == offered_ip && resp_type != DHCPNAK && p.ciaddr != 0)
            {
                br_addr.sin_addr.s_addr = p.ciaddr;
            }
            else if ((setsockopt(*s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on))) == -1)
            {
                return_ip_to_scope(offered_ip, scope);
                continue;
            }
            set_resp(scope, &p, offered_ip, resp_type);
            r = sendto(*s, &p, sizeof(p), 0, (struct sockaddr*)&br_addr, sizeof(br_addr));
            if (r < 0)
            {
                cerr << "ERR on sendto\n";
                return_ip_to_scope(offered_ip, scope);
                continue;
            }
            if (message_type != DHCPDISCOVER)
            {
                offered_ip = UINT32_MAX;
                printrecord(new_record);
                delete_record(new_record, records);
                records.insert(records.end(), new_record);
            }
        }
        else if (message_type == DHCPRELEASE && ! new_record.permanent)
        {
            memcpy(&new_record.chaddr, &p.chaddr, MAX_DHCP_CHADDR_LENGTH);
            id = record_position(new_record, records, MAC_SIZE);
            return_ip_to_scope(records[id].host_ip, scope);
            delete_record(new_record, records);
        }
        cout << "----------*****---------\n"; // debug print
        for (auto i : scope->exclude_list)
        {
           cout << inet_ntoa(*(struct in_addr*)&i) << " is excluded\n";
        }
        cout << "------------------------\n";
        for (auto item : records)
        {
            for (size_t i = 0; i < MAC_SIZE; i++)
            {
                char c='\0';
                (i == MAC_SIZE - 1) ? c=' ' : c=':';
                cout << setw(2) << setfill ('0') << hex << +item.chaddr[i] << c << dec;
            }
            cout << inet_ntoa(*(struct in_addr*)&item.host_ip) << "\t in record" << endl;
        }
        cout << "----------|||||---------\n";
    }
    return EXIT_SUCCESS;
}

bool err_parse_file(vector<record> &records, scope_settings* scope)
{
    ifstream perm_reserv (scope->filename);
    if (perm_reserv.is_open())
    {
        size_t pos;
        record perm;
        perm.permanent = true;
        uint8_t chaddr[MAX_DHCP_CHADDR_LENGTH];
        uint32_t ip_addr;
        string line;
        string token;
        string delimiter = " ";
        while (getline(perm_reserv,line))
        {
            cout<< line<<endl;
            if ((pos = line.find(delimiter)) == string::npos)
            {
                cerr << ERR_FILE_IN;
                return EXIT_FAILURE;
            }
            token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            if (check_mac(token, chaddr))
            {
                cerr << ERR_FILE_IN;
                return EXIT_FAILURE;
            }
            pos = token.length();
            token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            ip_addr = inet_addr(token.c_str());
            if (! from_scope(ip_addr, scope))
            {
                cerr << ERR_FILE_IN;
                return EXIT_FAILURE;
            }
            std::cout << token << std::endl;

            if (line != "")
            {
                cerr << ERR_FILE_IN;
                return EXIT_FAILURE;
            }
        }
        perm_reserv.close();
    }
    else
    {
        cerr << "Error opening file for static bindings: " << scope->filename << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

bool check_mac(string in, uint8_t * chaddr)
{
    size_t cut = 0, pos = 0,i = 0;
    string token;
    string delimiter = ":";
    memset(chaddr, 0, MAX_DHCP_CHADDR_LENGTH);
    while ((cut = in.find(delimiter)) != string::npos)
    {
        token = in.substr(0, cut);
        in.erase(0, cut + delimiter.length());
        if (token.length() != 2)
            return EXIT_FAILURE;
        chaddr[pos++] = stoi(token, 0, 16);
        i++;
    }
    chaddr[pos++] = std::stoi(in, 0, 16);
    return (i > MAC_SIZE - 1 || in.length() != 2) ? EXIT_FAILURE :EXIT_SUCCESS;
}

void delete_expired(vector<record> &records, scope_settings* scope)
{// funtion deletes expired bindings from records
    time_t time_now = time(nullptr);
    for (auto item : records)
    {
        if (item.reserv_end < time_now)
        {
            delete_record(item, records);
            return_ip_to_scope(item.host_ip, scope);
        }
    }
}

void printrecord(record out)
{// funtion for creating binding output to console in format:
    //  MAC IP startdate_starttime enddate_endtime
    //  c8:0a:a9:cd:7d:81 192.168.0.101 2016-09-29_13:45 2016-09-29_15:45
    for (size_t i = 0; i < MAC_SIZE; i++)
    {
        char c='\0';
        (i == MAC_SIZE - 1) ? c=' ' : c=':';
        cout << setw(2) << setfill ('0') << hex << +out.chaddr[i] << c << dec;
    }
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
{// check whether ip is from given scope or not
    return (scope->srv_addr < desired_ip && desired_ip < scope->broadcast);
}

uint32_t get_info(uint8_t* options, uint8_t info_len, uint32_t info_type)
{// jump = 2 size of magic coockie 4Bytes
    int cookie[COOKIE_SIZE] = {99, 130, 83, 99};
    uint32_t result = UINT32_MAX;
    for (size_t i = 0; i < COOKIE_SIZE; i++)
    {
        if (options[i]!=cookie[i])
            return result;
    }
    int pos = COOKIE_SIZE;
    while( pos < MAX_DHCP_OPTIONS_LENGTH - info_len)
    {
        if (options[pos] == info_type && options[pos+1] == info_len)
        {
            if (info_type == MSG)
                result = options[pos + 2];
            else
                memcpy (&result, &options[pos + 2] , info_len);
            return result;
        }
        else if (options[pos] == 0)
            pos++;
        else
            pos = pos + options[pos+1] + 2;
    }
    return result;
}

void set_resp(scope_settings* scope, dhcp_packet* p, u_int32_t offr_ip, int t)
{// funtion for creating packet structure before sending to client
    response r;
    p->op = BOOTREPLY;
    p->hops = ZERO;
    p->secs = ZERO;
    p->ciaddr = ZERO;
    if (r.msg_type != DHCPNAK)
        p->yiaddr = offr_ip;
    else  // NAK message does not contain yiaddr record
        p->yiaddr = ZERO;
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
    if (r.msg_type != DHCPNAK)
    {
        memcpy(&p->options[pos], &r.mask_type, sizeof(r.mask_type));
        pos += sizeof(r.mask_type);
        memcpy(&p->options[pos], &scope->mask, sizeof(scope->mask));
        pos += sizeof(&scope->mask);
        // ACK message does not contain lease time option too
        memcpy(&p->options[pos], &r.lease_time_opt, sizeof(r.lease_time_opt));
        pos += sizeof(r.lease_time_opt);
        r.lease_time = htonl(r.lease_time); // change endian
        memcpy(&p->options[pos], &r.lease_time, sizeof(r.lease_time));
        pos += sizeof(r.lease_time);
    }
    memcpy(&p->options[pos], &r.srv_identif, sizeof(r.srv_identif));
    pos += sizeof(r.srv_identif);
    memcpy(&p->options[pos], &scope->srv_addr, sizeof(scope->srv_addr));
    pos += sizeof(&scope->srv_addr);
    p->options[pos]=255;
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

size_t record_position(record item, vector<record> list, int by)
{// return record position if fits MAC or IP address of record
    size_t index = 0;
    for ( auto i = list.begin(); i < list.end(); i++, index++)
    {
        if (by == MAC_SIZE)
        {
            if (memcmp(item.chaddr, i->chaddr, MAX_DHCP_CHADDR_LENGTH) == 0)
                break;
        }
        else if (by == IP_SIZE)
        {
            if (item.host_ip == i->host_ip)
                break;
        }

    }
    return index;
}

void delete_record(record item, vector<record> &list)
{// function to remove bninding records to mac address
    size_t pos;
    while ( (pos = record_position(item, list, MAC_SIZE)) != list.size())
    {
        list.erase(list.begin() + pos);
    }
    return;
}

void return_ip_to_scope(uint32_t ip, scope_settings* scope)
{// https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
    if (item_in_list(ip, scope->exclude_list))
        scope->exclude_list.erase(remove(scope->exclude_list.begin(), scope->exclude_list.end(), ip), scope->exclude_list.end());
    return;
}

bool item_in_list(uint32_t item, vector<uint32_t> list)
{// returns true when item in list
    return (find(list.begin(), list.end(), item) != list.end());
}
