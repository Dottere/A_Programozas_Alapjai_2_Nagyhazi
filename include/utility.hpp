#ifndef UTILITY_H
#define UTILITY_H

template <typename T, typename... Args>
constexpr bool is_any_of(const T &val, Args &&...args)
{
    return ((val == args) || ...);
}

#include <string_view>

bool starts_with(std::string_view str, std::string_view prefix)
{
    return str.size() >= prefix.size() &&
           str.compare(0, prefix.size(), prefix) == 0;
}

std::string_view trim(std::string_view s)
{
    auto start = s.find_first_not_of(" \n\r\t");
    if (start == std::string_view::npos)
        return "";

    auto end = s.find_last_not_of(" \n\r\t");
    return s.substr(start, end - start + 1);
}

#endif