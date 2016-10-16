#include <cstdlib>
#include <csignal>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
/*
* namespace
*/
using namespace std;
/*
* Prototypes of functions
*/
struct settings
{
    string network;
    unsigned int cmask;
    bool exclude = false;
    vector<string> exclude_list;
} ;

const unsigned int MAX_OCTET_CNT = 4;
const unsigned int MAX_OCTET_NUM = 255;
const string USAGE = "\tusage: ./dserver -p <network_address/CIRD_mask> [-e <ip_address_list>]\n";
const string ERR_NO_ARGS = "./dserver: at least option -- 'p' is needed\n";
const string ERR_MULTIPLE_OPT = "./dserver: not allowed multiple usage of option -- ";
const string ERR_CHAR = "Unknown option character `\\x%x'\n";
const string ERR_ARG_1 = "./dserver: option's -- '";
const string ERR_ARG_2 = "' argument in wrong format\n";
const string ERR_OPT = "./dserver: invalid option(s) " ;
const string ERR_IP_FORMAT = "Wrong IP address format: " ;

void sig_handler(int signal);
bool opt_err(int argc, char** argv, settings* args);
bool arg_err(char option, string optarg_val, settings* args);
unsigned int mystrtoui(string optarg_val);
bool wrong_ipaddr_format(string ip);
/*
*   Main
*/
int main(int argc, char** argv)
{
    settings args;
    if (opt_err(argc, argv, &args))
    {
        return EXIT_FAILURE;
    }
    cout<<"Network: \t"<<args.network<<"\nCIRD Mask:\t"<< args.cmask<<endl;
    if (args.exclude) {
        cout<< "IP EXCLUDE LIST:";

        for (auto it=args.exclude_list.begin(); it<args.exclude_list.end(); it++)
            cout  <<*it << "\n\t\t";
        cout<<endl;
    }
    signal(SIGINT, sig_handler);
    while (true)
    {
        /* code */
    }
    return EXIT_SUCCESS;
}
void sig_handler(int signal)
{
    cout << "Interrupt signal (" << signal << ") received.\n";
    // cleanup and close up stuff here
    // terminate program
    exit(signal);
}


/*
*   For checking argument
*/
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
    //bool sflag = false;

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
                if ( arg_err(c, static_cast<string>(optarg), args ) )
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
                if ( arg_err(c, static_cast<string>(optarg), args ) )
                {
                    cerr << ERR_ARG_1 << (char)c << ERR_ARG_2 << USAGE;
                    return EXIT_FAILURE;
                }
                eflag++;
                max_argc_val += 2;
                break;

            case '?':
                if ( ! isprint (optopt))
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

bool arg_err(char option, string optarg_val, settings* args)
{
    size_t pos = 0;
    if (option == 'p')
    {
        string delimiter = "/";
        if ((pos = optarg_val.find(delimiter)) == string::npos){
            return EXIT_FAILURE;
        }
        args->network = optarg_val.substr(0, pos);
        optarg_val.erase(0, pos + delimiter.length());
        args->cmask = mystrtoui(optarg_val);
        // TODO je to potrebne az na 32 ?
        if (  (args->cmask > 32))
        {
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
            cout<< "HERE";
            cerr << ERR_IP_FORMAT << optarg_val << endl;
            return EXIT_FAILURE;
        }
        // Musi byt este jeden pretoze potom poslednu polozku z listu neulozi
        args->exclude_list.insert(args->exclude_list.end(), optarg_val);
    }

    return EXIT_SUCCESS;
}

unsigned int mystrtoui(string optarg_val)
{
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
