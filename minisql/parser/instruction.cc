#include "instruction.hh"

namespace Minisql
{
    std::deque<Instruction::Instruction*> InstructionQueue::instructions;

    bool isJIT = true;
}