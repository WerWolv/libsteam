#pragma once

#include <steam.hpp>

#include <steam/helpers/fs.hpp>
#include <steam/helpers/file.hpp>
#include <steam/file_formats/keyvalues.hpp>

#include <string>

namespace steam::api {

    class User {
    public:
        explicit User(u32 userId) : m_userId(userId), m_userName(queryUserName(userId)) { }

        static const std::vector<User>& getUsers() {
            static auto users = []{
                std::vector<User> users;

                for (const auto &folder : std::fs::directory_iterator(fs::getSteamDirectory() / "userdata")) {
                    u32 userId = std::stoi(folder.path().filename());
                    if (userId == 0)
                        continue;

                    users.emplace_back(userId);
                }

                std::erase_if(users, [](const User &user){
                    return user.getName().empty();
                });

                return users;
            }();

            return users;
        }

        [[nodiscard]]
        u32 getId() const noexcept {
            return this->m_userId;
        }

        [[nodiscard]]
        const std::string &getName() const noexcept {
            return this->m_userName;
        }

    public:
        static std::string queryUserName(u32 userId) {
            auto localConfigFile = fs::File(fs::getSteamDirectory() / "userdata" / std::to_string(userId) / "config" / "localconfig.vdf", fs::File::Mode::Read);
            if (!localConfigFile.isValid())
                return { };

            auto localConfig = steam::KeyValues(localConfigFile.readString());
            if (!localConfig.contains("UserLocalConfigStore")) return { };

            auto userLocalConfigStore = localConfig["UserLocalConfigStore"];
            if (!userLocalConfigStore.contains("friends")) return { };

            auto friends = userLocalConfigStore["friends"];
            if (!friends.contains(std::to_string(userId))) return { };

            auto user = friends[std::to_string(userId)];
            if (!user.contains("name")) return { };

            return user["name"].string();
        }

        u32 m_userId;
        std::string m_userName;
    };

}