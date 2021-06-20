#include "controller.hh"
#include "parser/parser.hh"
#include "catalog/catalog.hh"
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
            if (field.type == "int")
                attr.type = TableAttribute::AttrType::Integer;
            else if (field.type == "float")
                attr.type = TableAttribute::AttrType::Float;
            else
            {
                attr.type = TableAttribute::AttrType::Char;
                attr.typeExInfo = std::stoi(field.type.substr(4));
            }
            if (field.attribute == "unique")
                attr.setUnique();
            head.push_back(attr);
        }
        head.at(inst->primaryKeyIdentifier).setPrimaryKey();

        Catalog::createTable(Table(inst->tableName, head));
    }

    void InstructionHandler::executeDropTable(Instruction::DropTable* inst)
    {
        Catalog::dropTable(inst->tableName);
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
        Catalog::getTable(inst->tableName).selectItem(conditions, std::cout);
    }

    void InstructionHandler::executeInsert(Instruction::Insert* inst)
    {
        Catalog::getTable(inst->tableName).insertItem(inst->values);
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
        Catalog::getTable(inst->tableName).deleteItem(conditions);
    }


    void InstructionHandler::executeQuit(Instruction::Quit* inst)
    {
        isQuitting = true;
    }

    void InstructionHandler::executeExecfile(Instruction::Execfile* inst)
    {
        isJIT = false;
        auto fileName = inst->fileName;
        yyin = fopen(fileName.c_str(), "r");
        if (!yyin)
        {
            std::cout << "Unable to open file " << fileName << std::endl;
            return;
        }
        yyparse();
        fclose(yyin);
        yyin = stdin;
        isJIT = true;
    }



}
