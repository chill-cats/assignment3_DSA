#include "SymbolTable.h"

namespace pam {    // parse and match

ParsedInstruction::ParsedInstruction(InstructionType type) : m_type(type) {}
InstructionType ParsedInstruction::getType() const noexcept {
    return m_type;
}

ParsedINSERT::ParsedINSERT(std::string name,
    bool isFunc,
    unsigned long funcParamCount) : ParsedInstruction(InstructionType::INSERT),
                                    m_name(std::move(name)),
                                    m_isFunc(isFunc),
                                    m_funcParamCount(funcParamCount) {
}
const std::string &ParsedINSERT::getName() const noexcept {
    return m_name;
}
unsigned long ParsedINSERT::getParamCount() const noexcept {
    return m_funcParamCount;
}
bool ParsedINSERT::isFunc() const noexcept {
    return m_isFunc;
}

ParsedLOOKUP::ParsedLOOKUP(std::string nameToLookup) : ParsedInstruction(InstructionType::LOOKUP),
                                                       m_nameToLookup(std::move(nameToLookup)) {}
const std::string &ParsedLOOKUP::getNameToLookup() const noexcept {
    return m_nameToLookup;
}

ParsedCALL::ParsedCALL(std::string functionName,
    FixedSizeVec<std::string> params) : ParsedInstruction(InstructionType::CALL),
                                        m_functionName(std::move(functionName)), m_params(std::move(params)) {}
const std::string &ParsedCALL::getFunctionName() const noexcept {
    return m_functionName;
}
const FixedSizeVec<std::string> &ParsedCALL::getParams() const noexcept {
    return m_params;
}

ParsedASSIGN::ParsedASSIGN(std::string name,
    AssignType valueType,
    std::string valueName,
    FixedSizeVec<std::string> params) : ParsedInstruction(InstructionType::ASSIGN),
                                        m_name(std::move(name)),
                                        m_valueType(valueType),
                                        m_valueName(std::move(valueName)),
                                        m_params(std::move(params)) {}
const std::string &ParsedASSIGN::getName() const noexcept {
    return m_name;
}
AssignType ParsedASSIGN::getValueType() const noexcept {
    return m_valueType;
}
const std::string &ParsedASSIGN::getValueName() const noexcept {
    return m_valueName;
}
const FixedSizeVec<std::string> &ParsedASSIGN::getParams() const noexcept {
    return m_params;
}

using StrCIter = std::string::const_iterator;
unsigned long strtoul(StrCIter begin, StrCIter end) {
    // INFO: Params are non-negative integer with maximum 6 digit
    auto length = end - begin;
    if (length > 6 || length == 0) {    // NOLINT
        throw GenericParsingException();
    }

    unsigned long result = 0;
    for (auto current = begin; current != end; ++current) {
        if (*current < '0' || '9' < *current) {
            throw GenericParsingException();
        }
    }
    switch (length) {
    case 6:                                                                    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        result += static_cast<unsigned long>(*(end - 6) - '0') * 100000ULL;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        [[clang::fallthrough]];
    case 5:                                                                   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        result += static_cast<unsigned long>(*(end - 5) - '0') * 10000ULL;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        [[clang::fallthrough]];
    case 4:
        result += static_cast<unsigned long>(*(end - 4) - '0') * 1000ULL;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        [[clang::fallthrough]];
    case 3:
        result += static_cast<unsigned long>(*(end - 3) - '0') * 100ULL;    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        [[clang::fallthrough]];
    case 2:
        result += static_cast<unsigned long>(*(end - 2) - '0') * 10ULL;    // NOLINT
        [[clang::fallthrough]];
    case 1:
        result += static_cast<unsigned long>(*(end - 1) - '0');
        break;
    default:
        throw std::logic_error("Cannot reach here");
    }
    return result;
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
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
    FixedSizeVec<std::string> params(static_cast<unsigned long>(paramCount));
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
    if (end - begin < 3) {
        throw GenericParsingException();
    }
    auto sep = std::find(begin, end, ' ');
    if (sep == end) {
        throw GenericParsingException();
    }
    if (!isValidIdentifierName(begin, sep)) {
        throw GenericParsingException();
    }
    std::string name{ begin, sep };
    AssignType type = AssignType::FUNC_CALL;

    if (isValidLiteralNumber(std::next(sep), end)) {
        type = AssignType::LITERAL_NUMBER;
    } else if (isValidLiteralString(std::next(sep), end)) {
        type = AssignType::LITERAL_STRING;
    } else if (isValidIdentifierName(std::next(sep), end)) {
        type = AssignType::IDENTIFIER;
    }

    if (type == AssignType::IDENTIFIER) {
        return std::make_unique<ParsedASSIGN>(name, type, std::string{ std::next(sep), end }, FixedSizeVec<std::string>());
    }
    if (type == AssignType::FUNC_CALL) {
        auto parsedFunctionCall = parseFunctionCall(std::next(sep), end);
        return std::make_unique<ParsedASSIGN>(name, type, std::move(parsedFunctionCall.functionName), std::move(parsedFunctionCall.params));
    }
    return std::make_unique<ParsedASSIGN>(name, type, "", FixedSizeVec<std::string>());
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
        const size_t QUADRATIC_INS_MIN_LEN = 15;
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

Symbol::Symbol(std::string &&name, unsigned long level, SymbolType symbolType) : m_symbolType(symbolType),
                                                                                 m_name(name),
                                                                                 m_level(level) {}
Symbol::SymbolType Symbol::getSymbolType() const noexcept {
    return m_symbolType;
}

const std::string &Symbol::getName() const noexcept {
    return m_name;
}

unsigned long Symbol::getLevel() const noexcept {
    return m_level;
}

Symbol::DataType Symbol::getDataType() const noexcept {
    return m_dataType;
}

void Symbol::setDataType(DataType type) noexcept {
    m_dataType = type;
}

std::string Symbol::toKey() const {
    if (isKeyCalculated) {
        return key;
    }
    std::string newKey = std::to_string(m_level);
    for (char c : m_name) {
        if (c >= ':') {
            newKey += static_cast<char>(((c - 48) / 10) + '0');    // NOLINT
            newKey += static_cast<char>(((c - 48) % 10) + '0');    // NOLINT
        } else {
            newKey += static_cast<char>((c - 48) + '0');    // NOLINT
        }
    }
    key = std::move(newKey);
    return key;
}

std::string toKey(unsigned long level, const std::string &name) {
    std::string newKey = std::to_string(level);
    for (char c : name) {
        if (c >= ':') {
            newKey += static_cast<char>(((c - 48) / 10) + '0');    // NOLINT
            newKey += static_cast<char>(((c - 48) % 10) + '0');    // NOLINT
        } else {
            newKey += static_cast<char>((c - 48) + '0');    // NOLINT
        }
    }
    return newKey;
}

FunctionSymbol::FunctionSymbol(std::string name,
    unsigned long level,    // NOLINT
    unsigned long paramsNum) : Symbol(std::move(name), level, SymbolType::FUNC),
                               m_paramsType(FixedSizeVec<DataType>(paramsNum)) {
    std::fill(m_paramsType.begin(), m_paramsType.end(), DataType::UN_INFERRED);
}

decltype(FunctionSymbol::m_paramsType) &FunctionSymbol::getParams() noexcept {
    return m_paramsType;
}

VariableSymbol::VariableSymbol(std::string name,
    unsigned long level) : Symbol(std::move(name), level, SymbolType::VAR) {}

SymbolTable::HashEntry::HashEntry(std::unique_ptr<Symbol> &&value) : value(std::move(value)) {}
const std::unique_ptr<Symbol> &SymbolTable::HashEntry::getValue() const noexcept {
    return value;
}
bool SymbolTable::HashEntry::isTombStone() const noexcept {
    return isTombstone;
}
void SymbolTable::HashEntry::setTombStone() noexcept {
    isTombstone = true;
}
void SymbolTable::HashEntry::unsetTombStone() noexcept {
    isTombstone = false;
}

void SymbolTable::HashEntry::setValue(std::unique_ptr<Symbol> &&newVal) {
    value = std::move(newVal);
}


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
        this->processLine(line);
    }
    this->detectUnclosedBlock();
}

unsigned long SymbolTable::hashFunc(unsigned long level, const std::string &name) {
    auto size = container.size();
    auto hash = level;

    for (auto a : name) {
        unsigned long c = static_cast<unsigned long>(a - '0');    // NOLINT
        hash %= size;

        hash *= c >= 10 ? 100 : 10;    // NOLINT

        hash %= size;
        c %= size;
        hash += c;
        hash %= size;
    }
    return hash;
}

/*
unsigned long SymbolTable::doubleHashFunc(const Symbol *symbol) {
    auto size = container.size() - 2;
    auto hash = symbol->getLevel();

    for (auto a : symbol->getName()) {
        unsigned long c = static_cast<unsigned long>(a - '0');    // NOLINT
        hash %= size;

        hash *= a > 10 ? 100 : 10;    // NOLINT

        hash %= size;
        c %= size;
        hash += c;
        hash %= size;
    }
    return hash + 1;
}

unsigned long SymbolTable::doubleHashFunc(unsigned long level, const std::string &name) {
    auto size = container.size() - 2;
    auto hash = level;

    for (auto a : name) {
        unsigned long c = static_cast<unsigned long>(a - '0');    // NOLINT
        hash %= size;

        hash *= a > 10 ? 100 : 10;    // NOLINT

        hash %= size;
        c %= size;
        hash += c;
        hash %= size;
    }
    return hash + 1;
}
*/
void SymbolTable::setupHashTable(const std::string &setupLine) {
    auto res = pam::parseSetupLine(setupLine);
    container = FixedSizeVec<HashEntry>(res.params[0]);

    switch (res.method) {
    case pam::ProbingMethod::LINEAR: {
        auto c = res.params[1];
        getIndex = [this, c](unsigned int iter, unsigned long firstHash, unsigned long) -> unsigned long {    // NOLINT
            return (firstHash + (c * iter) % container.size()) % container.size();
        };

        doubleHashFunc = [](unsigned long, const std::string &) -> unsigned long {    // NOLINT
            return std::numeric_limits<unsigned long>::max();
        };

#ifdef DEBUG
        std::clog << "Setup hash table with LINEAR probing, size: " << res.params[0] << " and c: " << res.params[1] << '\n';
#endif
        break;
    }
    case pam::ProbingMethod::DOUBLE: {
        auto c = res.params[1];
        getIndex = [this, c](unsigned int iter, unsigned long firstHash, unsigned long secondHash) -> unsigned long {
            return (firstHash
                       + ((c * iter * secondHash) % container.size()))
                   % container.size();
        };
        doubleHashFunc = [this](unsigned long level, const std::string &name) -> unsigned long {
            auto size = container.size() - 2;
            auto hash = level;

            for (auto a : name) {
                unsigned long c = static_cast<unsigned long>(a - '0');    // NOLINT
                hash %= size;

                hash *= c > 10 ? 100 : 10;    // NOLINT

                hash %= size;
                c %= size;
                hash += c;
                hash %= size;
            }
            return hash + 1;
        };
#ifdef DEBUG
        std::clog << "Setup hash table with DOUBLE probing, size: " << res.params[0] << " and c: " << res.params[1] << '\n';
#endif
        break;
    }
    case pam::ProbingMethod::QUADRARTIC:
        auto c1 = res.params[1];
        auto c2 = res.params[2];
        getIndex = [this, c1, c2](unsigned int iter, unsigned long firstHash, unsigned long) -> unsigned long {    // NOLINT
            return (firstHash
                       + (c1 * iter) % container.size()
                       + (c2 * iter * iter) % container.size())
                   % container.size();
        };
        doubleHashFunc = [](unsigned long, const std::string &) -> unsigned long {
            return std::numeric_limits<unsigned long>::max();
        };
#ifdef DEBUG
        std::clog << "Setup hash table with QUADRATIC probing, size: " << res.params[0] << ", c1: " << res.params[1] << " and c2: " << res.params[2] << '\n';
#endif
        break;
    }
}

unsigned long SymbolTable::insert(const pam::ParsedINSERT *parsed) {
    if (container.empty()) {
        throw sbtexcept::GenericOverflowException();
    }

    std::unique_ptr<Symbol> newSymbol;
    if (parsed->isFunc()) {
        if (currentLevel == 0) {
            newSymbol = std::make_unique<FunctionSymbol>(parsed->getName(), currentLevel, parsed->getParamCount());
        } else {
            throw InvalidDeclaration(parsed->getName());
        }
    } else {
        newSymbol = std::make_unique<VariableSymbol>(parsed->getName(), currentLevel);
    }

#ifdef DEBUG
    std::clog << "Trying to insert with name: \"" << parsed->getName() << "\", is Func: " << parsed->isFunc() << '\n';
#endif

    auto probingIter = 0U;

    const auto firstHash = hashFunc(currentLevel, newSymbol->getName());
    const auto secondHash = doubleHashFunc(currentLevel, newSymbol->getName());

#ifdef DEBUG
    std::clog << "firstHash: " << firstHash << ", secondHash: " << secondHash << '\n';
#endif

    const auto initialPosition = getIndex(probingIter, firstHash, secondHash);
    auto position = initialPosition;

    for (; !container[position].isTombStone() && container[position].getValue();
         position = getIndex(++probingIter, firstHash, secondHash)) {
#ifdef DEBUG
        std::clog << "Trying to insert to position: " << position << '\n';
#endif
        if (/*(probingIter != 0 && position == initialPosition) || */ probingIter > container.size()) {
            throw sbtexcept::GenericOverflowException();
        }

        if (container[position].getValue()->toKey() == newSymbol->toKey()) {
            throw Redeclared(parsed->getName());
        }
    }
#ifdef DEBUG
    std::clog << "Inserting to position: " << position << '\n';
#endif
    container[position].setValue(std::move(newSymbol));
    container[position].unsetTombStone();
    return probingIter;
}

void SymbolTable::begin() noexcept {
    currentLevel++;
}

void SymbolTable::end() {
    if (currentLevel == 0) {
        throw UnknownBlock();
    }

    if (container.empty()) {
        return;
    }

    for (auto &entry : container) {
        if (entry.getValue() && entry.getValue()->getLevel() == currentLevel) {
            entry.setTombStone();
        }
    }
    currentLevel--;
}

void SymbolTable::print() {
    if (container.empty()) {
        return;
    }
    auto entryIndex = 0;
    bool firstEntry = true;
    for (const auto &entry : container) {
        if (entry.getValue() && !entry.isTombStone()) {
            std::cout
                << (firstEntry ? "" : ";")
                << entryIndex
                << ' '
                << entry.getValue()->getName()
                << "//"
                << entry.getValue()->getLevel();
            firstEntry = false;
        }
        ++entryIndex;
    }
    if (!firstEntry) {
        std::cout << '\n';
    }
}

SymbolTable::LookupResult SymbolTable::lookup(pam::ParsedLOOKUP *parsed) {
    return lookup(parsed->getNameToLookup());
}

SymbolTable::LookupResult SymbolTable::lookup(const std::string &name) {
    if (container.empty()) {
        throw Undeclared(name);
    }

    for (long level = static_cast<long>(currentLevel); level >= 0; level--) {
        auto firstHash = hashFunc(static_cast<unsigned long>(level), name);
        auto secondHash = doubleHashFunc(static_cast<unsigned long>(level), name);

        auto iter = 0U;
        auto initialPosition = getIndex(iter, firstHash, secondHash);
        for (auto position = initialPosition;; position = getIndex(++iter, firstHash, secondHash)) {
            if (container[position].getValue() == nullptr || iter >= container.size()) {
                break;
            }
            if (!container[position].isTombStone()    // we not encounter deleted item
                && container[position].getValue()->getLevel() == static_cast<unsigned long>(level)
                && container[position].getValue()->getName() == name) {    // name is matching
                return { position, &container[position], iter };
            }
        }
    }
    throw Undeclared(name);
}

void SymbolTable::compareTypeAndInferIfNeeded(Symbol::DataType targetType, Symbol::DataType &unknownType) {
    if (unknownType == Symbol::DataType::UN_INFERRED) {
        unknownType = targetType;
    } else if (unknownType != targetType) {
        throw sbtexcept::TypeMismatch();
    }
}

void SymbolTable::compareTypeAndInferIfNeeded(Symbol::DataType targetType, Symbol &unknownSymbol) {
    if (unknownSymbol.getDataType() == Symbol::DataType::UN_INFERRED) {
        unknownSymbol.setDataType(targetType);
    } else if (unknownSymbol.getDataType() != targetType) {
        throw sbtexcept::TypeMismatch();
    }
}

void SymbolTable::compareTypeAndInferIfNeeded(Symbol::DataType &unknownType, Symbol &unknownSymbol) {
    if (unknownType == Symbol::DataType::UN_INFERRED
        && unknownSymbol.getDataType() == Symbol::DataType::UN_INFERRED) {
        throw sbtexcept::InferError();
    }

    if (unknownType == Symbol::DataType::UN_INFERRED) {
        unknownType = unknownSymbol.getDataType();
    } else if (unknownSymbol.getDataType() == Symbol::DataType::UN_INFERRED) {
        unknownSymbol.setDataType(unknownType);
    } else if (unknownSymbol.getDataType() != unknownType) {
        throw sbtexcept::TypeMismatch();
    }
}

void SymbolTable::compareTypeAndInferIfNeeded(Symbol &unknownSymbol1, Symbol &unknownSymbol2) {
    if (unknownSymbol1.getDataType() == Symbol::DataType::UN_INFERRED && unknownSymbol2.getDataType() == Symbol::DataType::UN_INFERRED) {
        throw sbtexcept::InferError();
    }

    if (unknownSymbol2.getDataType() == Symbol::DataType::UN_INFERRED) {
        unknownSymbol2.setDataType(unknownSymbol1.getDataType());

    } else if (unknownSymbol1.getDataType() == Symbol::DataType::UN_INFERRED) {
        unknownSymbol1.setDataType(unknownSymbol2.getDataType());

    } else if (unknownSymbol1.getDataType() != unknownSymbol2.getDataType()) {
        throw sbtexcept::TypeMismatch();
    }
}

unsigned long SymbolTable::call(const pam::ParsedCALL *parsed) {
    if (container.empty()) {
        throw Undeclared(parsed->getFunctionName());
    }

    auto totalNumOfProbes = 0UL;

    auto functionLookupRes = lookup(parsed->getFunctionName());    // NOTE: Unhandled Undeclared will be handle by caller
    auto *functionSymbol = functionLookupRes.ptr->getValue().get();
    if (functionSymbol->getSymbolType() != Symbol::SymbolType::FUNC) {
        throw sbtexcept::TypeMismatch();
    }

    auto *castedFuncSymbol = static_cast<FunctionSymbol *>(functionSymbol);    // NOLINT conversion is safe
    if (parsed->getParams().size() != castedFuncSymbol->getParams().size()) {
        throw sbtexcept::TypeMismatch();
    }

    for (auto i = 0U; i < parsed->getParams().size(); i++) {
        const auto &param = parsed->getParams()[i];

        if ('0' <= *param.begin() && *param.begin() <= '9') {    // number
            compareTypeAndInferIfNeeded(Symbol::DataType::NUMBER, castedFuncSymbol->getParams()[i]);

        } else if (*param.begin() == '\'') {    // string
            compareTypeAndInferIfNeeded(Symbol::DataType::STRING, castedFuncSymbol->getParams()[i]);

        } else {                               // name
            auto lookupRes = lookup(param);    // NOTE: Unhandled Undeclared will be handle by caller
            auto *symbol = lookupRes.ptr->getValue().get();

            if (symbol->getSymbolType() == Symbol::SymbolType::FUNC) {
                throw sbtexcept::TypeMismatch();
            }
            auto *varSymbol = static_cast<VariableSymbol *>(symbol);    // NOLINT conversion is safe here

            static_cast<void (&)(Symbol::DataType &, Symbol &)>(compareTypeAndInferIfNeeded)(castedFuncSymbol->getParams()[i], *varSymbol);

            totalNumOfProbes += lookupRes.numOfProbes;
        }
    }
    compareTypeAndInferIfNeeded(Symbol::DataType::VOID, *castedFuncSymbol);
    totalNumOfProbes += functionLookupRes.numOfProbes;
    return totalNumOfProbes;
}

unsigned long SymbolTable::assign(const pam::ParsedASSIGN *parsed) {    // NOLINT
    const auto &assignee = parsed->getName();
    auto totalEntry = 0UL;

    switch (parsed->getValueType()) {
    case pam::AssignType::LITERAL_NUMBER: {
        auto assigneeLookupRes = lookup(assignee);
        auto *assigneeSymbol = assigneeLookupRes.ptr->getValue().get();
        if (assigneeSymbol->getSymbolType() == Symbol::SymbolType::FUNC) {
            throw sbtexcept::TypeMismatch();
        }
        compareTypeAndInferIfNeeded(Symbol::DataType::NUMBER, *assigneeSymbol);

        totalEntry += assigneeLookupRes.numOfProbes;
        break;
    }
    case pam::AssignType::LITERAL_STRING: {
        auto assigneeLookupRes = lookup(assignee);
        auto *assigneeSymbol = assigneeLookupRes.ptr->getValue().get();
        if (assigneeSymbol->getSymbolType() == Symbol::SymbolType::FUNC) {
            throw sbtexcept::TypeMismatch();
        }
        compareTypeAndInferIfNeeded(Symbol::DataType::STRING, *assigneeSymbol);

        totalEntry += assigneeLookupRes.numOfProbes;
        break;
    }
    case pam::AssignType::IDENTIFIER: {
        auto assignerLookupRes = lookup(parsed->getValueName());
        auto *assignerSymbol = assignerLookupRes.ptr->getValue().get();
        if (assignerSymbol->getSymbolType() == Symbol::SymbolType::FUNC) {
            throw sbtexcept::TypeMismatch();
        }

        auto assigneeLookupRes = lookup(assignee);
        auto *assigneeSymbol = assigneeLookupRes.ptr->getValue().get();
        if (assigneeSymbol->getSymbolType() == Symbol::SymbolType::FUNC) {
            throw sbtexcept::TypeMismatch();
        }
        compareTypeAndInferIfNeeded(*assigneeSymbol, *assignerSymbol);

        totalEntry += assigneeLookupRes.numOfProbes;
        totalEntry += assignerLookupRes.numOfProbes;
        break;
    }
    case pam::AssignType::FUNC_CALL: {
        auto functionSymbolLookupRes = lookup(parsed->getValueName());
        auto *functionSymbol = functionSymbolLookupRes.ptr->getValue().get();
        if (functionSymbol->getSymbolType() == Symbol::SymbolType::VAR) {
            throw sbtexcept::TypeMismatch();
        }

        auto *actualFunctionSymbol = static_cast<FunctionSymbol *>(functionSymbol);    // NOLINT conversion is safe
        if (actualFunctionSymbol->getParams().size() != parsed->getParams().size()) {
            throw sbtexcept::TypeMismatch();
        }

        for (auto i = 0UL; i < actualFunctionSymbol->getParams().size(); i++) {
            const auto &param = parsed->getParams()[i];

            if ('0' <= *param.begin() && *param.begin() <= '9') {    // number
                compareTypeAndInferIfNeeded(Symbol::DataType::NUMBER, actualFunctionSymbol->getParams()[i]);
            } else if (*param.begin() == '\'') {    // string
                compareTypeAndInferIfNeeded(Symbol::DataType::STRING, actualFunctionSymbol->getParams()[i]);
            } else {                               // name
                auto lookupRes = lookup(param);    // NOTE: Unhandled Undeclared will be handle by caller
                auto *symbol = lookupRes.ptr->getValue().get();

                if (symbol->getSymbolType() == Symbol::SymbolType::FUNC) {
                    throw sbtexcept::TypeMismatch();
                }
                auto *varSymbol = static_cast<VariableSymbol *>(symbol);    // NOLINT conversion is safe here
                static_cast<void (&)(Symbol::DataType &, Symbol &)>(compareTypeAndInferIfNeeded)(actualFunctionSymbol->getParams()[i], *varSymbol);

                totalEntry += lookupRes.numOfProbes;
            }
        }
        auto assigneeLookupRes = lookup(assignee);
        auto *assigneeSymbol = assigneeLookupRes.ptr->getValue().get();
        if (assigneeSymbol->getSymbolType() == Symbol::SymbolType::FUNC) {
            throw sbtexcept::TypeMismatch();
        }
        if (assigneeSymbol->getDataType() == Symbol::DataType::UN_INFERRED && actualFunctionSymbol->getDataType() == Symbol::DataType::VOID) {
            throw sbtexcept::TypeMismatch();
        }
        compareTypeAndInferIfNeeded(*assigneeSymbol, *actualFunctionSymbol);

        totalEntry += functionSymbolLookupRes.numOfProbes;
        totalEntry += assigneeLookupRes.numOfProbes;
    }
    }
    return totalEntry;
}

void SymbolTable::processLine(const std::string &line) {
    std::unique_ptr<pam::ParsedInstruction> parsed;
    try {
        parsed = pam::parseInstruction(line);
    } catch (pam::GenericParsingException &e) {
        throw InvalidInstruction(line);
    }

    switch (parsed->getType()) {
    case pam::InstructionType::INSERT: {
        auto *parsedInsert = static_cast<pam::ParsedINSERT *>(parsed.get());    // NOLINT
        try {
            unsigned long res = insert(parsedInsert);    // NOTE: This throw Redeclared, InvalidDeclaration or GenericOverflowException
            std::cout << res << '\n';
        } catch (sbtexcept::GenericOverflowException &e) {    // NOTE: Redeclared and InvalidDeclaration will be handle by caller
            throw Overflow(line);
        }
        return;
    }
    case pam::InstructionType::ASSIGN: {
        auto *parsedAssign = static_cast<pam::ParsedASSIGN *>(parsed.get());    // NOLINT
        try {
            unsigned long res = assign(parsedAssign);
            std::cout << res << '\n';
        } catch (sbtexcept::TypeMismatch &e) {
            throw TypeMismatch(line);
        } catch (sbtexcept::InferError &e) {
            throw TypeCannotBeInferred(line);
        }
        return;
    }
    case pam::InstructionType::BEGIN: {
        this->begin();
        return;
    }
    case pam::InstructionType::END:
        this->end();
        return;
    case pam::InstructionType::CALL: {
        auto *parsedCall = static_cast<pam::ParsedCALL *>(parsed.get());    // NOLINT
        try {
            auto res = call(parsedCall);
            std::cout << res << '\n';
        } catch (sbtexcept::TypeMismatch &e) {
            throw TypeMismatch(line);
        } catch (sbtexcept::InferError &e) {
            throw TypeCannotBeInferred(line);
        }
        return;
    }
    case pam::InstructionType::LOOKUP: {
        auto *parsedLookup = static_cast<pam::ParsedLOOKUP *>(parsed.get());    // NOLINT
        auto res = lookup(parsedLookup);                                        // NOTE: Undeclared will be handled by caller
        std::cout << res.index << '\n';
        return;
    }
    case pam::InstructionType::PRINT:
        print();
        return;
    }
    throw std::runtime_error("Cannot reach here!");
}

void SymbolTable::detectUnclosedBlock() const {
    if (currentLevel != 0) {
        throw UnclosedBlock(static_cast<int>(currentLevel));
    }
}
