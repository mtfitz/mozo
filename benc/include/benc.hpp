#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <variant>
#include <vector>
#include <map>

namespace benc {
    // a shortcut for representing raw bytes
    using byte_t = uint8_t;

    // algebraic data type!
    class BencTree;
    using BencInt = int64_t;
    using BencString = std::vector<byte_t>;
    using BencList = std::vector<BencTree>;
    using BencDict = std::map<BencString, BencTree>;

    // node types for parse tree
    enum BencType {
        INT,
        STRING,
        LIST,
        DICT,
        NONE
    };

    // parse tree
    class BencTree {
    private:
        BencType m_type;
        std::optional<
            std::variant<BencInt,       // integer 
                        BencString,    // byte string
                        BencList,      // list
                        BencDict>>     // dictionary
            m_data;

    public:
        BencTree() : m_type(NONE), m_data({}) {}
        BencTree(BencInt i) : m_type(INT), m_data(i) {}
        BencTree(BencString v) : m_type(STRING), m_data(v) {}
        BencTree(BencList v) : m_type(LIST), m_data(v) {}
        BencTree(BencDict d) : m_type(DICT), m_data(d) {}
        BencTree(const BencTree& tree) : m_type(tree.m_type), m_data(tree.m_data) {}

        BencType type();

        BencInt& get_int();
        BencString& get_byte_string();
        BencList& get_list();
        BencDict& get_dict();

        std::string to_string(size_t level = 0, bool local_override = false);
    };

    std::optional<BencTree> decode(std::vector<byte_t>& src);
    std::optional<BencTree> decode(std::string& src);
    
    std::optional<std::vector<byte_t>> encode(BencTree& tree);
    std::optional<std::string> encode_string(BencTree& tree);
}