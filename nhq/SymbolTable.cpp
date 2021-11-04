#include "SymbolTable.h"

//==================== Parser =================================
Parser::Parser(){
    this->input = "";
    this->command=this->name=this->probingMethod = "";
    this->sizeHash = this->constFind = this->effective_numParams = -1;
}
void Parser::getInstructionType()
{
    std::string line = this->input; //Receive a line to process
    if(line.empty())
        INVALID_INSTRUCTION(line); //Empty line
    auto firstSpace = line.find_first_of(' '); //First space to check
    if(firstSpace == NPOS){ //Not found space -> maybe 3 instructions no spaces
        if(line == BEGIN || line == PRINT || line == END)
           this->command = line;
        else
            INVALID_INSTRUCTION(line);
    }
    else{ //Found first space -> verify
        auto instruction = line.substr(0, firstSpace); //Get the Instruction type
        //Then parse the args
        if(instruction == LINEAR || instruction == DOUBLE || instruction == QUADRATIC){
            this->probingMethod = instruction;
            this->parseProbing(firstSpace, line);
            return;
        }
        if(instruction == INSERT){
            this->parseInsert(firstSpace, line);
            return;
        }
        else if(instruction == ASSIGN){
            this->parseAssign(firstSpace, line);
            return;

        }
        else if(instruction == LOOKUP){
            Parser::parseLookup(firstSpace, line);
            return;
        }
        else if(instruction == CALL){ //Parse Call to get args
            this->parseCall(firstSpace, line);
            return;
        }
        else
            INVALID_INSTRUCTION(line);
    }
}
void Parser::parseProbing(unsigned long & pos, std::string& line) {
    auto secondSpace = line.find(' ', pos+1);
    if(secondSpace == NPOS)
        INVALID_INSTRUCTION(line);

    auto constM = line.substr(pos+1, secondSpace-pos-1);
    if(!Parser::isNumber(constM))
        INVALID_INSTRUCTION(line);

    auto constC = line.substr(secondSpace+1);
    if(!this->isNumber(constC)) 
        INVALID_INSTRUCTION(line);
    //Put args into parser -> Pass to methods
    this->sizeHash = stoi(constM); this->constFind = stoi(constC);
}
void Parser::parseInsert(unsigned long &pos, std::string& line) {
    std::string identifier_name;
    int secondSpace = line.find(' ', pos+1), num_of_params = -1;
    if(secondSpace == NPOS){
        identifier_name = line.substr(pos+1);
        if(!Parser::isId(identifier_name))
            INVALID_INSTRUCTION(line);
    }
    else{
        identifier_name = line.substr(pos+1, secondSpace-pos-1);
        if(!Parser::isId(identifier_name))
            INVALID_INSTRUCTION(line);
        auto numParams = line.substr(secondSpace+1);
        if(!this->isNumber(numParams))
            INVALID_INSTRUCTION(line);
        num_of_params = stoi(numParams);
    }
    this->name = identifier_name; this->effective_numParams = num_of_params;
}
void Parser::parseLookup(unsigned long &pos, std::string &line) {
    auto ID = line.substr(pos+1);
    if(!Parser::isId(ID)) throw  InvalidInstruction(line);
}
void Parser::parseAssign(unsigned long &pos, string &line) {
    auto secondSpace = line.find(' ', pos+1);
    if(secondSpace == NPOS)
        INVALID_INSTRUCTION(line);
    string tobeAssign = line.substr(pos+1, secondSpace-pos-1);
    if(!Parser::isId(tobeAssign))
        INVALID_INSTRUCTION(line);
    string valueAssign = line.substr(secondSpace+1);
    if(!Parser::isId(valueAssign) && !Parser::isNumber(valueAssign) && !Parser::isString(valueAssign) && !Parser::isCallRoutine(valueAssign))
        INVALID_INSTRUCTION(line);
    //If valid instruction then get args
    this->name = tobeAssign;
    this->value = valueAssign;
}
void Parser::parseCall(unsigned long &pos, std::string &line) {
    std::string callID;
    std::string functionCall = line.substr(pos+1);
    if(!this->isCallRoutine(functionCall))
        INVALID_INSTRUCTION(line);

}
bool Parser::isNumber(const std::string &test) {
    auto size_test = test.size();
    for(auto i = 0; i < size_test; ++i){
        if(test[i] > '9' || test[i] < '0')
            return false;
    }
    return true;
}
bool Parser::isString(const std::string& test) {
    auto size_test = test.size();
    if(test[0] != '\'' || test[size_test-1] != '\'')
        return false;
    for(auto idx = 1; idx < size_test-1; ++idx){
        if(test[idx] < 'a' || test[idx] > 'z'){
            if(test[idx] < 'A' || test[idx] > 'Z'){
                if(test[idx] < '0' || test[idx] > '9'){
                    if(test[idx] != ' '){
                       return false;
                    }
                }
            }
        }
    }
    return true;
}
bool Parser::isId(const std::string &test) {
    if(test[0] > 'z' || test[0] < 'a')
        return false;
    auto sizeId = test.size();
    for(auto idx = 1; idx < sizeId; ++idx){
        
        if(test[idx] < 'a' || test[idx] > 'z'){
            if(test[idx] < '0' || test[idx] > '9'){
                if(test[idx] != '_')
                    return false;
            }
        }
    }
    return true;
}
bool Parser::isCallRoutine(const std::string &test) {
    /*
     Receive a std::string with the format as <ID_name>(<list params>)
     Ex: foo(abc,123)
     Need to check whether a valid function call or not:
        1. Check Identifier name
        2. Check the list params
     */
    auto firstBracket = test.find_first_of('(', 0);
    if(firstBracket == NPOS)
        return false;
    auto secondBracket = test.find(')', firstBracket+1);
    if(secondBracket == NPOS)
        return false;

    std::string testID = test.substr(0, firstBracket);
    std::string list_of_params = test.substr(firstBracket+1, secondBracket- firstBracket - 1);
    if(!Parser::isId(testID)) //Check whether a valid ID
        return false;
    if(list_of_params.empty())
        return true;
    //Parse and check the parameters list
    auto _numComma = std::count(list_of_params.begin(), list_of_params.end(), ',');
    unsigned long markedComma = 0;
    for(auto count = 1; count <= _numComma + 1; ++count){ //Tokenize the list of parameters
        std::string tmpArg;
        auto commaIdx = list_of_params.find(',', markedComma);
        if(commaIdx == NPOS){
            tmpArg = list_of_params.substr(markedComma);
        }
        else{
            tmpArg = list_of_params.substr(markedComma, commaIdx - markedComma);
            markedComma = commaIdx+1;
        }
        if(!Parser::isId(tmpArg) && !Parser::isNumber(tmpArg) && !Parser::isString(tmpArg))
            return false;
    }
    //If valid, then get the name of identifier and the number of real
    return true;
}
//=================== SymbolTable ================================
void SymbolTable::run(const std::string& filename)
{
    ifstream inFile(filename, ios::in);
    std::string line;
    Parser myParser;
    if(inFile)
    {
        while(getline(inFile, line)){
            myParser.setInp(line);
            myParser.getInstructionType();
        //    if(myParser.command == INSERT){

        //    }
        //    else if(myParser.command == ASSIGN){

            //}
        //    else if(myParser.command == LOOKUP){

        //    }
        //    else if(myParser.command == PRINT){

        //    }
        //    else if(myParser.command == BEGIN){

        //    }
        //    else if (myParser.command == END){

        //    }

        }
    }
    inFile.close();
}

void SymbolTable::Insert(Parser theParser) {

}

void SymbolTable::Assign(Parser theParser) {

}

void SymbolTable::Begin() {

}

void SymbolTable::End() {

}

void SymbolTable::Print() {

}

void SymbolTable::Lookup(Parser theParser) {

}

void SymbolTable::Call(Parser theParser) {

}
