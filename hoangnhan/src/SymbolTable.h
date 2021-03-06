#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

template<typename T>
class FixedSizeVec {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    std::unique_ptr<value_type[]> m_data;
    size_type m_size = 0;

public:
    FixedSizeVec();
    explicit FixedSizeVec(size_type size);
    FixedSizeVec(const FixedSizeVec &other);
    FixedSizeVec(FixedSizeVec &&other) noexcept;
    template<typename Iter>
    FixedSizeVec(Iter begin, Iter end);
    ~FixedSizeVec() = default;

    FixedSizeVec &operator=(const FixedSizeVec &other);
    FixedSizeVec &operator=(FixedSizeVec &&other) noexcept;

    reference operator[](size_type index);
    const_reference operator[](size_type index) const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    size_type size() const noexcept;
    bool empty() const noexcept;
};
template<typename T>
FixedSizeVec<T>::FixedSizeVec() : m_data(std::make_unique<value_type[]>(0)) {}    // NOLINT

template<typename T>
FixedSizeVec<T>::FixedSizeVec(size_type size) : m_data(std::make_unique<value_type[]>(size)), m_size(size) {}    // NOLINT

template<typename T>
FixedSizeVec<T>::FixedSizeVec(const FixedSizeVec &other) : m_data(std::make_unique<value_type[]>(other.m_size)), m_size(other.m_size) {    // NOLINT
    for (size_type i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

template<typename T>
FixedSizeVec<T>::FixedSizeVec(FixedSizeVec &&other) noexcept : m_data(std::move(other.m_data)), m_size(other.m_size) {    // NOLINT
    other.m_size = 0;
}

template<typename T>
template<typename Iter>
FixedSizeVec<T>::FixedSizeVec(Iter begin, Iter end) : m_data(std::make_unique<value_type[]>(end - begin)), m_size(end - begin) {    // NOLINT
    size_type index = 0;
    for (const auto &it = begin; it != end; ++it) {
        m_data[index++] = it;
    }
}

template<typename T>
FixedSizeVec<T> &FixedSizeVec<T>::operator=(const FixedSizeVec &other) {
    if (&other == this) {
        return *this;
    }
    m_data = std::make_unique<value_type[]>(other.m_size);    // NOLINT
    m_size = other.m_size;
    for (size_type i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    return *this;
}

template<typename T>
FixedSizeVec<T> &FixedSizeVec<T>::operator=(FixedSizeVec &&other) noexcept {
    m_data = std::move(other.m_data);
    m_size = other.m_size;
    other.m_size = 0;
    return *this;
}

template<typename T>
typename FixedSizeVec<T>::reference FixedSizeVec<T>::operator[](size_type index) {
    return m_data[index];
}

template<typename T>
typename FixedSizeVec<T>::const_reference FixedSizeVec<T>::operator[](size_type index) const {
    return m_data[index];
}

template<typename T>
typename FixedSizeVec<T>::iterator FixedSizeVec<T>::begin() noexcept {
    return this->m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::begin() const noexcept {
    return this->m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::cbegin() const noexcept {
    return this->m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::iterator FixedSizeVec<T>::end() noexcept {
    return this->m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::end() const noexcept {
    return this->m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::cend() const noexcept {
    return this->m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::reverse_iterator FixedSizeVec<T>::rbegin() noexcept {
    return std::reverse_iterator<iterator>(this->m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::rbegin() const noexcept {
    return std::reverse_iterator<const_iterator>(this->m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::crbegin() const noexcept {
    return std::reverse_iterator<const_iterator>(this->m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::reverse_iterator FixedSizeVec<T>::rend() noexcept {
    return std::reverse_iterator<const_iterator>(this->m_data.get());
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::rend() const noexcept {
    return std::reverse_iterator<const_iterator>(this->m_data.get());
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::crend() const noexcept {
    return std::reverse_iterator<const_iterator>(this->m_data.get());
}
template<typename T>
typename FixedSizeVec<T>::size_type FixedSizeVec<T>::size() const noexcept {
    return m_size;
}

template<typename T>
bool FixedSizeVec<T>::empty() const noexcept {
    return size() == 0;
}

namespace pam {    // parse and match
using StrCIter = std::string::const_iterator;

enum class InstructionType {
    INSERT,
    ASSIGN,
    CALL,
    BEGIN,
    END,
    LOOKUP,
    PRINT,
};

enum class AssignType {
    LITERAL_NUMBER,
    LITERAL_STRING,
    IDENTIFIER,
    FUNC_CALL,
};

class ParsedInstruction {
    InstructionType m_type;

public:
    explicit ParsedInstruction(InstructionType type);
    InstructionType getType() const noexcept;

    virtual ~ParsedInstruction() = default;
};

class ParsedINSERT : public ParsedInstruction {
    const std::string m_name;
    const bool m_isFunc = false;
    const unsigned long m_funcParamCount = 0;

public:
    ParsedINSERT(std::string name, bool isFunc, unsigned long funcParamCount);

    const std::string &getName() const noexcept;
    bool isFunc() const noexcept;
    unsigned long getParamCount() const noexcept;
};

class ParsedASSIGN : public ParsedInstruction {
    const std::string m_name;
    const AssignType m_valueType;
    const std::string m_valueName;
    const FixedSizeVec<std::string> m_params;

public:
    ParsedASSIGN(std::string name, AssignType valueType, std::string valueName, FixedSizeVec<std::string> params);
    const std::string &getName() const noexcept;
    AssignType getValueType() const noexcept;
    const std::string &getValueName() const noexcept;
    const FixedSizeVec<std::string> &getParams() const noexcept;
};

class ParsedCALL : public ParsedInstruction {
    const std::string m_functionName;
    FixedSizeVec<std::string> m_params;

public:
    ParsedCALL(std::string functionName, FixedSizeVec<std::string> params);
    const std::string &getFunctionName() const noexcept;
    const FixedSizeVec<std::string> &getParams() const noexcept;
};

class ParsedLOOKUP : public ParsedInstruction {
    const std::string m_nameToLookup;

public:
    explicit ParsedLOOKUP(std::string nameToLookup);
    const std::string &getNameToLookup() const noexcept;
};

enum class ProbingMethod {
    LINEAR,
    QUADRARTIC,
    DOUBLE
};

/**
 * @brief This represent the parsed line with params
 */
struct ParsedSetupLine {
    ProbingMethod method;
    FixedSizeVec<unsigned long> params;
};

struct ParsedFunctionCall {
    std::string functionName;
    FixedSizeVec<std::string> params;
};

class GenericParsingException : std::exception {};

/*
unsigned long strtoul(StrCIter begin, StrCIter end);

std::unique_ptr<ParsedInstruction> parseAssign(StrCIter begin, StrCIter end);
std::unique_ptr<ParsedInstruction> parseCall(StrCIter begin, StrCIter end);
std::unique_ptr<ParsedInstruction> parseInsert(StrCIter begin, StrCIter end);
std::unique_ptr<ParsedInstruction> parseInstruction(const std::string &line);

FixedSizeVec<unsigned long> parseParamsLINEARorDOUBLE(StrCIter begin, StrCIter end);

FixedSizeVec<unsigned long> parseParamsQUADRATIC(StrCIter begin, StrCIter end);

ParsedSetupLine parseSetupLine(const std::string &line);
*/
}    // namespace pam
class Symbol {
public:
    enum class SymbolType {
        VAR,
        FUNC,
    };

    enum class DataType {
        UN_INFERRED,
        NUMBER,
        STRING,
        VOID
    };

private:
    const SymbolType m_symbolType;
    const std::string m_name;
    const unsigned long m_level = 0;
    // NOTE: This is data type of Variable symbol and Function symbol's return type
    DataType m_dataType = DataType::UN_INFERRED;    // data type is always uninferred when insert

public:
    Symbol(std::string &&name, unsigned long level, SymbolType symbolType);    // PERF: Allow construction of generic symbol use in LOOKUP

    const std::string &getName() const noexcept;
    unsigned long getLevel() const noexcept;
    DataType getDataType() const noexcept;
    SymbolType getSymbolType() const noexcept;
    void changeDataType(DataType type) noexcept;

    virtual ~Symbol() = default;
};

class FunctionSymbol : public Symbol {
    FixedSizeVec<DataType> m_paramsType;

public:
    FunctionSymbol(std::string name, unsigned long level, unsigned long paramsNum);

    decltype(m_paramsType) &getParams() noexcept;
};

class VariableSymbol : public Symbol {
public:
    VariableSymbol(std::string name, unsigned long level);
};

namespace sbtexcept {
class GenericOverflowException : std::exception {};
class InvalidDeclaration : std::exception {};
class UnknownBlock : std::exception {};
class TypeMismatch : std::exception {};
class InferError : std::exception {};
class InvalidInstruction : std::exception {};
}    // namespace sbtexcept

class SymbolTable {
    class HashEntry {
        std::unique_ptr<Symbol> value = nullptr;
        bool isTombstone = false;

    public:
        explicit HashEntry(std::unique_ptr<Symbol> &&value);
        HashEntry() = default;
        const std::unique_ptr<Symbol> &getValue() const noexcept;
        void changeValue(std::unique_ptr<Symbol> &&newVal);
        bool isTombStone() const noexcept;
        void changeToTombStone() noexcept;
        void unChangeToTombStone() noexcept;
    };

    unsigned long hashFunc(unsigned long level, const std::string &name);

    std::function<unsigned long(unsigned long level, const std::string &name)> doubleHashFunc;
    std::function<unsigned long(unsigned long iter, unsigned long firstHash, unsigned long secondHash)> getIndex;

    FixedSizeVec<HashEntry> container;

    unsigned long currentLevel = 0;
    
    void initLinearProbing(unsigned long coefficient);
    void initDoubleProbing(unsigned long coefficient);
    void initQuadraticProbing(unsigned long firstOrderCoefficient, unsigned long secondOrderCoefficient);

    void initHashTable(const std::string &initLine);
    void processLine(const std::string &line);

    std::unique_ptr<Symbol> constructNewSymbol(const std::string &name, bool isFunc, unsigned long paramNum);
    unsigned long findInsertPosition(const std::unique_ptr<Symbol> &symbolToInsert, unsigned long &probingNum);
    unsigned long insert(const pam::ParsedINSERT *parsed);

    unsigned long processFunctionCallParams(const FixedSizeVec<std::string> &params, FixedSizeVec<Symbol::DataType> &functionParamType);

    unsigned long call(const pam::ParsedCALL *parsed);

    unsigned long assignWithVarWithType(const std::string &name, Symbol::DataType targetType);
    unsigned long assign(const pam::ParsedASSIGN *parsed);

    void print();
    void detectUnclosedBlock() const;
    void begin() noexcept;
    void end();

    struct LookupResult {
        unsigned long index = 0;
        HashEntry *ptr = nullptr;
        unsigned long numOfProbes = 0;
    };
    LookupResult lookup(pam::ParsedLOOKUP *parsed);
    LookupResult lookup(const std::string &name);

    static void compareTypeAndInferIfNeeded(Symbol::DataType targetType, Symbol::DataType &unknownType);
    static void compareTypeAndInferIfNeeded(Symbol::DataType targetType, Symbol &unknownSymbol);
    static void compareTypeAndInferIfNeeded(Symbol &unknownSymbol1, Symbol &unknownSymbol2);
    static void compareTypeAndInferIfNeeded(Symbol::DataType &unknownType, Symbol &unknownSymbol);

    enum class ParamType {
        STRING,
        NUMBER,
        IDENTIFIER,
    };

    static ParamType fastParamTypeDeduce(const std::string &param);

    Symbol *lookupSymbolWithSymbolType(const std::string &nameOfSymbol, Symbol::SymbolType expectedSymbolType);

public:
    void run(const string &filename);
};
#endif
