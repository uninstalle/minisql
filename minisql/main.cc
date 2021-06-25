#include "controller.hh"

int main(int args, char** argv)
{
    Minisql::InstructionHandler::initialize();
    while (!Minisql::isQuitting) {
        auto instHandler = Minisql::InstructionHandler::createInstructionHandler();
        instHandler.execute();
    }
    Minisql::InstructionHandler::saveBeforeQuit();
    return 0;
}
