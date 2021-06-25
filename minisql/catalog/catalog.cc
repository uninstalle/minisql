#include "catalog.hh"
#include "../fs/buffer.hh"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <functional>

namespace Minisql
{
    std::vector<Table> Catalog::tables;

    bool Table::insertItem(const std::vector<std::string>& values)
    {
        auto iter = itemIterator;
        auto buffer = BufferManager::getTableDataPointerForInsert(name);
        iter.setBaseAddr(buffer->ptr + buffer->usedSize);

        try {
            for (int i = 0; i < head.size(); ++i)
            {
                switch (head[i].type)
                {
                case TableAttribute::AttrType::Integer:
                    *iter[i].Integer = std::stoi(values[i]);
                    break;
                case TableAttribute::AttrType::Float:
                    *iter[i].Float = std::stof(values[i]);
                    break;
                case TableAttribute::AttrType::Char:
                    std::strncpy(iter[i].Char, values[i].c_str(), head[i].typeExInfo);
                    iter[i].Char[values[i].size()] = '\0';
                    break;
                }
            }
        }
        catch (std::invalid_argument&)
        {
            std::cout << "Invalid value in inserting.\n";
            return false;
        }
        catch (std::out_of_range&)
        {
            std::cout << "Value out of range in inserting.\n";
            return false;
        }
        catch (std::runtime_error&)
        {
            std::cout << "Unexpected error in inserting.\n";
            return false;
        }

        size++;
        HeaderWriter::updateTableNode(name, size);
        buffer->usedSize += rowSize;
        buffer->isDirty = true;
        return true;
    }

    bool Table::deleteItem(const std::vector<Condition>& conditions)
    {
        std::vector<std::function<bool(TableItemIterator&)>> comparator;

        for (auto& cond : conditions)
        {
            bool isValid = false;
            for (int i = 0; i < head.size(); ++i)
            {
                if (head[i].name == cond.fieldName)
                {
                    isValid = true;
                    switch (head[i].type)
                    {
                    case TableAttribute::AttrType::Integer:
                    {
                        auto val = std::stoi(cond.value);
                        switch (cond.op)
                        {
                        case Condition::Operator::EQ:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer == val;
                                });
                            break;
                        case Condition::Operator::NE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer != val;
                                });
                            break;
                        case Condition::Operator::GT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer > val;
                                });
                            break;
                        case Condition::Operator::LT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer < val;
                                });
                            break;
                        case Condition::Operator::GE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer >= val;
                                });
                            break;
                        case Condition::Operator::LE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer <= val;
                                });
                            break;
                        }
                        break;
                    }
                    case TableAttribute::AttrType::Float:
                    {
                        auto val = std::stof(cond.value);
                        switch (cond.op)
                        {
                        case Condition::Operator::EQ:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float == val;
                                });
                            break;
                        case Condition::Operator::NE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float != val;
                                });
                            break;
                        case Condition::Operator::GT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float > val;
                                });
                            break;
                        case Condition::Operator::LT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float < val;
                                });
                            break;
                        case Condition::Operator::GE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float >= val;
                                });
                            break;
                        case Condition::Operator::LE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float <= val;
                                });
                            break;
                        }
                        break;
                    }
                    case TableAttribute::AttrType::Char:
                    {
                        auto val = cond.value;
                        switch (cond.op)
                        {
                        case Condition::Operator::EQ:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val == iter[i].Char;
                                });
                            break;
                        case Condition::Operator::NE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val != iter[i].Char;
                                });
                            break;
                        case Condition::Operator::GT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val > iter[i].Char;
                                });
                            break;
                        case Condition::Operator::LT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val < iter[i].Char;
                                });
                            break;
                        case Condition::Operator::GE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val >= iter[i].Char;
                                });
                            break;
                        case Condition::Operator::LE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val <= iter[i].Char;
                                });
                            break;
                        }
                        break;
                    }
                    }
                }
            }
            if (!isValid)
            {
                std::cout << "Invalid field in deleting.\n";
                return false;
            }
        }

        auto iter = itemIterator;
        auto buffer = BufferManager::getTableDataPointer(name);
        iter.setBaseAddr(buffer->ptr);

        int deleted = 0;
        for (int i = 0, bufferIndex = 0; i < size; ++i)
        {
            bool isPassed = true;
            for (auto& comp : comparator)
                if (!comp(iter))
                    isPassed = false;
            if (isPassed)
            {
                if ((buffer->usedSize / rowSize) > 1) {
                    auto lastItem = itemIterator;
                    lastItem.setBaseAddr(buffer->ptr);
                    lastItem.next(buffer->usedSize / rowSize - 1);
                    memcpy(iter.raw(), lastItem.raw(), rowSize);
                }
                buffer->usedSize -= rowSize;
                buffer->isDirty = true;
                deleted++;
            }

            if (i == size) break;

            if (bufferIndex < buffer->usedSize / rowSize)
            {
                if (!isPassed)
                {
                    bufferIndex++;
                    iter.next();
                }
            }
            else
            {
                if (size - i > 1)
                {
                    bufferIndex = 0;
                    buffer = buffer->next;
                    iter.setBaseAddr(buffer->ptr);
                }
            }
        }
        size -= deleted;
        HeaderWriter::updateTableNode(name, size);
        return true;
    }

    void Table::outputHead()
    {
        for (int i = 0; i < head.size(); ++i)
        {
            std::cout.width(head[i].outputWidth);
            std::cout << std::left << head[i].name << " ";
        }
        std::cout << std::endl;
    }


    void Table::outputItem(TableItemIterator& iter)
    {
        for (int i = 0; i < head.size(); ++i)
        {
            switch (head[i].type)
            {
            case TableAttribute::AttrType::Integer:
                std::cout.width(head[i].outputWidth);
                std::cout << std::left << *iter[i].Integer;
                break;
            case TableAttribute::AttrType::Float:
                std::cout.width(head[i].outputWidth);
                std::cout << std::left << *iter[i].Float;
                break;
            case TableAttribute::AttrType::Char:
                std::cout.width(head[i].outputWidth);
                std::cout << std::left << iter[i].Char;
                break;
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }

    bool Table::selectItem(const std::vector<Condition>& conditions, std::ostream& os)
    {
        std::vector<std::function<bool(TableItemIterator&)>> comparator;

        for (auto& cond : conditions)
        {
            bool isValid = false;
            for (int i = 0; i < head.size(); ++i)
            {
                if (head[i].name == cond.fieldName)
                {
                    isValid = true;
                    switch (head[i].type)
                    {
                    case TableAttribute::AttrType::Integer:
                    {
                        auto val = std::stoi(cond.value);
                        switch (cond.op)
                        {
                        case Condition::Operator::EQ:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer == val;
                                });
                            break;
                        case Condition::Operator::NE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer != val;
                                });
                            break;
                        case Condition::Operator::GT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer > val;
                                });
                            break;
                        case Condition::Operator::LT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer < val;
                                });
                            break;
                        case Condition::Operator::GE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer >= val;
                                });
                            break;
                        case Condition::Operator::LE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Integer <= val;
                                });
                            break;
                        }
                        break;
                    }
                    case TableAttribute::AttrType::Float:
                    {
                        auto val = std::stof(cond.value);
                        switch (cond.op)
                        {
                        case Condition::Operator::EQ:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float == val;
                                });
                            break;
                        case Condition::Operator::NE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float != val;
                                });
                            break;
                        case Condition::Operator::GT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float > val;
                                });
                            break;
                        case Condition::Operator::LT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float < val;
                                });
                            break;
                        case Condition::Operator::GE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float >= val;
                                });
                            break;
                        case Condition::Operator::LE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return *iter[i].Float <= val;
                                });
                            break;
                        }
                        break;
                    }
                    case TableAttribute::AttrType::Char:
                    {
                        auto val = cond.value;
                        switch (cond.op)
                        {
                        case Condition::Operator::EQ:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val == iter[i].Char;
                                });
                            break;
                        case Condition::Operator::NE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val != iter[i].Char;
                                });
                            break;
                        case Condition::Operator::GT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val > iter[i].Char;
                                });
                            break;
                        case Condition::Operator::LT:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val < iter[i].Char;
                                });
                            break;
                        case Condition::Operator::GE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val >= iter[i].Char;
                                });
                            break;
                        case Condition::Operator::LE:
                            comparator.emplace_back([val, i](TableItemIterator& iter)
                                {
                                    return val <= iter[i].Char;
                                });
                            break;
                        }
                        break;
                    }
                    }
                }
            }
            if (!isValid)
            {
                std::cout << "Invalid field in selecting.\n";
                return false;
            }
        }

        auto iter = itemIterator;
        auto buffer = BufferManager::getTableDataPointer(name);
        iter.setBaseAddr(buffer->ptr);

        outputHead();
        for (int i = 0; i < size; ++i)
        {
            bool isPassed = true;
            for (auto& comp : comparator)
                if (!comp(iter))
                    isPassed = false;
            if (isPassed)
            {
                outputItem(iter);
            }
            iter.next();
        }
        return true;
    }

    bool Catalog::createTable(const Table& t)
    {
        for (auto& table : tables)
            if (table.getName() == t.getName())
            {
                std::cout << "Table name occupied. ";
                return false;
            }
        tables.push_back(t);
        if (!BufferManager::initTableDataPointer(t.getName(), t.getRowSize()))
        {
            std::cout << "Unexpected situation that table data existed. ";
            tables.erase(tables.end() - 1);
            return false;
        }
        HeaderWriter::createTableNode(t);
        return true;
    }

    bool Catalog::dropTable(const std::string& name)
    {
        for (auto t = tables.begin(); t != tables.end(); ++t)
            if (t->getName() == name)
            {
                tables.erase(t);
                HeaderWriter::deleteTableNode(name);
                return true;
            }
        return false;
    }

    Table* Catalog::getTable(const std::string& name)
    {
        for (auto& t : tables)
            if (t.getName() == name)
                return &t;
        return nullptr;
    }



}
