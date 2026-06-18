#include <webserv.h>

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
    std::string config_file;
    Logger::instance().setLevel(DEBUG);
    // Logger::instance().setFile("webserv.log");
    if (!validate_main(ac, av))
        return (1);
    if (ac == 1)
        config_file = "config_files/default.conf";
    else
        config_file = av[1];

    
    return (0);
}