#pragma once
#include "parser/instruction.hh"

namespace Minisql
{
    extern bool isQuitting;

    class InstructionHandler
    {
        Instruction::Instruction* inst;

        static void executeCreateTable(Instruction::CreateTable* inst);
        static void executeDropTable(Instruction::DropTable* inst);
        static void executeCreateIndex(Instruction::CreateIndex* inst);
        static void executeDropIndex(Instruction::DropIndex* inst);
        static void executeSelect(Instruction::Select* inst);
        static void executeInsert(Instruction::Insert* inst);
        static void executeDelete(Instruction::Delete* inst);
        static void executeQuit(Instruction::Quit* inst);
        static void executeExecfile(Instruction::Execfile* inst);


    public:
        InstructionHandler(Instruction::Instruction* inst) :inst(inst) {}
        InstructionHandler(const InstructionHandler& handler) = delete;
        InstructionHandler(InstructionHandler&& handler) noexcept :inst(handler.inst) { handler.inst = nullptr; }
        InstructionHandler& operator=(const InstructionHandler& handler) = delete;
        InstructionHandler& operator=(InstructionHandler&& handler) noexcept
        {
            inst = handler.inst;
            handler.inst = nullptr;
            return *this;
        }
        void execute();
        ~InstructionHandler() { delete inst; }

        static InstructionHandler createInstructionHandler();
        static void getNextInstruction();
    };
}