#pragma once

#include <steam/helpers/fs.hpp>
#include <steam/helpers/hash.hpp>

#include <string>

namespace steam {

    class AppId {
    public:
        AppId(const std::fs::path &exePath, const std::string &appName) noexcept
            : m_appId((u64(crc32(exePath.string() + appName) | 0x8000'0000) << 32) | 0x0200'0000) { }

        explicit AppId(u64 appId) : m_appId(appId) { }

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