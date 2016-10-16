#include <cstdlib>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


/*
* namespace
*/
using namespace std;
/*
* Prototypes of functions
*/

bool args_err(int argc, char** argv);

/*
*   Main
*/
int main(int argc, char** argv)
{
    if (args_err(argc,argv))
    {
        return EXIT_FAILURE;
    }

    cout<<("OK")<<endl;
    return EXIT_SUCCESS;
}
/*
*   For checking argument
*/
bool args_err(int argc, char** argv)
{
    unsigned int min_opt_cnt = 2;
    if (argc < min_opt_cnt)
    {
        fprintf (stderr, "./dserver: at least option -- 'p' is needed\n\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n");
        return EXIT_FAILURE;
    }
    unsigned int pflag = 0;
    unsigned int eflag = 0;
    unsigned int max_opt_cnt = 1;
    //bool sflag = false;
    char *cvalue = NULL;
    int c;
    unsigned int f = 0;

    while ((c = getopt (argc, argv, "p:e:")) != -1)
    switch (c)
    {
        case 'p':
            if (pflag)
            {
                fprintf (stderr, "./dserver: not allowed multiple usage of option --%c\n\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n", c);
                return EXIT_FAILURE;
            }
            cvalue = optarg;
            pflag++;
            f = f+1;
            max_opt_cnt += 2;
            break;

        case 'e':
            if (eflag)
            {
                return EXIT_FAILURE;
            }
            cvalue = optarg;
            eflag++;
            max_opt_cnt += 2;
            f = f+1;

            break;

        case '?':
            if (optopt == 'p' || optopt == 'e')
            {
                fprintf (stderr, "\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n");
            }
            else if ( ! isprint (optopt))
            {
                fprintf (stderr,"Unknown option character `\\x%x'\n\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n", optopt);
            }

        default:
            return EXIT_FAILURE;
    }
    if (argc > max_opt_cnt)
    {
        fprintf (stderr, "./dserver: unknown option(s) ");
        for (size_t i = max_opt_cnt; i < argc; i++)
        {
            fprintf(stderr, "-- '%s' ",argv[i] );
        }
        fprintf(stderr, "\n\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n", c);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
