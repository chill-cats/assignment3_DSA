#include "SymbolTable.h"

namespace pam {    // parse and match

ParsedInstruction::ParsedInstruction(InstructionType type) : m_type(type) {}
InstructionType ParsedInstruction::getType() const noexcept {
    return m_type;
}
ParsedINSERT::ParsedINSERT(std::string name, bool isFunc, unsigned long funcParamCount) : ParsedInstruction(InstructionType::INSERT),
                                                                                          m_name(std::move(name)),
                                                                                          m_isFunc(isFunc),
                                                                                          m_funcParamCount(funcParamCount) {}
ParsedLOOKUP::ParsedLOOKUP(std::string nameToLookup) : ParsedInstruction(InstructionType::LOOKUP), m_nameToLookup(nameToLookup) {}
ParsedCALL::ParsedCALL(std::string functionName, FixedSizeVec<std::string> params) : ParsedInstruction(InstructionType::CALL), m_functionName(functionName), m_params(std::move(params)) {}
const std::string &ParsedCALL::getFunctionName() const noexcept { return m_functionName; }
const FixedSizeVec<std::string> &ParsedCALL::getParams() const noexcept { return m_params; }
const std::string &ParsedINSERT::getName() const noexcept { return m_name; }

bool ParsedINSERT::isFunc() const noexcept {
    return m_isFunc;
}
unsigned long ParsedINSERT::getParamCount() const noexcept {
    return m_funcParamCount;
}

using StrCIter = std::string::const_iterator;
unsigned long strtoul(StrCIter begin, StrCIter end) {
    // INFO: Params are non-negative integer with maximum 6 digit
    auto length = end - begin;
    if (length > 6 || length == 0) {
        throw GenericParsingException();
    }

    unsigned long result = 0;
    for (auto current = begin; current != end; ++current) {
        if (*current < '0' || '9' < *current) {
            throw GenericParsingException();
        }
    }

    switch (length) {
    case 6:
        result += static_cast<unsigned long>(*(end - 6) - '0') * 100000ULL;
        [[clang::fallthrough]];
    case 5:
        result += static_cast<unsigned long>(*(end - 5) - '0') * 10000ULL;
        [[clang::fallthrough]];
    case 4:
        result += static_cast<unsigned long>(*(end - 4) - '0') * 1000ULL;
        [[clang::fallthrough]];
    case 3:
        result += static_cast<unsigned long>(*(end - 3) - '0') * 100ULL;
        [[clang::fallthrough]];
    case 2:
        result += static_cast<unsigned long>(*(end - 2) - '0') * 10ULL;
        [[clang::fallthrough]];
    case 1:
        result += static_cast<unsigned long>(*(end - 1) - '0');
    }
    return result;
}


bool isValidLiteralString(StrCIter begin, StrCIter end) noexcept {
    if (end - begin < 2 || *begin != '\'' || *std::prev(end) != '\'') {
        return false;
    }
    if (!std::all_of(std::next(begin), std::prev(end), [](char c) {
            return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == ' ';
        })) {
        return false;
    }
    return true;
}

bool isValidLiteralNumber(StrCIter begin, StrCIter end) noexcept {
    if (end == begin) {
        return false;
    }
    if (!std::all_of(begin, end, [](char c) { return '0' <= c && c <= '9'; })) {
        return false;
    }
    return true;
}

bool isValidIdentifierName(StrCIter begin, StrCIter end) noexcept {
    if (begin == end) {
        return false;
    }
    if (*begin < 'a' || 'z' < *begin) {
        return false;
    }
    if (std::any_of(begin, end, [](char c) {
            return (c < 'a' || 'z' < c) && (c < 'A' || 'Z' < c) && (c < '0' || '9' < c) && c != '_';
        })) {
        return false;
    }
    return true;
}

std::unique_ptr<ParsedInstruction> parseInsert(StrCIter begin, StrCIter end) {
    if (begin == end) {
        throw GenericParsingException();
    }
    auto sep = std::find(begin, end, ' ');

    const std::string name{ begin, sep };
    // INFO: Name must start with lowercase letter and only contain lowrcase letter, upper case letter, digit and underscore
    if (!isValidIdentifierName(name.begin(), name.end())) {
        throw GenericParsingException();
    }

    bool isFn = false;
    unsigned long paramNum = 0;
    if (sep != end) {    // is function
        isFn = true;
        paramNum = strtoul(std::next(sep), end);
    }

    std::unique_ptr<ParsedInstruction> parsed = std::make_unique<ParsedINSERT>(name, isFn, paramNum);
    return parsed;
}

// INFO: This function parse string with format: "foo(bar,123,'hello world')"
ParsedFunctionCall parseFunctionCall(StrCIter begin, StrCIter end) {
    auto openParan = std::find(begin, end, '(');
    if (openParan == begin || openParan == end) {
        throw GenericParsingException();
    }
    if (!isValidIdentifierName(begin, openParan)) {
        throw GenericParsingException();
    }
    std::string name{ begin, openParan };

    auto closingParan = std::find(std::next(openParan), end, ')');
    if (closingParan == end || closingParan != std::prev(end)) {
        throw GenericParsingException();
    }
    auto startOfArgs = std::next(openParan);
    if (startOfArgs == closingParan) {
        return { name, FixedSizeVec<std::string>() };
    }
    auto paramCount = std::count(startOfArgs, closingParan, ',') + 1;
    FixedSizeVec<std::string> params(paramCount);
    unsigned long currentIndex = 0;
    auto currentEnd = startOfArgs;
    for (; currentEnd != closingParan; ++currentEnd) {
        if (*currentEnd == ',') {
            if (!isValidLiteralNumber(startOfArgs, currentEnd) && !isValidLiteralString(startOfArgs, currentEnd) && !isValidIdentifierName(startOfArgs, currentEnd)) {
                throw GenericParsingException();
            }
            params[currentIndex++] = std::string{ startOfArgs, currentEnd };
            ++currentEnd;
            startOfArgs = currentEnd;
        }
    }
    if (startOfArgs != closingParan) {
        if (!isValidLiteralNumber(startOfArgs, currentEnd) && !isValidLiteralString(startOfArgs, currentEnd) && !isValidIdentifierName(startOfArgs, currentEnd)) {
            throw GenericParsingException();
        }
        params[currentIndex++] = std::string{ startOfArgs, closingParan };
    }
    return { name, std::move(params) };
}

std::unique_ptr<ParsedInstruction> parseCall(StrCIter begin, StrCIter end) {
    auto res = parseFunctionCall(begin, end);
    return std::make_unique<ParsedCALL>(res.functionName, std::move(res.params));
}

std::unique_ptr<ParsedInstruction> parseAssign(StrCIter begin, StrCIter end) {
}

std::unique_ptr<ParsedInstruction> parseLookup(StrCIter begin, StrCIter end) {
    if (!isValidIdentifierName(begin, end)) {
        throw GenericParsingException();
    }
    std::unique_ptr<ParsedInstruction> parsed = std::make_unique<ParsedLOOKUP>(std::string(begin, end));
    return parsed;
}

std::unique_ptr<ParsedInstruction> parseInstruction(const std::string &line) {
    if (line.empty()) {
        throw GenericParsingException();
    }
    if (line == "BEGIN") {
        return std::make_unique<ParsedInstruction>(pam::InstructionType::BEGIN);
    }
    if (line == "END") {
        return std::make_unique<ParsedInstruction>(pam::InstructionType::END);
    }
    if (line == "PRINT") {
        return std::make_unique<ParsedInstruction>(pam::InstructionType::PRINT);
    }

    auto firstSpace = std::find(line.begin(), line.end(), ' ');
    if (firstSpace == line.end()) {
        throw GenericParsingException();
    }

    const std::string_view insertWord = "INSERT";
    if (std::equal(insertWord.begin(), insertWord.end(), line.begin(), firstSpace)) {
        return parseInsert(std::next(firstSpace), line.end());
    }

    const std::string_view assignWord = "ASSIGN";
    if (std::equal(assignWord.begin(), assignWord.end(), line.begin(), firstSpace)) {
        return parseAssign(std::next(firstSpace), line.end());
    }

    const std::string_view callWord = "CALL";
    if (std::equal(callWord.begin(), callWord.end(), line.begin(), firstSpace)) {
        return parseCall(std::next(firstSpace), line.end());
    }

    const std::string_view lookupWord = "LOOKUP";
    if (std::equal(lookupWord.begin(), lookupWord.end(), line.begin(), firstSpace)) {
        return parseLookup(std::next(firstSpace), line.end());
    }
    throw GenericParsingException();
}

FixedSizeVec<unsigned long> parseParamsLINEARorDOUBLE(StrCIter begin, StrCIter end) {
    // 183 934
    // ^      ^
    // |      end
    // begin
    // smallest string: 1 2
    const long minSize = 3;
    if (end - begin < minSize) {
        throw GenericParsingException();
    }
    auto sep = std::find(begin, end, ' ');
    if (sep == end) {
        throw GenericParsingException();
    }
    FixedSizeVec<unsigned long> params(2);
    params[0] = strtoul(begin, sep);
    params[1] = strtoul(std::next(sep), end);
    return params;
}

FixedSizeVec<unsigned long> parseParamsQUADRATIC(StrCIter begin, StrCIter end) {
    // 1 4 3
    // ^    ^
    // |    end
    // begin
    const long minSize = 5;
    if (end - begin < minSize) {
        throw GenericParsingException();
    }
    auto firstSep = std::find(begin, end, ' ');
    if (firstSep == end) {
        throw GenericParsingException();
    }

    FixedSizeVec<unsigned long> params(3);
    params[0] = strtoul(begin, firstSep);

    auto secondSep = std::find(std::next(firstSep), end, ' ');
    if (secondSep == end) {
        throw GenericParsingException();
    }

    params[1] = strtoul(std::next(firstSep), secondSep);
    params[2] = strtoul(std::next(secondSep), end);
    return params;
}

ParsedSetupLine parseSetupLine(const std::string &line) {
    const unsigned long MIN_LEN = 10;
    if (line.length() < MIN_LEN) {
        throw GenericParsingException();
    }

    auto firstSpace = std::find(line.begin(), line.end(), ' ');
    if (firstSpace == line.begin() || firstSpace == line.end()) {
        throw GenericParsingException();
    }

    const std::string_view LINEAR_WORD{ "LINEAR" };
    if (std::equal(LINEAR_WORD.begin(), LINEAR_WORD.end(), line.begin(), firstSpace)) {
        return { ProbingMethod::LINEAR, parseParamsLINEARorDOUBLE(std::next(firstSpace), line.end()) };
    }

    const std::string_view QUADRATIC_WORD{ "QUADRATIC" };
    if (std::equal(QUADRATIC_WORD.begin(), QUADRATIC_WORD.end(), line.begin(), firstSpace)) {
        size_t QUADRATIC_INS_MIN_LEN = 15;
        if (line.length() < QUADRATIC_INS_MIN_LEN) {
            throw GenericParsingException();
        }
        return { ProbingMethod::QUADRARTIC, parseParamsQUADRATIC(std::next(firstSpace), line.end()) };
    }

    const std::string_view DOUBLE_WORD{ "DOUBLE" };
    if (std::equal(DOUBLE_WORD.begin(), DOUBLE_WORD.end(), line.begin(), firstSpace)) {
        return { ProbingMethod::DOUBLE, parseParamsLINEARorDOUBLE(std::next(firstSpace), line.end()) };
    }
    throw GenericParsingException();
}

}    // namespace pam

void SymbolTable::run(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;

    if (!std::getline(file, line)) {
        return;
    }

    try {
        setupHashTable(line);
    } catch (pam::GenericParsingException &e) {
        throw InvalidInstruction(line);
    }

    while (std::getline(file, line)) {
        auto output = this->processLine(line);
        if (printFlag) {
            std::cout << output << '\n';
        }
        printFlag = false;
    }
    this->detectUnclosedBlock();
}

void SymbolTable::setupHashTable(const std::string &setupLine) {
    auto res = pam::parseSetupLine(setupLine);
}

std::string SymbolTable::processLine(const std::string &line) {
    std::unique_ptr<pam::ParsedInstruction> parsed;
    try {
        parsed = pam::parseInstruction(line);
    } catch (pam::GenericParsingException &e) {
        throw InvalidInstruction(line);
    }

    switch (parsed->getType()) {
    case pam::InstructionType::INSERT:
        // TODO: Implement INSERT handler
        break;
    case pam::InstructionType::ASSIGN:
        // TODO: Implement ASSIGN handler
        break;
    case pam::InstructionType::BEGIN:
        // TODO: Implement BEGIN handler
        break;
    case pam::InstructionType::END:
        // TODO: Implement END handler
        break;
    case pam::InstructionType::CALL:
        // TODO: Implement CALL handler
        break;
    case pam::InstructionType::LOOKUP:
        // TODO: Implement LOOKUP handler
        break;
    case pam::InstructionType::PRINT:
        // TODO: Implement PRINT handler
        break;
    }
    throw std::runtime_error("Cannot reach here!");
}

void SymbolTable::detectUnclosedBlock() const {
    if (currentLevel != 0) {
        throw UnclosedBlock(currentLevel);
    }
}
