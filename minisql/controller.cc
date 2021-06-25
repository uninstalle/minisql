#include "controller.hh"
#include "parser/parser.hh"
#include "catalog/catalog.hh"
#include "fs/buffer.hh"
#include <iostream>

extern FILE* yyin;

namespace Minisql
{
    bool isQuitting = false;

    InstructionHandler InstructionHandler::createInstructionHandler()
    {
        while (InstructionQueue::isEmpty())
            getNextInstruction();
        return InstructionQueue::dequeue();
    }

    void InstructionHandler::getNextInstruction()
    {
        std::cout << ">";
        yyparse();
    }

    void InstructionHandler::execute()
    {
        switch (inst->getType())
        {
        case Instruction::Instruction::InstructionType::CREATE_TABLE:
            executeCreateTable(dynamic_cast<Instruction::CreateTable*>(inst));
            break;
        case Instruction::Instruction::InstructionType::DROP_TABLE:
            executeDropTable(dynamic_cast<Instruction::DropTable*>(inst));
            break;
        case Instruction::Instruction::InstructionType::CREATE_INDEX:
            executeCreateIndex(dynamic_cast<Instruction::CreateIndex*>(inst));
            break;
        case Instruction::Instruction::InstructionType::DROP_INDEX:
            executeDropIndex(dynamic_cast<Instruction::DropIndex*>(inst));
            break;
        case Instruction::Instruction::InstructionType::SELECT:
            executeSelect(dynamic_cast<Instruction::Select*>(inst));
            break;
        case Instruction::Instruction::InstructionType::INSERT:
            executeInsert(dynamic_cast<Instruction::Insert*>(inst));
            break;
        case Instruction::Instruction::InstructionType::DELETE:
            executeDelete(dynamic_cast<Instruction::Delete*>(inst));
            break;
        case Instruction::Instruction::InstructionType::QUIT:
            executeQuit(dynamic_cast<Instruction::Quit*> (inst));
            break;
        case Instruction::Instruction::InstructionType::EXECFILE:
            executeExecfile(dynamic_cast<Instruction::Execfile*>(inst));
            break;
        }
    }

    void InstructionHandler::executeCreateTable(Instruction::CreateTable* inst)
    {
        std::vector<TableAttribute> head;
        for (auto& field : inst->fields)
        {
            TableAttribute attr;
            attr.name = std::move(field.fieldName);
            auto nameLen = attr.name.length();
            if (field.type == "int")
            {
                attr.type = TableAttribute::AttrType::Integer;
                attr.outputWidth = nameLen > 10 ? nameLen : 10;
            }
            else if (field.type == "float")
            {
                attr.type = TableAttribute::AttrType::Float;
                attr.outputWidth = nameLen > 10 ? nameLen : 10;
            }
            else
            {
                attr.type = TableAttribute::AttrType::Char;
                attr.typeExInfo = std::stoi(field.type.substr(4)) + 1;
                attr.outputWidth = nameLen > attr.typeExInfo ? nameLen : attr.typeExInfo;
            }
            if (field.attribute == "unique")
                attr.setUnique();
            head.push_back(attr);
        }
        head.at(inst->primaryKeyIdentifier).setPrimaryKey();

        if (Catalog::createTable(Table(inst->tableName, head)))
            std::cout << "Create table succeeded.\n";
        else
            std::cout << "Create table failed.\n";
    }

    void InstructionHandler::executeDropTable(Instruction::DropTable* inst)
    {
        if (Catalog::dropTable(inst->tableName))
            std::cout << "Drop table succeeded.\n";
        else
            std::cout << "Drop table failed.\n";
    }


    void InstructionHandler::executeCreateIndex(Instruction::CreateIndex* inst)
    {
        //TODO
    }

    void InstructionHandler::executeDropIndex(Instruction::DropIndex* inst)
    {
        //TODO
    }

    void InstructionHandler::executeSelect(Instruction::Select* inst)
    {
        std::vector<Condition> conditions;
        for (auto& cond : inst->conditions)
        {
            Condition c;
            c.fieldName = std::move(cond.fieldName);
            c.value = std::move(cond.literal);
            if (cond.op == "=")
                c.op = Condition::Operator::EQ;
            else if (cond.op == "!=")
                c.op = Condition::Operator::NE;
            else if (cond.op == ">")
                c.op = Condition::Operator::GT;
            else if (cond.op == "<")
                c.op = Condition::Operator::LT;
            else if (cond.op == ">=")
                c.op = Condition::Operator::GE;
            else
                c.op = Condition::Operator::LE;
            conditions.push_back(c);
        }

        auto pt = Catalog::getTable(inst->tableName);
        if (!pt)
            std::cout << "Select failed: table not found.\n";
        else if (!pt->selectItem(conditions, std::cout))
            std::cout << "Select failed.\n";
    }

    void InstructionHandler::executeInsert(Instruction::Insert* inst)
    {
        auto pt = Catalog::getTable(inst->tableName);
        if (!pt)
            std::cout << "Insert failed: table not found.\n";
        else if (!pt->insertItem(inst->values))
            std::cout << "Insert failed.\n";
        else
            std::cout << "Insert succeeded.\n";
    }

    void InstructionHandler::executeDelete(Instruction::Delete* inst)
    {
        std::vector<Condition> conditions;
        for (auto& cond : inst->conditions)
        {
            Condition c;
            c.fieldName = std::move(cond.fieldName);
            c.value = std::move(cond.literal);
            if (cond.op == "=")
                c.op = Condition::Operator::EQ;
            else if (cond.op == "!=")
                c.op = Condition::Operator::NE;
            else if (cond.op == ">")
                c.op = Condition::Operator::GT;
            else if (cond.op == "<")
                c.op = Condition::Operator::LT;
            else if (cond.op == ">=")
                c.op = Condition::Operator::GE;
            else
                c.op = Condition::Operator::LE;
            conditions.push_back(c);
        }

        auto pt = Catalog::getTable(inst->tableName);
        if (!pt)
            std::cout << "Delete failed: table not found.\n";
        else if (!pt->deleteItem(conditions))
            std::cout << "Delete failed.\n";
        else
            std::cout << "Delete succeeded.\n";
    }


    void InstructionHandler::executeQuit(Instruction::Quit* inst)
    {
        isQuitting = true;
    }

    void InstructionHandler::executeExecfile(Instruction::Execfile* inst)
    {
        isJIT = false;
        auto fileName = inst->fileName;
        auto backup = yyin;
        yyin = fopen(fileName.c_str(), "r");
        if (!yyin)
        {
            std::cout << "Unable to open file " << fileName << std::endl;
            yyin = backup;
            isJIT = true;
            return;
        }
        yyparse();
        fclose(yyin);
        yyin = backup;
        flex_flush_buffer();
        isJIT = true;
    }

    void InstructionHandler::initialize()
    {

        BufferManager::loadFromDisk();
        HeaderWriter::loadFromDisk();
    }

    void InstructionHandler::saveBeforeQuit()
    {
        BufferManager::writeToDisk();
        HeaderWriter::writeToDisk();
    }


}
