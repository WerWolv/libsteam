#include <steam/api/steam_grid_db.hpp>

#include <nlohmann/json.hpp>

namespace steam::api {

    std::future<std::vector<SteamGridDBAPI::SearchEntry>> SteamGridDBAPI::search(const std::string &keyword) {
        return std::async(std::launch::async, [=, this]() -> std::vector<SteamGridDBAPI::SearchEntry> {
            auto response = this->m_net.getJson(
                    SteamGridDBAPI::BaseUrl + "/search/autocomplete/" + this->m_net.encode(keyword),
                    Net::DefaultTimeout,
                    { { "Authorization", "Bearer " + this->m_apiKey } }
                    ).get();

            if (response.code != 200)
                return { };

            auto body = response.body;
            if (!body.contains("success") || !body.contains("data") || body["success"] == false)
                return { };

            std::vector<SteamGridDBAPI::SearchEntry> result;

            for (const auto &entry : body["data"]) {
                if (!(entry.contains("types") && entry.contains("id") && entry.contains("name") && entry.contains("verified")))
                    continue;

                if (!entry["types"].is_array())
                    continue;
                if (!entry["id"].is_number())
                    continue;
                if (!entry["name"].is_string())
                    continue;
                if (!entry["verified"].is_boolean())
                    continue;

                result.push_back({ entry["types"], AppId(entry["id"]), entry["name"], entry["verified"] });
            }

            return result;
        });
    }

    static std::vector<SteamGridDBAPI::ImageResult> parseImageResults(const nlohmann::json &data) {
        std::vector<SteamGridDBAPI::ImageResult> result;

        for (const auto &entry : data) {
            if (!entry.contains("id") || !entry["id"].is_number())
                continue;
            if (!entry.contains("score") || !entry["score"].is_number())
                continue;
            if (!entry.contains("url") || !entry["url"].is_string())
                continue;
            if (!entry.contains("thumb") || !entry["thumb"].is_string())
                continue;

            result.push_back({
                AppId(entry["id"]),
                entry["score"],
                entry["style"],
                entry["url"],
                entry["thumb"]
            });
        }

        return result;
    }

    std::future<std::vector<SteamGridDBAPI::ImageResult>> SteamGridDBAPI::getGrids(const AppId &appId) {
        return std::async(std::launch::async, [=, this]() -> std::vector<SteamGridDBAPI::ImageResult> {
            auto response = this->m_net.getJson(
                    SteamGridDBAPI::BaseUrl + "/grids/game/" + std::to_string(appId.getAppId()),
                    Net::DefaultTimeout,
                    { { "Authorization", "Bearer " + this->m_apiKey } }
            ).get();

            if (response.code != 200)
                return { };

            auto body = response.body;
            if (!body.contains("success") || !body.contains("data") || body["success"] == false)
                return { };

            return parseImageResults(body["data"]);
        });
    }

    std::future<std::vector<SteamGridDBAPI::ImageResult>> SteamGridDBAPI::getHeroes(const AppId &appId) {
        return std::async(std::launch::async, [=, this]() -> std::vector<SteamGridDBAPI::ImageResult> {
            auto response = this->m_net.getJson(
                    SteamGridDBAPI::BaseUrl + "/heroes/game/" + std::to_string(appId.getAppId()),
                    Net::DefaultTimeout,
                    { { "Authorization", "Bearer " + this->m_apiKey } }
            ).get();

            if (response.code != 200)
                return { };

            auto body = response.body;
            if (!body.contains("success") || !body.contains("data") || body["success"] == false)
                return { };

            return parseImageResults(body["data"]);
        });
    }

    std::future<std::vector<SteamGridDBAPI::ImageResult>> SteamGridDBAPI::getLogos(const AppId &appId) {
        return std::async(std::launch::async, [=, this]() -> std::vector<SteamGridDBAPI::ImageResult> {
            auto response = this->m_net.getJson(
                    SteamGridDBAPI::BaseUrl + "/logos/game/" + std::to_string(appId.getAppId()),
                    Net::DefaultTimeout,
                    { { "Authorization", "Bearer " + this->m_apiKey } }
            ).get();

            if (response.code != 200)
                return { };

            auto body = response.body;
            if (!body.contains("success") || !body.contains("data") || body["success"] == false)
                return { };

            return parseImageResults(body["data"]);
        });
    }

    std::future<std::vector<SteamGridDBAPI::ImageResult>> SteamGridDBAPI::getIcons(const AppId &appId) {
        return std::async(std::launch::async, [=, this]() -> std::vector<SteamGridDBAPI::ImageResult> {
            auto response = this->m_net.getJson(
                    SteamGridDBAPI::BaseUrl + "/icons/game/" + std::to_string(appId.getAppId()),
                    Net::DefaultTimeout,
                    { { "Authorization", "Bearer " + this->m_apiKey } }
            ).get();

            if (response.code != 200)
                return { };

            auto body = response.body;
            if (!body.contains("success") || !body.contains("data") || body["success"] == false)
                return { };

            return parseImageResults(body["data"]);
        });
    }

}