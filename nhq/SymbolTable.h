#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
#define INVALID_INSTRUCTION(line)    throw InvalidInstruction(line)
#define UNDECLARED(line)    throw Undeclared(line)
#define UNKNOWN_BLOCK   throw Unknownlbock()
#define INVALID_DECLARATION(line) throw InvalidDeclaration(line)
#define TYPEMISMATCH(line) throw TypeMismatch(line)
#define OVERFLOW    throw Overflow()
#define UncloseBlock(level)    throw UncloseBlock(level)
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
    int sizeHash, constFind; //Probing
    int effective_numParams, nominal_numParams; //Insert
    static bool isNumber(const string& test);
    static bool isString(const string& test);
    static bool isId(const string& line);
    bool isCallRoutine(const string& test);
    void parseInsert(unsigned long & pos, string& line);
    void parseAssign(int& pos, string& line);
    void parseCall(unsigned long & pos, string& line);
    static void parseLookup(unsigned long & pos, string& line);
    void parseProbing(unsigned long & pos, string& line);
    Parser();
    string getInp() const{return this->input;}
    void getInstructionType();
    void setInp(string& line){this->input = line;}
};

class SymbolTable{

public:
    int currentLevel;
    void run(const string& filename);
    void Insert(Parser theParser);
    void Assign(Parser theParser);
    void Begin();
    void End();
    void Print();
    void Lookup(Parser theParser);
    void Call(Parser theParser);
};

//class HTable{
//private:
//
//public:
//
//};


#endif
