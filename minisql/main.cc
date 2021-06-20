#include <iostream>
#include "controller.hh"

int main(int args, char** argv)
{
    while (!Minisql::isQuitting) {
        auto instHandler = Minisql::InstructionHandler::createInstructionHandler();
        instHandler.execute();
    }
    return 0;
}
