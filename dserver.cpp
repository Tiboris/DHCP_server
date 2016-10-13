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
/*
* namespace
*/
using namespace std;
/*
* Prototypes of functions
*/

bool args_err(int argc, const char** argv);

/*
*   Main
*/
int main(int argc, char const *argv[])
{
    if (args_err(argc,argv))
    {
        cerr<<("Wrong parameters, usage: ./dserver -p <ip_address/mask> [-e <ip_address_list>]")<<endl;
        return EXIT_FAILURE;
    }

    cout<<("OK")<<endl;
    return EXIT_SUCCESS;
}
/*
*   For checking argument
*/
bool args_err(int argc, const char** argv)
{
    return EXIT_SUCCESS;
}
