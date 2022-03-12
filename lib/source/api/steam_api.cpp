#include <steam/api/steam_api.hpp>
#include <steam/api/appid.hpp>

#include <steam/file_formats/vdf.hpp>

#include <steam/helpers/fs.hpp>
#include <steam/helpers/file.hpp>


#include <fmt/format.h>

namespace steam::api {

    std::optional<AppId> addGameShortcut(const User &user, const std::string &appName, const std::fs::path &exePath, const std::string &launchOptions) {
        auto configPath = fs::getSteamDirectory() / "userdata" / std::to_string(user.getId()) / "config";

        // Create backup of original shortcuts file if there haven't been any modifications done by us yet
        if (!fs::exists(configPath / "shortcuts.vdf.orig"))
            if (!fs::copyFile(configPath / "shortcuts.vdf", configPath / "shortcuts.vdf.orig"))
                return std::nullopt;

        // Create a backup of the current shortcuts file
        if (!fs::remove(configPath / "shortcuts.vdf.bak"))
            return std::nullopt;
        if (!fs::copyFile(configPath / "shortcuts.vdf", configPath / "shortcuts.vdf.bak"))
            return std::nullopt;

        auto shortcutsFile = fs::File(configPath / "shortcuts.vdf", fs::File::Mode::Write);
        if (!shortcutsFile.isValid())
            return std::nullopt;

        auto shortcuts = VDF(shortcutsFile.readBytes());

        u32 nextShortcutId = 0;
        {
            for (const auto &[key, value] : shortcuts["shortcuts"].set()) {
                auto id = std::stoi(key);
                if (id > nextShortcutId)
                    nextShortcutId = id;
            }

            nextShortcutId++;
        }

        auto appId = AppId(exePath, appName);

        {
            VDF::Set shortcut;
            shortcut["AllowDesktopConfig"]  = true;
            shortcut["AllowOverlay"]        = true;
            shortcut["AppName"]             = appName;
            shortcut["Devkit"]              = false;
            shortcut["DevkitGameID"]        = "";
            shortcut["DevkitOverrideAppID"] = false;
            shortcut["Exe"]                 = fmt::format("\"{0}\"", exePath.string());
            shortcut["FlatpakAppID"]        = "";
            shortcut["IsHidden"]            = false;
            shortcut["LastPlayTime"]        = 0;
            shortcut["LaunchOptions"]       = "";
            shortcut["OpenVR"]              = false;
            shortcut["ShortcutPath"]        = "";
            shortcut["StartDir"]            = fmt::format("\"{0}\"", exePath.parent_path().string());
            shortcut["appid"]               = appId.getShortAppId();
            shortcut["icon"]                = "";
            shortcut["tags"]                = VDF::Set{ };

            shortcuts["shortcuts"][std::to_string(nextShortcutId)] = shortcut;
        }

        shortcutsFile.setSize(0);
        shortcutsFile.write(shortcuts.dump());

        return appId;
    }

    bool enableProtonForApp(AppId appId, bool enabled) {
        auto configPath = fs::getSteamDirectory() / "config";

        // Create backup of original config file if there haven't been any modifications done by us yet
        if (!fs::exists(configPath / "config.vdf.orig"))
            if (!fs::copyFile(configPath / "config.vdf", configPath / "config.vdf.orig")) return false;

        // Create a backup of the current config file
        if (!fs::remove(configPath / "config.vdf.bak")) return false;
        if (!fs::copyFile(configPath / "config.vdf", configPath / "config.vdf.bak")) return false;

        auto configFile = fs::File(configPath / "config.vdf", fs::File::Mode::Write);
        if (!configFile.isValid())
            return false;

        auto config = KeyValues(configFile.readString());

        if (enabled) {
            KeyValues::Set entry;

            entry["name"]       = "proton_experimental";
            entry["config"]     = "";
            entry["Priority"]   = "250";

            config["InstallConfigStore"]["Software"]["Valve"]["Steam"]["CompatToolMapping"][std::to_string(appId.getShortAppId())] = entry;
        } else {
            config["InstallConfigStore"]["Software"]["Valve"]["Steam"]["CompatToolMapping"].set().erase(std::to_string(appId.getShortAppId()));
        }

        configFile.setSize(0);
        configFile.write(config.dump());

        return true;
    }


}