#pragma once

#include <steam.hpp>
#include <steam/helpers/fs.hpp>
#include <steam/api/user.hpp>

#include <string>

namespace steam::api {

    bool addGameShortcut(const User &user, const std::string &appName, const std::fs::path &exePath, const std::string &launchOptions = "");

}