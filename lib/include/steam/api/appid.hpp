#pragma once

#include <steam/api/user.hpp>

#include <steam/file_formats/vdf.hpp>

#include <steam/helpers/fs.hpp>
#include <steam/helpers/hash.hpp>

#include <string>
#include <vector>

namespace steam::api {

    class AppId {
    public:
        AppId(const std::fs::path &exePath, const std::string &appName) noexcept
            : m_appId((u64(crc32(exePath.string() + appName) | 0x8000'0000) << 32) | 0x0200'0000) { }

        explicit AppId(u64 appId) : m_appId(appId) { }

        static std::vector<AppId> getAppIds(const api::User &user) {
            auto configPath = fs::getSteamDirectory() / "userdata" / std::to_string(user.getId()) / "config";

            // Open shortcuts file
            auto shortcutsFile = fs::File(configPath / "shortcuts.vdf", fs::File::Mode::Write);
            if (!shortcutsFile.isValid()) {
                return { };
            }

            // Parse shortcuts
            auto shortcuts = VDF(shortcutsFile.readBytes());

            // Query all app IDs from the list
            std::vector<AppId> appIds;
            for (const auto &[key, value] : shortcuts["shortcuts"].set()) {
                if (value.set().contains("appid"))
                    appIds.emplace_back(value.set().at("appid").integer());
            }

            return appIds;
        }

        [[nodiscard]]
        u64 getAppId() const noexcept {
            return this->m_appId;
        }

        [[nodiscard]]
        u32 getShortAppId() const noexcept {
            return this->m_appId >> 32;
        }

        [[nodiscard]]
        u32 getShortcutId() const noexcept {
            return (this->m_appId >> 32) - 0x1'0000'0000;
        }

        [[nodiscard]]
        explicit operator u64() const noexcept {
            return this->m_appId;
        }

    private:
        u64 m_appId;
    };

}