#include <steam/file_formats/vdf.hpp>

int main() {

    auto vdfData = steam::fs::File(std::fs::current_path() / "shortcuts.vdf", steam::fs::File::Mode::Read).readBytes();

    auto parsedInput  = steam::VDF(vdfData);
    auto parsedOutput = steam::VDF(parsedInput.dump());

    printf("Input == Output: %d\n", parsedInput == parsedOutput);

    printf("%s\n", parsedInput.format().c_str());

    return EXIT_SUCCESS;
}