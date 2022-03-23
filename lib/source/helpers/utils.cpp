#include <steam/helpers/utils.hpp>

#include <algorithm>

namespace steam {

    bool isIntegerString(std::string string) {
        if (string.starts_with('-'))
            string = string.substr(1);

        return std::all_of(string.begin(), string.end(), ::isdigit);
    }

}