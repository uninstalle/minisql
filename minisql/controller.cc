#include "controller.hh"
#include "parser/parser.hh"

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
        if (inst->getType() == Instruction::Instruction::InstructionType::QUIT)
            isQuitting = true;

    }

}
