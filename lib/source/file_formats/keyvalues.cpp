#include <steam/file_formats/keyvalues.hpp>

#include <steam/helpers/utils.hpp>

#include <codecvt>

#include <fmt/format.h>

namespace steam {

    std::pair<KeyValues::KeyValuePair, size_t> parseElement(std::u32string_view data);
    std::string dumpElement(const std::string &key, const KeyValues::Value &value, u32 indent);

    std::pair<std::u32string, size_t> parseString(std::u32string_view data) {
        std::u32string result;
        size_t advance = 0;

        if (!data.starts_with('"'))
            return { { }, 0 };

        for (u32 i = 1; i < data.length(); i++) {
            advance++;
            auto character = data[i];

            if (character < 0x7F) {
                // If this is an ASCII character...

                if (character == '\\') {
                    if (i == (data.length() - 1))
                        return { { }, 0 };

                    switch (data[i + 1]) {
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case '\\': result += '\\'; break;
                        case '"': result += '"'; break;
                        default: return { { }, 0 };
                    }
                    i++;
                    advance++;
                } else if (character == '"') {
                    break;
                } else {
                    result += character;
                }
            } else {
                result += character;
            }
        }

        return { result, advance + 1 };
    }

    size_t consumeWhitespace(std::u32string_view data) {
        size_t advance = 0;

        for (const auto &character : data) {
            if (character < 0x7F && std::isspace(static_cast<char>(character & 0x7F))) {
                advance++;
            } else {
                break;
            }
        }

        return advance;
    }

    std::pair<KeyValues::Set, size_t> parseSet(std::u32string_view data) {
        KeyValues::Set result;
        size_t advance = 0;

        if (!data.starts_with('{'))
            return { { }, 0 };

        advance++;
        advance += consumeWhitespace(data.substr(advance));

        while (advance < data.length() && data[advance] != '}') {
            auto [keyValue, valueBytesUsed] = parseElement(data.substr(advance));
            if (valueBytesUsed == 0)
                return { { }, 0 };

            advance += valueBytesUsed;
            advance += consumeWhitespace(data.substr(advance));

            result.emplace(keyValue.key, keyValue.value);
        }

        return { result, advance + 1 };
    }

    std::pair<KeyValues::KeyValuePair, size_t> parseElement(std::u32string_view data) {
        KeyValues::KeyValuePair result;
        size_t advance = 0;

        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

        advance += consumeWhitespace(data);

        {
            const auto [value, bytesUsed] = parseString(data.substr(advance));
            if (bytesUsed == 0)
                return { { }, 0 };

            result.key = converter.to_bytes(value);
            advance += bytesUsed;
        }

        advance += consumeWhitespace(data.substr(advance));

        switch (data[advance]) {
            case '"': {
                auto [value, bytesUsed] = parseString(data.substr(advance));
                result.value.content = converter.to_bytes(value);
                advance += bytesUsed;
                break;
            }
            case '{': {
                auto [value, bytesUsed] = parseSet(data.substr(advance));
                result.value.content = value;
                advance += bytesUsed;
                break;
            }
            default:
                return { { }, 0 };
        }

        return { result, advance };
    }

    KeyValues::Set KeyValues::parse(const std::string &data) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        const auto convertedData = converter.from_bytes(data);

        KeyValues::Set result;
        size_t advance = 0;

        while (advance < convertedData.size()) {
            auto [keyValue, bytesUsed] = parseElement(convertedData.substr(advance));
            if (bytesUsed == 0)
                return { };

            advance += bytesUsed;

            result.emplace(keyValue.key, keyValue.value);

            advance += consumeWhitespace(convertedData.substr(advance));
        }

        return result;
    }

    std::string dumpString(const std::string &string) {
        std::u32string result;

        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        const auto convertedString = converter.from_bytes(string);

        for (const auto &character : convertedString) {
            switch (character) {
                case '\\': result += U"\\\\"; break;
                case '\"': result += U"\\\""; break;
                case '\t': result += U"\\t"; break;
                case '\n': result += U"\\n"; break;
                default:   result += character; break;
            }
        }

        return fmt::format("\"{0}\"", converter.to_bytes(result));
    }

    std::string dumpSet(const KeyValues::Set &set, u32 indent) {
        std::string result;

        result += fmt::format("{0: >{1}}{{\n", "", indent);

        for (const auto &[key, value] : set) {
            result += dumpElement(key, value, indent + 4);
        }

        result += fmt::format("{0: >{1}}}}", "", indent);

        return result;
    }

    std::string dumpElement(const std::string &key, const KeyValues::Value &value, u32 indent) {
        std::string result;

        result += fmt::format("{0: >{1}}{2}", "", indent, dumpString(key));

        result += std::visit(overloaded {
            [](const std::string &value) {
                return fmt::format("\t\t{0}\n", dumpString(value));
            },
            [&indent](const KeyValues::Set &value) {
                return fmt::format("\n{0}\n", dumpSet(value, indent));
            }
        }, value.content);

        return result;
    }

    std::string KeyValues::dump() const {
        std::string result;

        for (const auto &[key, value] : this->m_content) {
            result += dumpElement(key, value, 0);
        }

        return result;
    }
}