#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
#define INVALID_INSTRUCTION(line)    throw InvalidInstruction(line)
#define UNDECLARED(line)    throw Undeclared(line)
#define UNKNOWN_BLOCK   throw Unknownlbock()
#define INVALID_DECLARATION(name) throw InvalidDeclaration(name)
#define TYPEMISMATCH(line) throw TypeMismatch(line)
#define OVERFLOW(line)    throw Overflow(line)
#define UNCLOSEDBLOCK(level)    throw UncloseBlock(level)
#define REDEC(line) throw Redeclared(line)
#define NUMBER "number"
#define STRING "string"
#define BEGIN "BEGIN"
#define END "END"
#define PRINT "PRINT"
#define INSERT "INSERT"
#define ASSIGN "ASSIGN"
#define LOOKUP "LOOKUP"
#define CALL "CALL"
#define LINEAR "LINEAR"
#define QUADRATIC "QUADRATIC"
#define  DOUBLE "DOUBLE"
#define NPOS std::string::npos
class ID{

};
class Parser{
public:
    
    string input;
    string command, name, probingMethod; //Instruction
    string value, nameFunc, returnType; //Assign
    int sizeHash, constFind, constFind2; //Probing
    int effective_numParams, nominal_numParams; //Insert
    static bool isNumber(const string& test);
    static bool isString(const string& test);
    static bool isId(const string& line);
    bool isCallRoutine(const string& test);
    void parseInsert(unsigned long & pos, string& line);
    void parseAssign(unsigned long & pos, string& line );
    void parseCall(unsigned long & pos, string& line);
    void parseLookup(unsigned long & pos, string& line);
    void parseProbing(unsigned long & pos,string &method, string& line);
    Parser();
    void getInstructionType();
    void setInp(const string& line){this->input = line;}
};

class Identifier{
public:
    string name, type, returnTypee;
    string *type_of_args = nullptr ;
    int level, num_of_params;
    bool deleted;
    Identifier();
};
class HTable{
public:
    int sizeTable;
    int insertedID;
    int currentLevel;
    Identifier *tableArr = nullptr;
    virtual long key2index(const string& ID, const int& diff, int level);
    virtual void validInsert(Parser);
    virtual void Begin();
    virtual void End();
    virtual short InferType(Identifier&, Identifier&);
    virtual void Print();
    virtual void Insert(Parser);
    virtual void Assign(Parser);
    virtual void Call(Parser);
    virtual long Lookup(const string&, Parser, int&){return -100;}
    virtual long Probing(const string&, int&, Parser){return 0;}
    HTable();
    ~HTable();

};
class SymbolTable{
public:
    void run(const string& filename);
    HTable *myTable = nullptr;
    //void run(const string& filename);
    SymbolTable();
    ~SymbolTable();
};

class LinearHTable: public HTable{
public:
     LinearHTable();
     ~LinearHTable();
     long Probing(const string&, int&, Parser);
     long Lookup(const string&, Parser, int&);
};

class DoubleHTable: public HTable{
public:
    DoubleHTable();
    ~DoubleHTable();
    long  Probing(const string&, int&, Parser);
    long Lookup(const string&, Parser,int&);
};

class QuadraticHTable: public HTable{
public:
    QuadraticHTable();
    ~QuadraticHTable();
    long Probing(const string&, int&, Parser);
    long Lookup(const string&, Parser,int&);
};


#endif
