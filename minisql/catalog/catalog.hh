#pragma once
#include <string>
#include <vector>
#include "../parser/instruction.hh"

namespace Minisql
{
    class TableAttribute
    {
        uint32_t constraint = 0;
    public:
        enum class AttrType
        {
            Integer,
            Char,
            Float
        };
        std::string name;
        AttrType type;
        int typeExInfo;
        unsigned getSizeOf() const
        {
            switch (type)
            {
            case AttrType::Integer:return 4;
            case AttrType::Char:return typeExInfo;
            case AttrType::Float:return 4;
            }
            return 0;
        }
        void setPrimaryKey() { constraint |= 0x1; }
        void setUnique() { constraint |= 0x2; }
        bool isPrimaryKey() const { return constraint & 0x1; }
        bool isUnique() const { return constraint & 0x2; }

    };

    class Condition
    {
    public:
        enum class Operator
        {
            EQ,
            NE,
            GT,
            LT,
            GE,
            LE
        };
        std::string fieldName;
        Operator op;
        std::string value;
    };

    class Table
    {
        std::string name;
        std::vector<TableAttribute> head;
        unsigned rowSize;
        unsigned size;

    public:
        Table(std::string tableName, std::vector<TableAttribute>& tableHead)
            :name(std::move(tableName)), head(std::move(tableHead)), rowSize(0), size(0)
        {
            for (auto& attr : head)
                rowSize += attr.getSizeOf();
        }
        TableAttribute& getAttribute(unsigned index)
        {
            return head.at(index);
        }
        void insertItem(std::vector<std::string> values);
        void deleteItem(std::vector<Condition> conditions);
        void selectItem(std::vector<Condition> conditions, std::ostream& os);
    };

    class Catalog
    {
        std::vector<Table> tables;
    public:
        static void createTable(Table t);
        static void dropTable(std::string name);
        static Table& getTable(std::string name);
    };
}
