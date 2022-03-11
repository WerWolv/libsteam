#include <steam/file_formats/vdf.hpp>
#include <steam/file_formats/keyvalues.hpp>

int main() {

    {
        auto vdfData = steam::fs::File(std::fs::current_path() / "shortcuts.vdf", steam::fs::File::Mode::Read).readBytes();

        auto parsedInput = steam::VDF(vdfData);
        auto parsedOutput = steam::VDF(parsedInput.dump());

        printf("VDF Input == Output: %d\n", parsedInput == parsedOutput);
    }

    {
        auto kvData = steam::fs::File(std::fs::current_path() / "localconfig.vdf", steam::fs::File::Mode::Read).readString();

        auto parsedInput = steam::KeyValues(kvData);
        auto parsedOutput = steam::KeyValues(parsedInput.dump());

        printf("KeyValue Input == Output: %d\n", parsedInput == parsedOutput);
    }

    return EXIT_SUCCESS;
}