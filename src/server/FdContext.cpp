#include <Server.hpp>

FdContext::FdContext(int c_fd, FDType c_type, Client *c_client) :
    fd(c_fd), type(c_type), client(c_client)
{}

FdContext::FdContext(FdContext &old_obj) :
    fd(old_obj.fd), type(old_obj.type), client(old_obj.client)
{}

FdContext FdContext::operator=(FdContext &old_obj)
{
    if (this != &old_obj) {
        fd = old_obj.fd;
        type = old_obj.type;
        client = old_obj.client;
    }
    return (*this);
}

FdContext::~FdContext(void)
{
    // if (type == CLIENT_FD)
    //     delete client;
}

bool FdContext::operator&(FDType t) const
{
    return type == t;
}