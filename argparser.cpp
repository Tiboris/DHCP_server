#include "argparser.hpp"
/*
 *  Argument parser for dserver.cpp as school project to ISA class
 */
using namespace std;

bool arg_err(char option, char* optarg_val, scope_settings* scope)
{
    size_t pos = 0;
    if (option == 'p')
    {
        string delimiter = "/";
        string tmp = static_cast<string>(optarg_val);
        if ((pos =  tmp.find(delimiter)) == string::npos)
        {
            return EXIT_FAILURE;
        }
        char result[INET_ADDRSTRLEN];
        cut(optarg_val, 0, pos, result);
        if ((scope->network_addr = strtoip(result)) == UINT32_MAX)
        {
            cerr << ERR_IP_FORMAT << optarg_val << endl;
            return EXIT_FAILURE;
        }
        tmp.erase(0, pos + delimiter.length());
        int cmask = mystrtoui(tmp); // > MAX_OCTET_NUM
        if ((cmask == 0) || (cmask > 30 ))
        {
            cerr << "CIRD mask '/" << tmp << "' NOT supported."<< endl;
            return EXIT_FAILURE;
        }
        scope->mask = scope->mask >> cmask;
        scope->mask = htonl(scope->mask);
        scope->broadcast = scope->network_addr + scope->mask;
        scope->mask = ~ scope->mask;
        scope->dhcp_srv_addr = htonl(scope->network_addr);
        scope->dhcp_srv_addr++;
        scope->first_addr = htonl(scope->dhcp_srv_addr + 1);
        scope->dhcp_srv_addr = htonl(scope->dhcp_srv_addr);
    }
    else if (option == 'e')
    {
        string delimiter = ",";
        u_int32_t exclude;
        size_t start_pos = 0;
        char result[INET_ADDRSTRLEN];
        string tmp = static_cast<string>(optarg_val);
        while ((pos = tmp.find(delimiter)) != string::npos)
        {
            string token = tmp.substr(0, pos);
            tmp.erase(0, pos + delimiter.length());
            cut(optarg_val, start_pos, pos+start_pos, result);
            if ((exclude = strtoip(result)) == UINT32_MAX)
            {
                cerr << ERR_IP_FORMAT << token << endl;
                return EXIT_FAILURE;
            }
            start_pos = start_pos + pos + delimiter.length();
            scope->exclude_list.insert(scope->exclude_list.end(), exclude);
        }
        cut(optarg_val, start_pos, start_pos + tmp.length(), result);
        if ((exclude = strtoip(result)) == UINT32_MAX)
        {
            cerr << ERR_IP_FORMAT << result << endl;
            return EXIT_FAILURE;
        }
        //has to be one more insert otherways last ip in list will be skipped
        scope->exclude_list.insert(scope->exclude_list.end(), exclude);
    }

    return EXIT_SUCCESS;
}

bool opt_err(int argc, char** argv, scope_settings* scope)
{
    int min_opt_cnt = 2;
    if (argc < min_opt_cnt)
    {
        cerr << ERR_NO_ARGS << USAGE;
        return EXIT_FAILURE;
    }
    u_int32_t pflag = 0;
    u_int32_t eflag = 0;
    int max_argc_val = 1;
    int c;
    while ((c = getopt (argc, argv, "p:e:")) != -1)
    {
        switch (c)
        {
            case 'p':
                if (pflag)
                {
                    cerr << ERR_MULTIPLE_OPT << (char)c << endl << USAGE;
                    return EXIT_FAILURE;
                }
                if (arg_err(c, optarg, scope ))
                {
                    cerr << ERR_ARG_1 << (char)c << ERR_ARG_2 << USAGE;
                    return EXIT_FAILURE;
                }
                pflag++;
                max_argc_val += 2;
                break;

            case 'e':
                if (eflag)
                {
                    cerr << ERR_MULTIPLE_OPT << (char)c << endl << USAGE;
                    return EXIT_FAILURE;
                }
                scope->exclude = !scope->exclude;
                if (arg_err(c, optarg, scope ))
                {
                    cerr << ERR_ARG_1 << (char)c << ERR_ARG_2 << USAGE;
                    return EXIT_FAILURE;
                }
                eflag++;
                max_argc_val += 2;
                break;

            case '?':
                if (! isprint (optopt))
                {
                    cerr << ERR_CHAR << USAGE;
                }
                else
                {
                    cerr << USAGE;
                }

            default:
                return EXIT_FAILURE;
        }
    }
    if (argc > max_argc_val)
    {
        cerr<<ERR_OPT;
        for (int i = max_argc_val; i < argc; i++)
        {
            cerr<< "-- '"<< argv[i] <<"' ";
        }
        cerr << USAGE;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

u_int32_t mystrtoui(string optarg_val)
{
    if (optarg_val == "")
    {
        return MAX_OCTET_NUM;
    }
    for (auto i = optarg_val.begin(); i != optarg_val.end(); i++)
    {
        if (!isdigit(*i))
        {
            return MAX_OCTET_NUM;
        }
    }
    return stoul(optarg_val,nullptr,10);
}

u_int32_t strtoip(const char* ip_in)
{
    struct in_addr ip_addr;
    return (inet_aton(ip_in, &ip_addr) == 0) ? UINT32_MAX : ip_addr.s_addr;
}

void cut(char* src, size_t from, size_t to, char* dst)
{
    size_t i;
    for (i = from; i < to ; i++)
    {
        dst[i-from]=src[i];
    }
    dst[i]='\0';
    return;
}
