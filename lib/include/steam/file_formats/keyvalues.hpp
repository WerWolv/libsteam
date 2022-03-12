#pragma once

#include <steam.hpp>
#include <steam/helpers/fs.hpp>
#include <steam/helpers/file.hpp>

#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include <map>

namespace steam {

    class KeyValues {
    public:

        KeyValues() = default;
        explicit KeyValues(const std::fs::path &path) : m_content(parse(fs::File(path, fs::File::Mode::Read).readString())) { }
        explicit KeyValues(const std::string &content) : m_content(parse(content)) { }

        struct Value;

        using Set = std::map<std::string, Value>;

        struct Value {
            std::variant<Set, std::string> content;

            [[nodiscard]]
            std::string& string() {
                return std::get<std::string>(content);
            }

            [[nodiscard]]
            const KeyValues::Set& set() const {
                return std::get<KeyValues::Set>(content);
            }

            [[nodiscard]]
            KeyValues::Set& set() {
                return std::get<KeyValues::Set>(content);
            }

            [[nodiscard]]
            const std::string& string() const {
                return std::get<std::string>(content);
            }

            [[nodiscard]]
            Value& operator[](const std::string &key) & {
                return std::get<Set>(content)[key];
            }

            [[nodiscard]]
            Value&& operator[](const std::string &key) && {
                return std::move(std::get<Set>(content)[key]);
            }

            [[nodiscard]]
            Value& operator[](const char *key) & {
                return std::get<Set>(content)[key];
            }

            [[nodiscard]]
            Value&& operator[](const char *key) && {
                return std::move(std::get<Set>(content)[key]);
            }

            auto& operator=(const std::string &value) {
                this->content = value;
                return *this;
            }

            auto& operator=(const Set &value) {
                this->content = value;
                return *this;
            }

            [[nodiscard]]
            explicit operator const std::string&() const {
                return this->string();
            }

            [[nodiscard]]
            explicit operator std::string() {
                return this->string();
            }

            bool operator==(const Value &other) const {
                return this->content == other.content;
            }

            bool operator!=(const Value &other) const {
                return this->content != other.content;
            }

            [[nodiscard]]
            bool contains(const std::string &key) {
                auto set = std::get_if<Set>(&this->content);
                if (set == nullptr)
                    return false;

                return set->contains(key);
            }

        };

        struct KeyValuePair {
            std::string key;
            Value value;
        };


        [[nodiscard]]
        Value& operator[](const char *key) {
            return this->m_content[key];
        }

        [[nodiscard]]
        const Value& operator[](const char *key) const {
            return this->m_content.at(key);
        }

        [[nodiscard]]
        Value& operator[](const std::string &key) {
            return this->m_content[key];
        }

        [[nodiscard]]
        const Value& operator[](const std::string &key) const {
            return this->m_content.at(key);
        }

        [[nodiscard]]
        Set& get() {
            return this->m_content;
        }

        [[nodiscard]]
        const Set& get() const {
            return this->m_content;
        }

        [[nodiscard]]
        std::string dump() const;

        [[nodiscard]]
        bool contains(const std::string &key) {
            return this->m_content.contains(key);
        }

        bool operator==(const KeyValues &other) const {
            return this->m_content == other.m_content;
        }

        bool operator!=(const KeyValues &other) const {
            return this->m_content != other.m_content;
        }

    private:
        Set parse(const std::string &data);
        Set m_content;
    };

}