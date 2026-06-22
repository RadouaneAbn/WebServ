#include <utils.h>
#include <Server.hpp>

void handle_signal(int signum)
{
    if (g_server_running && signum == SIGINT) {
        const char *msg = "\nSIGINT caught. Shutting down webserv core...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
    g_server_running = false;
}