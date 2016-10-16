#include "argparser.hpp"

using namespace std;

bool arg_err(char option, string optarg_val, settings* args)
{
    size_t pos = 0;
    if (option == 'p')
    {
        string delimiter = "/";
        if ((pos = optarg_val.find(delimiter)) == string::npos)
        {
            return EXIT_FAILURE;
        }
        args->network = optarg_val.substr(0, pos);
        if (wrong_ipaddr_format(args->network))
        {
            cerr << ERR_IP_FORMAT << args->network << endl;
            return EXIT_FAILURE;
        }
        optarg_val.erase(0, pos + delimiter.length());
        args->cmask = mystrtoui(optarg_val);
        if ((args->cmask == 0) || (args->cmask > 30 ))
        {
            cerr << "CIRD mask '" << optarg_val << "' NOT supported."<< endl;
            return EXIT_FAILURE;
        }
    }
    else if (option == 'e')
    {
        string delimiter = ",";
        while ((pos = optarg_val.find(delimiter)) != string::npos)
        {
            string token = optarg_val.substr(0, pos);
            if (wrong_ipaddr_format(token))
            {
                cerr << ERR_IP_FORMAT << token << endl;
                return EXIT_FAILURE;
            }
            args->exclude_list.insert(args->exclude_list.end(), token);
            optarg_val.erase(0, pos + delimiter.length());
        }
        if (wrong_ipaddr_format(optarg_val))
        {
            cerr << ERR_IP_FORMAT << optarg_val << endl;
            return EXIT_FAILURE;
        }
        // has to be one more insert otherways last ip in list will be skipped
        args->exclude_list.insert(args->exclude_list.end(), optarg_val);
    }

    return EXIT_SUCCESS;
}

bool opt_err(int argc, char** argv, settings* args)
{
    int min_opt_cnt = 2;
    if (argc < min_opt_cnt)
    {
        cerr << ERR_NO_ARGS << USAGE;
        return EXIT_FAILURE;
    }
    unsigned int pflag = 0;
    unsigned int eflag = 0;
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
                if (arg_err(c, static_cast<string>(optarg), args ))
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
                args->exclude = !args->exclude;
                if (arg_err(c, static_cast<string>(optarg), args ))
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
            fprintf(stderr, "-- '%s' ",argv[i] );
        }
        cerr << USAGE;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

unsigned int mystrtoui(string optarg_val)
{
    if (optarg_val == "")
    {
        return MAX_OCTET_NUM + 1;
    }
    for (auto i = optarg_val.begin(); i != optarg_val.end(); i++)
    {
        if (!isdigit(*i))
        {
            return MAX_OCTET_NUM + 1;
        }
    }
    return stoul(optarg_val,nullptr,10);
}

bool wrong_ipaddr_format(string ip)
{
    size_t pos = 0;
    string delimiter = ".";
    for (size_t octet = 1; octet <= MAX_OCTET_CNT; octet++)
    {
        pos = ip.find(delimiter);
        if( (pos == string::npos && octet != MAX_OCTET_CNT) ||
            ((mystrtoui(ip.substr(0, pos))) > MAX_OCTET_NUM) )
        {
            return EXIT_FAILURE;
        }
        ip.erase(0, pos + delimiter.length());
    }
    return EXIT_SUCCESS;
}
