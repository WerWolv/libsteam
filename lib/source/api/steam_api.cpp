#include <steam/api/steam_api.hpp>
#include <steam/api/appid.hpp>

#include <steam/file_formats/vdf.hpp>

#include <steam/helpers/fs.hpp>
#include <steam/helpers/file.hpp>


#include <fmt/format.h>

namespace steam::api {

    bool addGameShortcut(const User &user, const std::string &appName, const std::fs::path &exePath, const std::string &launchOptions) {
        auto shortcutsFile = fs::File(fs::getSteamDirectory() / "userdata" / std::to_string(user.getId()) / "config" / "shortcuts.vdf", fs::File::Mode::Write);
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

        {
            std::map<std::string, VDF::Value> shortcut;
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
            shortcut["appid"]               = AppId(exePath, appName).getShortAppId();
            shortcut["icon"]                = "";
            shortcut["tags"]                = std::map<std::string, VDF::Value>{ };

            shortcuts[std::to_string(nextShortcutId)] = shortcut;
        }

        printf("%s\n", shortcuts.format().c_str());

        return true;
    }


}