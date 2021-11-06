#include <SymbolTable.h>
#include <iostream>
int main() {
    std::string line;
    std::getline(std::cin, line);
    try {
        auto res = pam::parseSetupLine(line);
        switch (res.method) {
        case pam::ProbingMethod::LINEAR:
            std::cout << "LINEAR";
            break;
        case pam::ProbingMethod::QUADRARTIC:
            std::cout << "QUADRATIC";
            break;
        case pam::ProbingMethod::DOUBLE:
            std::cout << "DOUBLE";
        }
        for (auto token : res.params) {
            std::cout << '\n'
                      << token;
        }

    } catch (pam::GenericParsingException &e) {
        return 1;
    }
}
