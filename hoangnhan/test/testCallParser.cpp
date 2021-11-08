#include <iostream>
#include "SymbolTable.h"
#include <memory>
#include <chrono>
int main() {
    std::string line;
    std::getline(std::cin, line);
    std::unique_ptr<pam::ParsedInstruction> res;
    try {
        res = pam::parseInstruction(line);
        std::cout.flush();
    } catch (pam::GenericParsingException &e) {
        return 1;
    }
    if (res->getType() != pam::InstructionType::CALL) {
        return 1;
    }
    auto *actual = static_cast<pam::ParsedCALL *>(res.get());
    std::cout << "CALL\n"
              << actual->getFunctionName();
    for (auto a : actual->getParams()) {
        std::cout << '\n'
                  << a;
    }
}
