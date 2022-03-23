#include <steam/api/steam_api.hpp>

#include <steam/helpers/net.hpp>
#include <steam/api/steam_grid_db.hpp>

#include <fmt/format.h>

int main() {

    // Print all users
    auto users = steam::api::User::getUsers();
    fmt::print("Users:\n");
    for (const auto &user : users) {
        fmt::print("{} -> {}\n", user.getId(), user.getName().c_str());
    }

    // Print all app ids
    auto appIds = steam::api::AppId::getAppIds(users.front());
    fmt::print("AppIds:\n");
    for (const auto &appId : appIds) {
        fmt::print("{}\n", appId.getAppId());
    }

    // Add new Shortcut
    auto appId = steam::api::addGameShortcut(users.front(), "Hello World", "/home/deck/", "Test Options");
    if (!appId.has_value()) {
        printf("Failed to add game shortcut!\n");
        return EXIT_FAILURE;
    }

    // Enable proton for new shortcut
    if (!steam::api::enableProtonForApp(*appId, true)) {
        printf("Failed to enable proton for app %d!\n", appId->getShortAppId());
        return EXIT_FAILURE;
    }

    // Remove Shortcut
    if (!steam::api::removeGameShortcut(users.front(), *appId)) {
        printf("Failed to remove game shortcut!\n");
        return EXIT_FAILURE;
    }

    // Get Grid, Hero, Icon and Logo for a game from SteamGridDB
    steam::Net::init();

    steam::api::SteamGridDBAPI api("ENTER YOUR API KEY HERE");

    auto searchResult = api.search("Harry Potter").get();
    if (searchResult.empty())
        return EXIT_FAILURE;

    auto gridsResult  = api.getGrids(searchResult[0].id).get();
    auto heroesResult = api.getHeroes(searchResult[0].id).get();
    auto iconsResult  = api.getIcons(searchResult[0].id).get();
    auto logosResult  = api.getLogos(searchResult[0].id).get();

    if (!gridsResult.empty())
        fmt::print("Grid: {}\n", gridsResult[0].url);
    if (!heroesResult.empty())
        fmt::print("Hero: {}\n", heroesResult[0].url);
    if (!iconsResult.empty())
        fmt::print("Icon: {}\n", iconsResult[0].url);
    if (!logosResult.empty())
        fmt::print("Logo: {}\n", logosResult[0].url);

    steam::Net::exit();

    // Restart Steam
    steam::api::restartSteam();

    return EXIT_SUCCESS;
}