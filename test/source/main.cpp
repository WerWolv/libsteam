#include <steam/api/steam_api.hpp>

int main() {

    auto users = steam::api::User::getUsers();
    for (const auto &user : users) {
        printf("%d -> %s\n", user.getId(), user.getName().c_str());
    }

    steam::api::addGameShortcut(users.front(), "Hello World", "/home/deck/", "Test Options");

    return EXIT_SUCCESS;
}