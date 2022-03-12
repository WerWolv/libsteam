#pragma once

#include <steam.hpp>
#include <steam/helpers/fs.hpp>

#include <steam/api/appid.hpp>
#include <steam/api/user.hpp>

#include <optional>
#include <string>

namespace steam::api {

    std::optional<AppId> addGameShortcut(const User &user, const std::string &appName, const std::fs::path &exePath, const std::string &launchOptions = "");
    bool enableProtonForApp(AppId appId, bool enabled);

}