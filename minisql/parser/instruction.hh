#pragma once
#include <string>
#include <vector>
#include <deque>

namespace Minisql
{
    namespace Instruction
    {

        class Instruction
        {
        public:
            enum class InstructionType
            {
                CREATE_TABLE,
                DROP_TABLE,
                CREATE_INDEX,
                DROP_INDEX,
                SELECT,
                INSERT,
                DELETE,
                QUIT,
                EXECFILE
            };

            virtual InstructionType getType() = 0;
            virtual ~Instruction() = default;
        };

        class Field
        {
        public:
            std::string fieldName;
            std::string type;
            std::string attribute;

            Field(std::string* name, std::string* type)
                : fieldName(std::move(*name)), type(std::move(*type)), attribute()
            {
                delete name;
                delete type;
            }
            Field(std::string* name, std::string* type, std::string* attribute)
                : fieldName(std::move(*name)), type(std::move(*type)), attribute(std::move(*attribute))
            {
                delete name;
                delete type;
                delete attribute;
            }
        };

        class FieldList
        {
        public:
            std::vector<Field> fieldList;
            unsigned primaryKeyIdentifier;
            bool findPrimaryKey(std::string* key)
            {
                for (int i = 0; i < fieldList.size(); ++i)
                    if (fieldList[i].fieldName == *key)
                    {
                        primaryKeyIdentifier = i;
                        delete key;
                        return true;
                    }
                delete key;
                return false;
            }
        };

        class CreateTable : public Instruction
        {
        public:
            std::string tableName;
            std::vector<Field> fields;
            unsigned primaryKeyIdentifier;

            CreateTable(std::string* name, FieldList* fieldList)
                : tableName(std::move(*name)), fields(std::move(fieldList->fieldList)), primaryKeyIdentifier(fieldList->primaryKeyIdentifier)
            {
                delete name;
                delete fieldList;
            }
            InstructionType getType() override { return InstructionType::CREATE_TABLE; }
        };

        class DropTable : public Instruction
        {
        public:
            std::string tableName;
            DropTable(std::string* name) :tableName(std::move(*name))
            {
                delete name;
            }
            InstructionType getType() override { return InstructionType::DROP_TABLE; }
        };

        class CreateIndex : public Instruction
        {
        public:
            std::string indexName;
            std::string tableName;
            std::string fieldName;
            CreateIndex(std::string* index, std::string* table, std::string* field)
                :indexName(std::move(*index)), tableName(std::move(*table)), fieldName(std::move(*field))
            {
                delete index;
                delete table;
                delete field;
            }
            InstructionType getType() override { return InstructionType::CREATE_INDEX; }
        };

        class DropIndex : public Instruction
        {
        public:
            std::string indexName;
            DropIndex(std::string* index) :indexName(std::move(*index)) {
                delete index;
            }
            InstructionType getType() override { return InstructionType::DROP_INDEX; }
        };

        class Condition {
        public:
            std::string fieldName;
            std::string op;
            std::string literal;
            Condition(std::string* field, std::string* op, std::string* literal)
                :fieldName(std::move(*field)), op(std::move(*op)), literal(std::move(*literal))
            {
                delete field;
                delete op;
                delete literal;
            }
        };

        class ConditionList
        {
        public:
            std::vector<Condition> conditions;
        };

        class Select : public Instruction
        {
        public:
            std::string tableName;
            std::vector<Condition> conditions;
            Select(std::string* table) :tableName(std::move(*table))
            {
                delete table;
            }

            Select(std::string* table, ConditionList* conditions)
                :tableName(std::move(*table)), conditions(std::move(conditions->conditions))
            {
                delete table;
                delete conditions;
            }
            InstructionType getType() override { return InstructionType::SELECT; }
        };

        class ValueList {
        public:
            std::vector<std::string> values;
        };

        class Insert : public Instruction
        {
        public:
            std::string tableName;
            std::vector<std::string> values;
            Insert(std::string* table, ValueList* values)
                : tableName(std::move(*table)), values(std::move(values->values))
            {
                delete table;
                delete values;
            }
            InstructionType getType() override { return InstructionType::INSERT; }
        };

        class Delete : public Instruction
        {
        public:
            std::string tableName;
            std::vector<Condition> conditions;
            Delete(std::string* table) : tableName(std::move(*table))
            {
                delete table;
            }

            Delete(std::string* table, ConditionList* conditions)
                : tableName(std::move(*table)), conditions(std::move(conditions->conditions))
            {
                delete table;
                delete conditions;
            }
            InstructionType getType() override { return InstructionType::DELETE; }
        };

        class Quit : public Instruction
        {
        public:
            InstructionType getType() override { return InstructionType::QUIT; }
        };

        class Execfile : public Instruction
        {
        public:
            std::string fileName;
            Execfile(std::string* file) :fileName(std::move(*file))
            {
                delete file;
            }
            InstructionType getType() override { return InstructionType::EXECFILE; }
        };

    } // namespace Instruction
    class InstructionQueue
    {
        static std::deque<Instruction::Instruction*> instructions;
    public:
        InstructionQueue() = delete;

        static void enqueue(Instruction::Instruction* inst)
        {
            instructions.push_back(inst);
        }
        [[nodiscard]] static Instruction::Instruction* dequeue()
        {
            auto front = instructions.front();
            instructions.pop_front();
            return front;
        }
        static bool isEmpty()
        {
            return instructions.empty();
        }
    };

} // namespace Minisql
