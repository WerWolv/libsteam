#include <steam/api/steam_api.hpp>

int main() {

    auto users = steam::api::User::getUsers();
    for (const auto &user : users) {
        printf("%d -> %s\n", user.getId(), user.getName().c_str());
    }

    auto appId = steam::api::addGameShortcut(users.front(), "Hello World", "/home/deck/", "Test Options");
    if (!appId.has_value()) {
        printf("Failed to add game shortcut!\n");
        return EXIT_FAILURE;
    }

    if (!steam::api::enableProtonForApp(*appId, true)) {
        printf("Failed to enable proton for app %d!\n", appId->getShortAppId());
    }

    return EXIT_SUCCESS;
}