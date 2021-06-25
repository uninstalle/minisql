#pragma once
#include <string>
#include <vector>

namespace Minisql
{
    class HeaderWriter;

    class TableAttribute
    {
    public:
        enum class AttrType
        {
            Integer,
            Char,
            Float
        };
        std::string name;
        AttrType type;
        unsigned typeExInfo = 0;
        unsigned outputWidth;
        uint32_t constraint = 0;
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

    class TableItemIterator
    {
        union VarPtr
        {
            int32_t* Integer;
            float* Float;
            char* Char;
        };
        std::vector<TableAttribute::AttrType> varType;
        std::vector<unsigned> varPtrOffset;
        char* base = nullptr;
    public:
        TableItemIterator(const std::vector<TableAttribute>& head)
        {
            varPtrOffset.push_back(0);
            for (auto& attr : head)
            {
                varType.push_back(attr.type);
                varPtrOffset.push_back(attr.getSizeOf() + varPtrOffset.back());
            }
        }
        VarPtr operator[](unsigned index)
        {
            VarPtr ret;
            switch (varType[index])
            {
            case TableAttribute::AttrType::Integer:
                ret.Integer = reinterpret_cast<int32_t*>(base + varPtrOffset[index]);
                break;
            case TableAttribute::AttrType::Float:
                ret.Float = reinterpret_cast<float*>(base + varPtrOffset[index]);
                break;
            case TableAttribute::AttrType::Char:
                ret.Char = base + varPtrOffset[index];
                break;
            }
            return ret;
        }
        void setBaseAddr(char* base) { this->base = base; }
        void next(unsigned n = 1) { base += n * varPtrOffset.back(); }
        char* raw() { return base; }
    };

    class Table
    {
        friend HeaderWriter;
        std::string name;
        std::vector<TableAttribute> head;
        unsigned rowSize;
        unsigned size;
        TableItemIterator itemIterator;

        void outputHead();
        void outputItem(TableItemIterator& iter);

    public:
        Table(std::string tableName, std::vector<TableAttribute>& tableHead)
            :name(std::move(tableName)), head(std::move(tableHead)), rowSize(0), size(0), itemIterator(head)
        {
            for (auto& attr : head)
                rowSize += attr.getSizeOf();
        }
        Table(std::string tableName, std::vector<TableAttribute>& tableHead, unsigned size)
            :name(std::move(tableName)), head(std::move(tableHead)), rowSize(0), size(size), itemIterator(head)
        {
            for (auto& attr : head)
                rowSize += attr.getSizeOf();
        }
        TableAttribute& getAttribute(unsigned index)
        {
            return head.at(index);
        }
        const std::string& getName() const { return name; }
        unsigned getRowSize()const { return rowSize; }
        bool insertItem(const std::vector<std::string>& values);
        bool deleteItem(const std::vector<Condition>& conditions);
        bool selectItem(const std::vector<Condition>& conditions, std::ostream& os);
    };

    class Catalog
    {
        friend HeaderWriter;
        static std::vector<Table> tables;
    public:
        static bool createTable(const Table& t);
        static bool dropTable(const std::string& name);
        static Table* getTable(const std::string& name);
    };
}
