#pragma once

#include <steam/api/appid.hpp>

#include <steam/helpers/net.hpp>

namespace steam::api {

    class SteamGridDBAPI {
    public:
        SteamGridDBAPI(const std::string &apiKey) : m_apiKey(apiKey) { }

        const static inline std::string BaseUrl = "https://www.steamgriddb.com/api/v2";

        struct SearchEntry {
            std::vector<std::string> types;
            AppId id;
            std::string name;
            bool verified;
        };

        struct ImageResult {
            AppId id;
            u32 score;
            std::string style;
            std::string url;
            std::string thumb;
        };

        std::future<std::vector<SearchEntry>> search(const std::string &keyword);

        std::future<std::vector<ImageResult>> getGrids(const AppId &appId);
        std::future<std::vector<ImageResult>> getHeroes(const AppId &appId);
        std::future<std::vector<ImageResult>> getLogos(const AppId &appId);
        std::future<std::vector<ImageResult>> getIcons(const AppId &appId);

    private:
        std::string m_apiKey;
        Net m_net;
    };

}