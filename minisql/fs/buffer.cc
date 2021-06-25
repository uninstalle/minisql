#include "buffer.hh"
#include "fs.hh"
#include "../catalog/catalog.hh"
#include "../third-party/tinyxml/tinyxml2.h"
#include <map>

namespace Minisql
{
    tinyxml2::XMLDocument tableHead;
    tinyxml2::XMLDocument tableData;
    std::map <std::string, Buffer*> Buffers;


    Buffer* BufferManager::getTableDataPointer(const std::string& tableName)
    {
        auto buffer = Buffers.find(tableName)->second;
        return buffer;
    }

    Buffer* BufferManager::getTableDataPointerForInsert(const std::string& tableName)
    {
        auto buffer = Buffers.find(tableName)->second;
        while ((buffer->usedSize + buffer->itemSize) > buffer->bufferSize && buffer->next)
            buffer = buffer->next;
        if ((buffer->usedSize + buffer->itemSize) > buffer->bufferSize)
        {
            buffer->extend();
            return buffer->next;
        }
        else
        {
            if (buffer->isDirty)
                writeToDisk();
            return buffer;
        }
    }

    void BufferManager::writeToDisk()
    {
        for (auto b : Buffers)
        {
            auto name = b.first;
            auto buffer = b.second;
            auto itemSize = buffer->itemSize;

            unsigned totalSize = 0;
            while (buffer)
            {
                totalSize += buffer->usedSize;
                buffer = buffer->next;
            }

            auto totalBuffer = new char[totalSize];
            auto totalBufferPtr = totalBuffer;

            buffer = b.second;
            while (buffer)
            {
                memcpy(totalBufferPtr, buffer->ptr, buffer->usedSize);
                totalBufferPtr += buffer->usedSize;
                buffer = buffer->next;
            }
            auto node = tableData.FirstChildElement()->FirstChildElement(name.c_str());
            if (!node)
                node = tableData.FirstChildElement()->InsertNewChildElement(name.c_str());

            node->SetAttribute("Size", totalSize / itemSize);
            node->SetAttribute("ItemSize", itemSize);
            auto base64 = base64_encode(totalBuffer, totalSize);
            node->SetText(base64);
            delete base64;
            delete[] totalBuffer;
        }

        tableData.SaveFile("data.mndb");
    }

    void BufferManager::loadFromDisk()
    {
        auto res = tableData.LoadFile("data.mndb");
        if (res != tinyxml2::XML_SUCCESS)
        {
            tableData.Clear();
            tableData.InsertEndChild(tableData.NewElement("root"));
        }
        if (!tableData.FirstChildElement() || !tableData.FirstChildElement()->FirstChildElement())
            return;

        auto node = tableData.FirstChildElement()->FirstChildElement();
        while (node)
        {
            auto size = std::stoi(node->Attribute("Size"));
            auto itemSize = std::stoi(node->Attribute("ItemSize"));
            auto data = base64_decode(node->GetText());
            auto dataPtr = data;
            auto buffer = new Buffer(itemSize);
            auto currentBuffer = buffer;
            while (size > 0) {
                auto nextBufferWriteItemCount = size < currentBuffer->bufferSize / itemSize ? size : currentBuffer->bufferSize / itemSize;
                memcpy(currentBuffer->ptr, dataPtr, nextBufferWriteItemCount * itemSize);
                currentBuffer->usedSize += nextBufferWriteItemCount * itemSize;
                dataPtr += nextBufferWriteItemCount * itemSize;
                size -= nextBufferWriteItemCount;
                if (size > 0)
                {
                    currentBuffer->extend();
                    currentBuffer = currentBuffer->next;
                }
            }
            delete data;
            Buffers.insert(std::make_pair(node->Name(), buffer));
            node = node->NextSiblingElement();
        }
    }



    bool BufferManager::initTableDataPointer(const std::string& tableName, unsigned itemSize)
    {
        if (Buffers.find(tableName) == Buffers.end())
        {
            auto t = new Buffer(itemSize);
            Buffers.insert(std::make_pair(tableName, t));
            return true;
        }
        return false;
    }

    void HeaderWriter::loadFromDisk()
    {
        auto res = tableHead.LoadFile("head.mndb");
        if (res != tinyxml2::XML_SUCCESS)
        {
            tableHead.Clear();
            tableHead.InsertEndChild(tableHead.NewElement("root"));
        }
        if (!tableHead.FirstChildElement() || !tableData.FirstChildElement()->FirstChildElement())
            return;

        auto node = tableHead.FirstChildElement()->FirstChildElement();
        while (node)
        {
            auto size = std::stoi(node->Attribute("Size"));

            auto attrNode = node->FirstChildElement("Attribute");
            auto anode = attrNode->FirstChildElement();
            std::vector<TableAttribute> attributes;
            while (anode)
            {
                TableAttribute attr;
                attr.name = anode->Name();
                attr.type = static_cast<TableAttribute::AttrType>(std::stoi(anode->Attribute("Type")));
                attr.typeExInfo = std::stoi(anode->Attribute("TypeExInfo"));
                attr.outputWidth = std::stoi(anode->Attribute("OutputWidth"));
                attr.constraint = std::stoi(anode->Attribute("Constraint"));
                attributes.push_back(attr);
                anode = anode->NextSiblingElement();
            }

            Catalog::tables.emplace_back(node->Name(), attributes, size);
            node = node->NextSiblingElement();
        }
    }


    void HeaderWriter::writeToDisk()
    {
        tableHead.SaveFile("head.mndb");
    }

    void HeaderWriter::createTableNode(const Table& t)
    {
        auto root = tableHead.FirstChildElement();
        auto node = root->InsertNewChildElement(t.name.c_str());
        node->SetAttribute("Size", t.size);
        auto attrNode = node->InsertNewChildElement("Attribute");
        for (auto& attr : t.head)
        {
            auto newAttr = attrNode->InsertNewChildElement(attr.name.c_str());
            newAttr->SetAttribute("Type", static_cast<int>(attr.type));
            newAttr->SetAttribute("TypeExInfo", attr.typeExInfo);
            newAttr->SetAttribute("OutputWidth", attr.outputWidth);
            newAttr->SetAttribute("Constraint", attr.constraint);
        }
    }

    void HeaderWriter::deleteTableNode(const std::string& tableName)
    {
        auto headNode = tableHead.FirstChildElement()->FirstChildElement(tableName.c_str());
        tableHead.DeleteNode(headNode);
        auto buffer = Buffers.find(tableName);
        delete buffer->second;
        Buffers.erase(buffer);
    }

    void HeaderWriter::updateTableNode(const std::string& tableName, int size)
    {
        auto node = tableHead.FirstChildElement()->FirstChildElement(tableName.c_str());
        node->SetAttribute("Size", size);
    }



}
