#include <iostream>
#include "SymbolTable.h"

int main() {
    std::string line;
    std::getline(std::cin, line);
    std::unique_ptr<pam::ParsedInstruction> res;
    try {
        res = pam::parseInstruction(line);
    } catch (pam::GenericParsingException &e) {
        return 1;
    }

    if (res->getType() != pam::InstructionType::ASSIGN) {
        return 1;
    }
    
    auto *assign = static_cast<pam::ParsedASSIGN *>(res.get()); //NOLINT
    std::cout << "ASSIGN";
    std::cout << '\n' << assign->getName();

    switch (assign->getValueType()) {
        case pam::AssignType::LITERAL_NUMBER:
            std::cout << "\nNUMBER";
            break;
        case pam::AssignType::LITERAL_STRING:
            std::cout << "\nSTRING";
            break;
        case pam::AssignType::IDENTIFIER:
            std::cout << "\nIDENTIFIER";
            std::cout << '\n' << assign->getValueName();
            break;
        case pam::AssignType::FUNC_CALL:
            std::cout << "\nFUNC_CALL";
            std::cout << '\n' << assign->getValueName();
            for (const auto &a : assign->getParams()) {
                std::cout << '\n' << a;
            }
    }
    
    return 0;
}
