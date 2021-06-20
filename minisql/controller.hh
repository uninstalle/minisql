#pragma once
#include "parser/instruction.hh"

namespace Minisql
{
    extern bool isQuitting;

    class InstructionHandler
    {
        Instruction::Instruction* inst;
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