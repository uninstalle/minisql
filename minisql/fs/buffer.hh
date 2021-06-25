#pragma once
#include <string>


namespace Minisql
{
    class Table;

    class Buffer
    {
    public:
        static constexpr unsigned DefaultBufferSize = 4096;
        char* ptr;
        unsigned itemSize;
        unsigned usedSize;
        unsigned bufferSize;
        bool isDirty = false;
        Buffer* next = nullptr;
        Buffer(char* ptr, unsigned itemSz, unsigned bufferSz = DefaultBufferSize)
            :ptr(ptr), itemSize(itemSz), usedSize(0), bufferSize(bufferSz)
        {}
        Buffer(unsigned itemSz, unsigned bufferSz = DefaultBufferSize)
            :ptr(new char[bufferSz]), itemSize(itemSz), usedSize(0), bufferSize(bufferSz)
        {}
        void extend()
        {
            if (!next)
            {
                next = new Buffer(new char[DefaultBufferSize], itemSize);
                return;
            }

            auto last = next;
            while (last->next)
                last = last->next;
            last->next = new Buffer(new char[DefaultBufferSize], itemSize);
        }
        ~Buffer()
        {
            delete next;
        }
    };

    class BufferManager
    {
    public:
        BufferManager() = delete;
        static void writeToDisk();
        static void loadFromDisk();
        static Buffer* getTableDataPointer(const std::string& tableName);
        static Buffer* getTableDataPointerForInsert(const std::string& tableName);
        static bool initTableDataPointer(const std::string& tableName, unsigned itemSize);
    };

    class HeaderWriter
    {
    public:
        HeaderWriter() = delete;
        static void writeToDisk();
        static void loadFromDisk();
        static void createTableNode(const Table& t);
        static void deleteTableNode(const std::string& tableName);
        static void updateTableNode(const std::string& tableName, int size);
    };
}
