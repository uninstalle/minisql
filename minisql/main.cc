#include <iostream>
#include "controller.hh"

int main(int args, char** argv)
{
    while (!Minisql::isQuitting) {
        auto inst = Minisql::InstructionHandler::createInstructionHandler();
        inst.execute();
    }
    return 0;
}
