#pragma once

#include <string>

namespace steam {

    template<typename ... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<typename ... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    bool isIntegerString(std::string string);

}