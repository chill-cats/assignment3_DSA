#include <iostream>
#include <SymbolTable.h>

int main() {
    std::string line;
    std::getline(std::cin, line);

    std::unique_ptr<pam::ParsedInstruction> parsed;
    try {
        parsed = pam::parseInstruction(line);
    } catch (pam::GenericParsingException &e) {
        return 1;
    }
    auto insert = static_cast<pam::ParsedINSERT *>(parsed.get());
    std::cout << "INSERT" << '\n';
    std::cout << insert->getName();
    if (insert->isFunc()) {
        std::cout << '\n'
                  << insert->getParamCount();
    }
    return 0;
}
