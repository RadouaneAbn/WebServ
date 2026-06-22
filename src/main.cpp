#include <webserv.h>
#include <utils.h>
#include <Logger.hpp>
#include <Parser.hpp>
#include <Server.hpp>

bool validate_main(int ac, char **av)
{
    if (ac > 2)
    {
        LOG_ERROR("Usage: " + std::string(av[0]) + " [config_file]");
        return (false);
    }
    return (true);
}

int main(int ac, char **av)
{
    Logger::instance().setLevel(DEBUG);

    std::string config_file;
    // Logger::instance().setFile("webserv.log");
    if (!validate_main(ac, av))
        return (1);
    if (ac == 1)
        config_file = "config_files/default.conf";
    else
        config_file = av[1];
    
    try {
        std::signal(SIGINT, handle_signal);   
        std::signal(SIGTERM, handle_signal);

        Parser parser(config_file);
        Server server(parser.getConfig());
        server.initServer();
        server.startServer();
    }
    catch (const std::exception &e) {
        LOG_ERROR(e.what());
    }

    return (0);
}