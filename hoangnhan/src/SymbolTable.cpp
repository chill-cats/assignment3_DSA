#include "SymbolTable.h"

namespace pam {    // parse and match
enum class ProbingMethod {
    LINEAR,
    QUADRARTIC,
    DOUBLE
};

struct ParsedSetupLine {
    ProbingMethod method;
    FixedSizeVec<unsigned long> params;
};

ParsedSetupLine parseSetupLine(const std::string &line) {
    const unsigned long MIN_LEN = 10;
    if (line.length() < MIN_LEN) {
        throw InvalidInstruction(line);
    }

    auto firstSpace = std::find(line.begin(), line.end(), ' ');
    if (firstSpace == line.begin() || firstSpace == line.end()) {
        throw InvalidInstruction(line);
    }

    const char *LINEAR_WORD = "LINEAR";
    const unsigned long LINEAR_LEN = 6;
    if (std::equal(LINEAR_WORD, LINEAR_WORD + LINEAR_LEN, line.begin(), firstSpace)) {    // NOLINT
        auto secondSpace = std::find(std::next(firstSpace), line.end(), ' ');
        if (secondSpace == line.end()) {
            throw InvalidInstruction(line);
        }
        const std::string firstParam{ std::next(firstSpace), secondSpace };
        unsigned long long hashTableSize = 0;

        try {
            hashTableSize = std::stoull(firstParam);
        } catch (...) {
            throw InvalidInstruction(line);
        }
        const std::string secondParam{ std::next(firstSpace), line.end() };
    }

    const char *QUADRARTIC_WORD = "QUADRARTIC";
    const unsigned long QUADRARTIC_LEN = 10;
    if (std::equal(QUADRARTIC_WORD, QUADRARTIC_WORD + QUADRARTIC_LEN, line.begin(), firstSpace)) {    // NOLINT
    }

    const char *DOUBLE_WORD = "DOUBLE";
    const unsigned long DOUBLE_LEN = 6;
    if (std::equal(DOUBLE_WORD, DOUBLE_WORD + DOUBLE_LEN, line.begin(), firstSpace)) {    // NOLINT
    }
    throw InvalidInstruction(line);
}
}    // namespace pam

void SymbolTable::run(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        auto output = this->processLine(line);
        if (printFlag) {
#ifdef __EMSCRIPTEN__
            cout << output << '\n';
#else
            std::cout << output << '\n';
#endif
        }
        printFlag = false;
    }
    this->detectUnclosedBlock();
}

std::string SymbolTable::processLine(const std::string &line) {
    return "";
}

void SymbolTable::detectUnclosedBlock() const {
    if (currentLevel != 0) {
        throw UnclosedBlock(currentLevel);
    }
}
