# libsteam

A nice and easy to use C++ library to interact with Steam and its file formats. Mainly intended for the Steam Deck but it will work elsewhere too

## Features
- Binary VDF File parser (e.g shortcuts.vdf)
  - Parsing
  - Modifying fields
  - Adding new fields
  - Dumping back to binary representation
  - Pretty printing
- KeyValue File parser (e.g config.vdf)
  - Parsing
  - Modifying fields
  - Adding new fields
  - Dumping back to text representation
  - Pretty printing
- Interaction with the Steam Game UI
  - Restarting Game UI
  - Adding new shortcuts to Steam
  - Removing shortcuts from Steam
  - Enabling Proton for shortcuts
- Querying the SteamGridDB API
  - Searching
  - Getting Grids, Heroes, Logos and Icons

## Example

### shortcut.vdf interaction
```cpp

std::vector<u8> vdfFileContent = /* ... */;

steam::VDF vdf(vdfFileContent);

// Print game name of first entry
auto firstGameName = vdf["shortcuts"]["0"]["AppName"].string();
std::printf("%s", firstGameName.c_str());

// Hide first game from UI
vdf["shortcuts"]["0"]["IsHidden"] = true;

// Convert back to binary representation
auto binaryData = vdf.dump();

// Print vdf as formatted data
std::printf("%s", vdf.format().c_str());
```
