#include <steam/file_formats/vdf.hpp>

#include <steam/helpers/utils.hpp>

#include <span>

#include <fmt/format.h>

namespace steam {

    std::pair<VDF::KeyValuePair, size_t> parseElement(std::span<const u8> data);

    std::pair<std::string, size_t> parseString(std::span<const u8> data) {
        std::string result;
        size_t advance = 0;

        for (u8 byte : data) {
            if (byte == 0x00) break;
            result += char(byte);
            advance++;
        }

        if (data[advance] != 0x00) return { {}, 0 };

        advance++;

        return { result, advance };
    }

    std::pair<u32, size_t> parseInteger(std::span<const u8> data) {
        u32 result;
        size_t advance = 0;

        if (data.size() < 4) return { {}, 0 };

        return { (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24), 4 };
    }

    std::pair<VDF::KeyValuePair, size_t> parseKeyStringValue(std::span<const u8> data) {
        VDF::KeyValuePair result;
        size_t advance = 0;

        {
            auto [key, bytesUsed] = parseString(data);
            result.key = key;
            advance += bytesUsed;
        }

        {
            auto [value, bytesUsed] = parseString({ &data[advance], data.size() - advance });
            result.value.content = value;
            advance += bytesUsed;
        }

        return { result, advance };
    }

    std::pair<VDF::KeyValuePair, size_t> parseKeyIntegerValue(std::span<const u8> data) {
        VDF::KeyValuePair result;
        size_t advance = 0;

        {
            auto [key, bytesUsed] = parseString(data);
            result.key = key;
            advance += bytesUsed;
        }

        {
            auto [value, bytesUsed] = parseInteger({ &data[advance], data.size() - advance });
            result.value.content = value;
            advance += bytesUsed;
        }

        return { result, advance };
    }

    std::pair<VDF::KeyValuePair, size_t> parseSet(std::span<const u8> data) {
        VDF::KeyValuePair result;
        size_t advance = 0;

        {
            auto [value, usedBytes] = parseString(data);

            result.key = value;
            advance += usedBytes;
        }

        result.value.content = VDF::Set{};

        while (true) {
            if (advance >= data.size())
                return { {}, 0 };

            if (data[advance] == VDF::Type::EndSet) {
                advance++;
                break;
            }

            auto [element, usedBytes] = parseElement({ &data[advance], data.size() - advance });
            if (usedBytes == 0)
                return { { }, 0 };

            advance += usedBytes;
            std::get<VDF::Set>(result.value.content).emplace(element.key, element.value);
        }

        return { result, advance };
    }

    std::pair<VDF::KeyValuePair, size_t> parseElement(std::span<const u8> data) {
        VDF::KeyValuePair result;
        size_t advance = 0;

        const auto type = static_cast<VDF::Type>(data[0]);
        const auto span = std::span<const u8>{ &data[1], data.size() - 1 };

        switch (type) {
            case VDF::Type::Set:
                std::tie(result, advance) = parseSet(span);
                break;
            case VDF::Type::Integer:
                std::tie(result, advance) = parseKeyIntegerValue(span);
                break;
            case VDF::Type::String:
                std::tie(result, advance) = parseKeyStringValue(span);
                break;
            case VDF::Type::EndSet:
                break;
            default:
                return { {}, 0 };
        }

        return { result, advance + 1 };
    }

    std::map<std::string, VDF::Value> VDF::parse(const std::vector<u8> &data) {
        Set result;

        u64 offset = 0;
        while (offset < data.size()) {
            auto [element, bytesUsed] = parseElement({ &data[offset], data.size() - offset });

            if (bytesUsed == 0)
                return { };

            if (element.value.content.index() == 0)
                break;

            offset += bytesUsed;
            result.emplace(element.key, element.value);
        }

        return result;
    }

    void dumpKey(VDF::Type type, const std::string &key, std::vector<u8> &result) {
        if (key.empty()) return;

        result.push_back(static_cast<u8>(type));

        std::copy(key.begin(), key.end(), std::back_inserter(result));
        result.push_back(0x00);
    }

    void dumpString(const std::string &key, const std::string &content, std::vector<u8> &result) {
        dumpKey(VDF::Type::String, key, result);

        std::copy(content.begin(), content.end(), std::back_inserter(result));
        result.push_back(0x00);
    }

    void dumpInteger(const std::string &key, u32 content, std::vector<u8> &result) {
        dumpKey(VDF::Type::Integer, key, result);

        result.push_back((content >> 0)  & 0xFF);
        result.push_back((content >> 8)  & 0xFF);
        result.push_back((content >> 16) & 0xFF);
        result.push_back((content >> 24) & 0xFF);
    }

    void dumpSet(const std::string &setKey, const VDF::Set &content, std::vector<u8> &result) {
        dumpKey(VDF::Type::Set, setKey, result);

        for (const auto &[key, value] : content) {
            std::visit(overloaded {
                [&, key = key](const std::string &string) {
                    dumpString(key, string, result);
                },
                [&, key = key](const u32 &integer) {
                    dumpInteger(key, integer, result);
                },
                [&, key = key](const VDF::Set &set) {
                    dumpSet(key, set, result);
                }
            }, value.content);
        }

        result.push_back(static_cast<u8>(VDF::Type::EndSet));
    }

    std::vector<u8> VDF::dump() const {
        std::vector<u8> result;

        dumpSet("", this->m_content, result);

        return result;
    }

    std::string formatImpl(const VDF::Set &content, u32 indent) {
        std::string result;

        for (auto &[key, value] : content) {
            result += fmt::format(",\n{0: >{1}}\"{2}\": ", "", indent, key);
            std::visit(steam::overloaded {
                    [&](const std::string& x) {
                        result += fmt::format("\"{0}\"", x);
                    },
                    [&](const steam::u32& x) {
                        result += fmt::format("{0}", x);
                    },
                    [&](const VDF::Set& x) {
                        result += "{";
                        {
                            auto content = formatImpl(x, indent + 4);
                            if (!content.empty())
                                result += content.substr(1);
                        }
                        result += fmt::format("\n{0: >{1}}}}", "", indent);
                    }
            }, value.content);
        }

        return result;
    }

    std::string VDF::format() const {
        return fmt::format("{{{0}\n}}", formatImpl(this->m_content, 4).substr(1));
    }

}