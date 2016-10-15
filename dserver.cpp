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
    if (argc == 1)
    {
        return EXIT_FAILURE;
    }
    unsigned int pflag = 0;
    unsigned int eflag = 0;
    //bool sflag = false;
    char *cvalue = NULL;
    int c;

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
            pflag = !pflag;
            break;

        case 'e':
            if (eflag)
            {
                return EXIT_FAILURE;
            }
            cvalue = optarg;
            eflag = !eflag;
            break;

        case '?':
            if (optopt == 'p' || optopt == 'e')
            {
                if (optopt == 'p' && pflag )
                {
                    fprintf (stderr, "\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n");
                }
                else if (optopt == 'e' && eflag )
                {
                    fprintf (stderr, "\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n");
                }
                else
                {
                    fprintf (stderr, "\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n");
                }
            }
            else if (isprint (optopt))
            {
                fprintf (stderr, "\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n");
            }
            else
                fprintf (stderr,"Unknown option character `\\x%x'\n\tusage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]\n", optopt);
            return EXIT_FAILURE;

        default:
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
