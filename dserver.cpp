// Author Tibor Dudlák xdudla00@stud.fit.vutbr.cz
#include <csignal>
#include <iostream>
#include "structures.hpp"
#include "arg_parser.hpp"
#include "req_handler.hpp"

using namespace std;

int srv_socket = -1; // socket has to be closed after SIGINT;

void sig_handler(int signal);
/*
*   Main
*/
int main(int argc, char** argv)
{
    signal(SIGINT, sig_handler);
    scope_settings scope;
    if ( opt_err(argc, argv, &scope) )
    {// if there are wrong arguments program fails on startup
        return EXIT_FAILURE;
    }
    // handling server run and parsing file
    return handle_request(&scope, &srv_socket);
}
/*
 *  function to handle closing socket after SIGINT
 */
void sig_handler(int signal)
{
    cout << "\nInterrupt signal (" << signal << ") received...\n";
    if (srv_socket != -1)
    {
        cout << "Closing server socket...\n";
        close(srv_socket);
    }
    // exiting successfuly after signal
    exit(EXIT_SUCCESS);
}
