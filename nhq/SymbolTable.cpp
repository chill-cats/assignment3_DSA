#include "SymbolTable.h"

//==================== Parser =================================
Parser::Parser(){
    this->input = "";
    this->command=this->name=this->probingMethod = "";
    this->sizeHash = this->constFind = this->num_of_params = -1;
}
void Parser::getInstructionType()
{
    string line = this->input;
    if(line.empty())
        INVALID_INSTRUCTION(line); //Empty line
    auto firstSpace = line.find_first_of(' ');
    if(firstSpace == std::string::npos){ //Not found space -> maybe 3 instructions no spaces
        if(line == BEGIN || line == PRINT || line == END)
           this->command = line;
        else
            INVALID_INSTRUCTION(line);
    }
    else{ //Found first space -> verify
        auto instruction = line.substr(0, firstSpace);
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
            return;
        }
        else if(instruction == LOOKUP){
            Parser::parseLookup(firstSpace, line);
            return;
        }
        else if(instruction == CALL){
            return;
        }
        else
            INVALID_INSTRUCTION(line);
    }
}
void Parser::parseProbing(unsigned long & pos, string& line) {
    auto secondSpace = line.find(' ', pos+1);
    if(secondSpace == std::string::npos)
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
void Parser::parseInsert(unsigned long &pos, string& line) {
    string identifier_name;
    int secondSpace = line.find(' ', pos+1), num_of_params = -1;
    if(secondSpace == std::string::npos){
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
    this->name = identifier_name; this->num_of_params = num_of_params;
}
void Parser::parseLookup(unsigned long &pos, string &line) {
    auto ID = line.substr(pos+1);
    if(!Parser::isId(ID)) throw  InvalidInstruction(line);
}
bool Parser::isNumber(const string &test) {
    auto size_test = test.size();
    for(auto i = 0; i < size_test; ++i){
        if(test[i] > '9' || test[i] < '0')
            return false;
    }
    return true;
}
bool Parser::isString(const string& test) {
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
bool Parser::isId(const string &test) {
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

//=================== SymbolTable ================================
void SymbolTable::run(const string& filename)
{
    ifstream inFile(filename, ios::in);
    string line;
    Parser myParser;
    if(inFile)
    {
        while(getline(inFile, line)){
            myParser.setInp(line);
            myParser.getInstructionType();
        //    if(myParser.command == INSERT){

        //    }
        //    else if(myParser.command == ASSIGN){

        //    }
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
    return ;
}
