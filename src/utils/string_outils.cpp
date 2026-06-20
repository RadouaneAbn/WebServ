#include <utils.h>

std::string ft_itol(size_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string ft_itoa(int n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}
