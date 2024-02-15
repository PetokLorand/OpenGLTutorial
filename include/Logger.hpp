#pragma once

#include <format>
#include <cstdio>


template <typename... TArgs>
void log(std::format_string<TArgs...> fmt, TArgs&&... args)
{
    std::puts(std::format(fmt, std::forward<TArgs>(args)...).c_str());
}