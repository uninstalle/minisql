#include "controller.hh"
#include "parser/parser.hh"
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

    }

    void InstructionHandler::executeDropTable(Instruction::DropTable* inst)
    {

    }


    void InstructionHandler::executeCreateIndex(Instruction::CreateIndex* inst)
    {

    }

    void InstructionHandler::executeDropIndex(Instruction::DropIndex* inst)
    {

    }

    void InstructionHandler::executeSelect(Instruction::Select* inst)
    {

    }

    void InstructionHandler::executeInsert(Instruction::Insert* inst)
    {

    }

    void InstructionHandler::executeDelete(Instruction::Delete* inst)
    {

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
