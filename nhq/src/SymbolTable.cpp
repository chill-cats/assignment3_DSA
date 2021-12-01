#include "SymbolTable.h"
//==================== Parser =================================
Parser::Parser(){
    this->input = "";
    this->command=this->name=this->probingMethod = this->returnType = this->nameFunc = "";
    this->sizeHash = this->constFind = this->constFind2 = this->effective_numParams = this->nominal_numParams = -1;
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
            this->parseProbing(firstSpace,instruction, line);
            this->probingMethod = std::move(instruction);
            return;
        }
        if(instruction == INSERT){
            this->parseInsert(firstSpace, line);
            this->command = std::move(instruction);
            return;
        }
        else if(instruction == ASSIGN){
            this->parseAssign(firstSpace, line);
            this->command = std::move(instruction);
            return;

        }
        else if(instruction == LOOKUP){
            Parser::parseLookup(firstSpace, line);
            this->command = std::move(instruction);
            return;
        }
        else if(instruction == CALL){ //Parse Call to get args
            this->parseCall(firstSpace, line);
            this->command = std::move(instruction);
            return;
        }
        else
            INVALID_INSTRUCTION(line);
    }
}
void Parser::parseProbing(unsigned long & pos, std::string& method, std::string& line) {
    /*
        Receive a setup line then parse to check valid instruction
        If valid instruction then parse to get the arguments
        Invalid Instruction when:
            1. No matched spaces (miss second or third space, redundant spaces)
            2. ID is not deinfed as a valid format (start by a-z. Including a-z A-Z 0-9 _)
            3. All parameters must have less or equal than 6 digits
    */
    
    //Parse the <m> constant in setup line
    auto secondSpace = line.find(' ', pos+1);
    if(secondSpace == NPOS)
        INVALID_INSTRUCTION(line);
    auto constM = line.substr(pos+1, secondSpace-pos-1);
    if(!Parser::isNumber(constM))
        INVALID_INSTRUCTION(line);
    if(constM.size() > 6) //Max 6 digits and not equal to ZERO
        INVALID_INSTRUCTION(line);
    //Parse the <c1> in the setup line
    if(method == LINEAR || method == DOUBLE){
        //If DOUBLE then cannot mod 0
        if(method == DOUBLE){
            if(constM == "2")
                INVALID_INSTRUCTION(line);
        }
        
        auto constC1 = line.substr(secondSpace+1);
        if(!Parser::isNumber(constC1))
            INVALID_INSTRUCTION(line);
        if(constC1.size() > 6)
            INVALID_INSTRUCTION(line);
        this->sizeHash = stoull(constM); this->constFind = stoull(constC1);
        return;
    }
    //Parse the <c2> if the setup line QUADRACTIC
    else if(method == QUADRATIC){
        auto thirdSpace = line.find(' ', secondSpace+1);
        if(thirdSpace == NPOS)
            INVALID_INSTRUCTION(line);
        auto constC1 = line.substr(secondSpace+1, thirdSpace-secondSpace-1);
        if(!Parser::isNumber(constC1))
            INVALID_INSTRUCTION(line);
        if(constC1.size() > 6)
            INVALID_INSTRUCTION(line);
        auto constC2 = line.substr(thirdSpace+1);
        
        if(!Parser::isNumber(constC2))
            INVALID_INSTRUCTION(line);
        if(constC2.size() > 6)
            INVALID_INSTRUCTION(line);
        this->sizeHash = stoull(constM); this->constFind = stoull(constC1); this->constFind2 = stoull(constC2);
        return;
    }
    //Put args into parser -> Pass to methods
}
void Parser::parseInsert(unsigned long &pos, std::string& line) {

    // cout << "Parse Insert: "; //Testing
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
        num_of_params = stoull(numParams);
    }
    this->name = std::move(identifier_name); this->nominal_numParams = num_of_params;
}
void Parser::parseLookup(unsigned long &pos, std::string &line) {

    // cout << "Parse Lookup: ";
    auto ID = line.substr(pos+1);
    if(!Parser::isId(ID)) throw  InvalidInstruction(line);
    this->name = std::move(ID);
}
void Parser::parseAssign(unsigned long &pos, string &line) {

    // cout << "Parse Assign: ";
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
    //Value is a string contain a CALL routine  
    this->name = std::move(tobeAssign);
    this->value = std::move(valueAssign);
}
void Parser::parseCall(unsigned long &pos, std::string &line) {
    // cout << "Parse Call: " ;
    std::string functionCall = line.substr(pos+1);
    this->value = functionCall;
    if(!this->isCallRoutine(functionCall))
        INVALID_INSTRUCTION(line);
}
bool Parser::isNumber(const std::string &test) {
    if(test.empty())
        return false;
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
        if((test[idx] < 'a' || test[idx] > 'z') && (test[idx] < 'A' || test[idx] > 'Z') && (test[idx] < '0' || test[idx] > '9') && ((test[idx] != ' ')))
            return false;
    }
    return true;
}
bool Parser::isId(const std::string &test) {
    if(test.empty())    
        return false;
    if(test == "number" || test == "string")    
        return false;
    if(test[0] > 'z' || test[0] < 'a')
        return false;
    auto sizeId = test.size();
    for(auto idx = 1; idx < sizeId; ++idx){
        
        if((test[idx] < 'a' || test[idx] > 'z') && (test[idx] < '0' || test[idx] > '9') && (test[idx] != '_') && (test[idx] < 'A' || test[idx] > 'Z')){
            return false;
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
    if(firstBracket == NPOS){
        return false;
    }
    auto secondBracket = test.find(')', firstBracket+1);
    if(secondBracket == NPOS){
        return false;
    }
    std::string testID = test.substr(0, firstBracket);
    std::string list_of_params = test.substr(firstBracket+1, secondBracket- firstBracket - 1);
    if(!Parser::isId(testID)){
        return false;
    } //Check whether a valid ID
    
    //Empty args 
    if(list_of_params.empty()){
        this->nameFunc = testID;
        this->effective_numParams = 0;
        return true;
    }
        
    //Parse and check the parameters list. Number of args >= 1
    auto _numComma = std::count(list_of_params.begin(), list_of_params.end(), ',');
    unsigned long markedComma = 0;
    for(auto count = 1; count <= _numComma + 1; ++count){ //Tokenize the list of parameters
        int type_idx = count;
        std::string tmpArg;
        auto commaIdx = list_of_params.find(',', markedComma);
        if(commaIdx == NPOS){
            tmpArg = list_of_params.substr(markedComma);
        }
        else{
            tmpArg = list_of_params.substr(markedComma, commaIdx - markedComma);
            markedComma = commaIdx+1;
        }
        if(!Parser::isId(tmpArg) && !Parser::isNumber(tmpArg) && !Parser::isString(tmpArg)){
            return false;
        }
        // cout << tmpArg <<"\n"; //Testing
    }
    //If valid, then get the name of identifier and the number of real
    this->nameFunc = testID;
    this->effective_numParams = _numComma + 1;
    return true;
}

//=================== SymbolTable ================================
SymbolTable::SymbolTable() {this->myTable = nullptr;}
SymbolTable::~SymbolTable(){
    auto size = this->myTable->sizeTable;
    for(int i = 0; i < size; ++i){
        if(this->myTable->tableArr[i].type_of_args != nullptr)
            delete[] this->myTable->tableArr[i].type_of_args;  
    }
    if(this->myTable)   
        delete this->myTable;
}
void SymbolTable::run(const std::string& filename)
 {
     ifstream inFile(filename, ios::in);
     std::string line;
     Parser myParser;
     if(inFile)
     {  
        
        //Setup for the hash table
        getline(inFile, line);
        myParser.setInp(line);
        myParser.getInstructionType();
        auto method = myParser.probingMethod;
        if(method == LINEAR)
            this->myTable = new LinearHTable();
        else if(method == QUADRATIC)
            this->myTable = new QuadraticHTable();
        else if(method == DOUBLE)
            this->myTable = new DoubleHTable();
        if(myParser.sizeHash > -1){
            auto size = myParser.sizeHash;
            this->myTable->sizeTable = size;
            this->myTable->tableArr = new Identifier[size];
        }
        
        while(!inFile.eof()){
            
            getline(inFile, line);
             myParser.setInp(line);
             myParser.getInstructionType();
             if(myParser.command == INSERT){
                 this->myTable->Insert(myParser);
                 this->myTable->insertedID++;
             }
            else if(myParser.command == ASSIGN){
                this->myTable->Assign(myParser);
            }
            else if(myParser.command == LOOKUP){
                int buckets=  0;
                auto found = this->myTable->Lookup(myParser.name, myParser, buckets);
                if(found > -1)
                    cout << found << "\n";
                else if(found == -1)
                    UNDECLARED(myParser.name);
            }
            else if(myParser.command == CALL)
                this->myTable->Call(myParser);
            else if(myParser.command == PRINT){
                this->myTable->Print();
            }
            else if(myParser.command == BEGIN){this->myTable->Begin();}
            else if (myParser.command == END){this->myTable->End();}
         }
         if(this->myTable->currentLevel > 0)
            throw UnclosedBlock(this->myTable->currentLevel);
     }
     inFile.close();

 }

//====================== BASE CLASS =====================================================
HTable::HTable(){this->sizeTable = this->currentLevel =  this->insertedID = 0; this->tableArr = nullptr; }
HTable::~HTable(){delete[] this->tableArr;};
short HTable::InferType(Identifier& valueAss, Identifier& tobeAss){
     //Both have type
     // Return -1: TYPEMISMATCH
     //Return -2: TYPECANNOT
    if(!valueAss.type.empty() && !tobeAss.type.empty()){
        if(valueAss.type != tobeAss.type) //Not same type
            return -1;
    }
    else{
        //Both have not been inferred type.
        if(valueAss.type.empty() && tobeAss.type.empty())
            return -2;
        if(valueAss.type.empty())
            valueAss.type = tobeAss.type;
        else 
            tobeAss.type = valueAss.type;
    }
    return 0;
}
void HTable::validInsert(Parser theParser){
    /*
    Errors can be found:
        3. Overflow
        2. Invalid Declaration: when function is inserted in level > 0
        1. Redeclaration
    */
    //Once parsed the INSERT, theParser got the nam
    //to lookup.
    if(theParser.sizeHash == 0)
        OVERFLOW(theParser.input);
    int buckets = 0;
    int found = this->Lookup(theParser.name, theParser, buckets), size = theParser.sizeHash;
    
    if(found != -1){
        if(this->tableArr[found].name == theParser.name && 
        this->tableArr[found].level == this->currentLevel && 
        this->tableArr[found].deleted == false)
            REDEC(theParser.name);
    }
    if(theParser.nominal_numParams > -1 && this->currentLevel != 0) //If insert function not at level 0
        INVALID_DECLARATION(theParser.name);
    if(this->insertedID == this->sizeTable || size == 0)
        OVERFLOW(theParser.input);    
}
void HTable::Insert(Parser theParser){
    this->validInsert(theParser);
    string line = theParser.input;
    // cout << "Size: " << theParser.sizeHash << "\n"; //Testing
    //Check Overflow
    //Reach to this line -> Not full yet -> Add
    int passedBucket = 0;
    Identifier key = Identifier(); 
    key.name = theParser.name; key.level = this->currentLevel; key.num_of_params = theParser.nominal_numParams;
    if(key.num_of_params > 0){
        auto size = key.num_of_params;
        if(!key.type_of_args)
            key.type_of_args = new string[size];
    }
    if(key.num_of_params > -1){
        key.type = "func";
    }
    long index = this->Probing(key.name, passedBucket, theParser);
    // cout << "Index: " << index << "\n"; //Testing
    this->tableArr[index] = key;
    cout << passedBucket << "\n";
}
void HTable::Assign(Parser theParser){
    auto valueAssign = theParser.value;
    auto tobeAssign = theParser.name;

    //Assign STRING/NUMBER -> ID
    if(Parser::isNumber(valueAssign) || Parser::isString(valueAssign))
    {
        int buckets = 0;
        auto idx = this->Lookup(tobeAssign, theParser, buckets); //Stop here
        if(idx == -1)
            UNDECLARED(tobeAssign);
        if(this->tableArr[idx].type.empty())
            this->tableArr[idx].type = Parser::isNumber(valueAssign)?"number":"string";
        else{
            if(Parser::isNumber(valueAssign) && this->tableArr[idx].type != "number")
                TYPEMISMATCH(theParser.input);
            if(Parser::isString(valueAssign) && this->tableArr[idx].type != "string")
                TYPEMISMATCH(theParser.input);
        }
        cout << buckets <<"\n";
        return;
    }
    //Assign ID -> ID
    if(Parser::isId(valueAssign))
    {
        int buckets = 0;
        //Finding value (ASSIGN)
        auto value_idx = this->Lookup(valueAssign, theParser, buckets);
        if(value_idx == -1)
            UNDECLARED(valueAssign);
        else{
            if(this->tableArr[value_idx].type == "func")
                TYPEMISMATCH(theParser.input);
        }
        //Finding tobeAssign identifier
        auto id_idx = this->Lookup(tobeAssign, theParser, buckets);
        if(id_idx == -1)
            UNDECLARED(tobeAssign);
        if(this->tableArr[id_idx].type == "func")
            TYPEMISMATCH(theParser.input);
        else{
             //Both have type
            if(!this->tableArr[value_idx].type.empty() && !this->tableArr[id_idx].type.empty()){
                if(this->tableArr[value_idx].type != this->tableArr[id_idx].type) //Not same type
                    TYPEMISMATCH(theParser.input);
            }
            else{
                auto flag = this->InferType(this->tableArr[value_idx], this->tableArr[id_idx]);
                if(flag == -1)
                    TYPEMISMATCH(theParser.input);
                else if(flag == -2)
                    throw TypeCannotBeInferred(theParser.input);
            }
        }
        cout << buckets <<"\n";
    }
    //Assign a CALL Rountine -> ID
    if(theParser.isCallRoutine(valueAssign)){
        // cout << "Value: " << theParser.value <<"\n"; //Testing
        int start = 0, buckets = 0, realNumArgs = theParser.effective_numParams;
        std::string funcName = theParser.nameFunc;
        // cout << "Num of args: " << realNumArgs << "\n"; //Testing
        // cout << "Funcname: " << theParser.nameFunc << "\n"; //Testing
        //Find the function ID
        int idx_func = this->Lookup(funcName, theParser, buckets);
        if(idx_func == -1) //Not found
            UNDECLARED(funcName);
        if(this->tableArr[idx_func].type != "func"){
            TYPEMISMATCH(theParser.input);
        }
        //Check match the number of args
        if(this->tableArr[idx_func].num_of_params != realNumArgs)
            TYPEMISMATCH(theParser.input);
        auto firstBracket = valueAssign.find_first_of('(', 0);
        auto secondBracket = valueAssign.find(')', firstBracket+1);
        std::string list_of_params = valueAssign.substr(firstBracket+1, secondBracket- firstBracket - 1);
        auto _numComma = std::count(list_of_params.begin(), list_of_params.end(), ',');
        unsigned long markedComma = 0;
        for(auto count = 1; count <= _numComma + 1; ++count){ //Tokenize the list of parameters
            int type_idx = count;
            std::string tmpArg;
            auto commaIdx = list_of_params.find(',', markedComma);
            if(commaIdx == NPOS){
                tmpArg = list_of_params.substr(markedComma);
            }
            else{
                tmpArg = list_of_params.substr(markedComma, commaIdx - markedComma);
                markedComma = commaIdx+1;
            }
            
            long idx_of_arg = 0;
            if(Parser::isId(tmpArg)){
                idx_of_arg = this->Lookup(tmpArg, theParser, buckets);
                if(idx_of_arg == -1)
                    UNDECLARED(tmpArg);
                if(this->tableArr[idx_of_arg].type == "func")
                    TYPEMISMATCH(theParser.input);
            }
            if(this->tableArr[idx_func].type_of_args[type_idx-1].empty()){
                if(Parser::isId(tmpArg)){
                    if(this->tableArr[idx_of_arg].type.empty())
                        throw TypeCannotBeInferred(theParser.input);
                    this->tableArr[idx_func].type_of_args[--type_idx] = this->tableArr[idx_of_arg].type;
                }
                if(Parser::isNumber(tmpArg))
                    this->tableArr[idx_func].type_of_args[--type_idx] = "number";
                if(Parser::isString(tmpArg))
                    this->tableArr[idx_func].type_of_args[--type_idx] = "string";
            }
            else{
                if(Parser::isNumber(tmpArg) && this->tableArr[idx_func].type_of_args[type_idx-1] != "number")
                    TYPEMISMATCH(theParser.input);
                if(Parser::isString(tmpArg) && this->tableArr[idx_func].type_of_args[type_idx-1] != "string")
                    TYPEMISMATCH(theParser.input);
                if(Parser::isId(tmpArg)){
                    if(this->tableArr[idx_of_arg].type.empty())
                        this->tableArr[idx_of_arg].type = this->tableArr[idx_func].type_of_args[type_idx-1];
                    if(this->tableArr[idx_of_arg].type != this->tableArr[idx_func].type_of_args[type_idx-1])
                        TYPEMISMATCH(theParser.input);
            }
            }
        }
        // for(int i = 0; i < this->tableArr[idx_func].num_of_params; ++i){
        //         cout << this->tableArr[idx_func].type_of_args[i] << "\n";
        //     } //Testing
        //Find the tobeAssgin
        int tobe_assign_idx = this->Lookup(tobeAssign, theParser, buckets);
        if(tobe_assign_idx == -1)
            UNDECLARED(tobeAssign);
        if(this->tableArr[tobe_assign_idx].type == "func")
            TYPEMISMATCH(theParser.input);
        if(this->tableArr[idx_func].returnTypee == "void")
            TYPEMISMATCH(theParser.input);
        if(this->tableArr[tobe_assign_idx].type.empty() && this->tableArr[idx_func].returnTypee.empty())
            throw TypeCannotBeInferred(theParser.input);
        if(this->tableArr[tobe_assign_idx].type != this->tableArr[idx_func].returnTypee)
            TYPEMISMATCH(theParser.input);
        if(this->tableArr[tobe_assign_idx].type.empty() && !this->tableArr[idx_func].returnTypee.empty())
            this->tableArr[tobe_assign_idx].type = this->tableArr[idx_func].returnTypee;
        if(!this->tableArr[tobe_assign_idx].type.empty() && this->tableArr[idx_func].returnTypee.empty())
            this->tableArr[idx_func].returnTypee = this->tableArr[tobe_assign_idx].type ;
    }
}
void HTable::Call(Parser theParser){
    int buckets = 0;
    const std::string callID = theParser.nameFunc;
    auto idx_func = this->Lookup(callID, theParser, buckets);
    //Check Declaration
    if(idx_func == -1)
        UNDECLARED(callID);
    //Check TYPE
    if(this->tableArr[idx_func].type != "func")
        TYPEMISMATCH(theParser.input);
    //Check param list
    auto valueAssign = theParser.value;
    auto firstBracket = valueAssign.find_first_of('(', 0);
    auto secondBracket = valueAssign.find(')', firstBracket+1);
    std::string list_of_params = valueAssign.substr(firstBracket+1, secondBracket- firstBracket - 1);
    auto _numComma = std::count(list_of_params.begin(), list_of_params.end(), ',');
    unsigned long markedComma = 0;
    for(auto count = 1; count <= _numComma + 1; ++count){ //Tokenize the list of parameters
        int type_idx = count;
        std::string tmpArg;
        auto commaIdx = list_of_params.find(',', markedComma);
        if(commaIdx == NPOS){
            tmpArg = list_of_params.substr(markedComma);
        }
        else{
            tmpArg = list_of_params.substr(markedComma, commaIdx - markedComma);
            markedComma = commaIdx+1;
        }
        long idx_of_arg = 0;
        if(Parser::isId(tmpArg)){
            idx_of_arg = this->Lookup(tmpArg, theParser, buckets);
            if(idx_of_arg == -1)
                UNDECLARED(tmpArg);
            if(this->tableArr[idx_of_arg].type == "func")
                TYPEMISMATCH(theParser.input);
            
        }
        if(this->tableArr[idx_func].type_of_args[type_idx-1].empty()){
            if(Parser::isId(tmpArg)){
                if(this->tableArr[idx_of_arg].type.empty())
                    throw TypeCannotBeInferred(theParser.input);
                this->tableArr[idx_func].type_of_args[--type_idx] = this->tableArr[idx_of_arg].type;
            }
            if(Parser::isNumber(tmpArg))
                this->tableArr[idx_func].type_of_args[--type_idx] = "number";
            if(Parser::isString(tmpArg))
                this->tableArr[idx_func].type_of_args[--type_idx] = "string";
        }
        else{
            if(Parser::isNumber(tmpArg) && this->tableArr[idx_func].type_of_args[type_idx-1] != "number")
                TYPEMISMATCH(theParser.input);
            if(Parser::isString(tmpArg) && this->tableArr[idx_func].type_of_args[type_idx-1] != "string")
                TYPEMISMATCH(theParser.input);
            if(Parser::isId(tmpArg)){
                if(this->tableArr[idx_of_arg].type.empty())
                    this->tableArr[idx_of_arg].type = this->tableArr[idx_func].type_of_args[type_idx-1];
                if(this->tableArr[idx_of_arg].type != this->tableArr[idx_func].type_of_args[type_idx-1])
                    TYPEMISMATCH(theParser.input);
            }
        }
    }
    //Check return type
    if(this->tableArr[idx_func].returnTypee.empty())
        this->tableArr[idx_func].returnTypee = "void";
    else
    {
        if(this->tableArr[idx_func].returnTypee != "void")
            TYPEMISMATCH(theParser.input);
    }
    cout << buckets << "\n";
}
void HTable::Begin() {this->currentLevel += 1;}
void HTable::End() {
    //Traverse all the elements in the array.
    //If the element having the same level as the current level then delete it (marked as deleted)
    if(this->currentLevel == 0)
        throw UnknownBlock();
    if(this->tableArr){
        auto table_size = this->sizeTable;
        auto curLev = this->currentLevel;
        for(int i = 0; i < table_size; ++i){
            // cout << "Here" << endl;
            if(!this->tableArr[i].name.empty()){
                if(this->tableArr[i].level == curLev && this->tableArr[i].deleted == false){
                    this->tableArr[i].deleted = true;
                    this->insertedID--;
                }
            }
        }
    }
    this->currentLevel -= 1;
    return;
}
void HTable::Print(){
    if(this->insertedID == 0 || !this->tableArr)
        return;
    int idx = 0; 
    for(idx = 0; idx < this->sizeTable; ++idx){
        if(this->tableArr[idx].name != "" && this->tableArr[idx].level != -1 && !this->tableArr[idx].deleted){
            break;
        }
    }
    cout << idx << " " << this->tableArr[idx].name << "//" << this->tableArr[idx].level;
    for(++idx; idx < this->sizeTable; ++idx){
        if(this->tableArr[idx].name != "" && this->tableArr[idx].level != -1 && this->tableArr[idx].deleted == false){
            cout <<";" << idx << " " << this->tableArr[idx].name << "//" << this->tableArr[idx].level;
        }
    }
    cout << "\n";
};
long HTable::key2index(const string& ID, const int& diff, int level){
    //Receive a string -> return an index in hash table
    int index = level, table_size = this->sizeTable - diff; 
    auto _size = ID.size(); //Size of string to traverse
    for(auto i = 0; i < _size; ++i){
        index %= table_size;

        auto tmp = ID[i] - 48;
        if(tmp < 10)    
            index *= 10;
        else 
            index *= 100;
        
        index %= table_size;
        tmp %= table_size;
        index += tmp;
        index %= table_size;
    }
    return index;
}

//=================================== Derived Class =============================
LinearHTable::LinearHTable(){};
LinearHTable::~LinearHTable(){};
long LinearHTable::Probing(const string& ID, int& passedBucket, Parser theParser){
    int initIndex = this->key2index(ID, 0, this->currentLevel), constFind = theParser.constFind, i = 0;
    int validIndex;
    while(true){
        if(i == this->sizeTable)
            OVERFLOW(theParser.input);
        validIndex = (initIndex + constFind * i) % this->sizeTable;
        //Detected Cycle -> Overflow
        if(this->tableArr[validIndex].name.empty() || this->tableArr[validIndex].deleted == true){
            passedBucket = i;
            break;
        }
        else
            i++;
    }
    return validIndex;
}
long LinearHTable::Lookup(const string& name, Parser theParser, int& numBuckets){
    auto size = this->sizeTable;
    auto Const = theParser.constFind;
    for(int i = this->currentLevel; i >= 0; --i){
        long validIndex = this->key2index(name,0, i), count = 0; //h(k)
        while(true){
            if(this->tableArr[(validIndex + Const*count) % size].name == name && 
                this->tableArr[(validIndex + Const*count) % size].level == i &&
                this->tableArr[(validIndex + Const*count) % size].deleted == false){
                numBuckets += count;
                return (validIndex + Const*count) % size; // If same name and not be deleted -> found - Check after break
            }   
            if(this->tableArr[(validIndex + Const*count) % size].name != name || 
            this->tableArr[(validIndex + Const*count) % size].level != i || 
            this->tableArr[(validIndex + Const*count) % size].deleted == true)
                count++;
            if(this->tableArr[(validIndex + Const*count) % size].name.empty())
                break; //Not in a certain level -> Not found
            if(count == this->sizeTable) 
                break; //Not in a certain level -> Not found
        }
    }
    return -1; //Traversed all scopes but cannot be found
}

DoubleHTable::DoubleHTable(){};
DoubleHTable::~DoubleHTable(){};
long  DoubleHTable::Probing(const string& ID, int& passedBucket, Parser theParser){
    long initIndex = this->key2index(ID, 0, this->currentLevel), secondIndex = this->key2index(ID, 2, this->currentLevel) + 1, i = 0, constFind = theParser.constFind;
    long validIndex;
    while (true)
    {
        if(i == this->sizeTable)
            OVERFLOW(theParser.input);
        validIndex = (initIndex + constFind*i*secondIndex) % this->sizeTable;
        if(this->tableArr[validIndex].name.empty() || this->tableArr[validIndex].deleted == true){
            passedBucket = i;
            break;
        }
        else   
            i++;
    }
    return validIndex;
}
long DoubleHTable::Lookup(const string& name, Parser theParser, int& numBuckets){
    auto size = this->sizeTable;
    auto Const = theParser.constFind;
    for(int i = this->currentLevel; i >= 0; --i){
        long validIndex = this->key2index(name,0, i), validIndex2 = 1 + this->key2index(name, 2,i) , count = 0; //h(k)
        while(true)
        {
            if(this->tableArr[(validIndex + validIndex2*Const*count) % size].name == name&& 
                this->tableArr[(validIndex + validIndex2*Const*count) % size].level == i && 
                this->tableArr[(validIndex + validIndex2*Const*count) % size].deleted == false){
                numBuckets += count;
                return (validIndex + validIndex2*Const*count) % size; // If same name and not be deleted -> found
            }
            if(this->tableArr[(validIndex + validIndex2*Const*count) % size].name != name || 
            this->tableArr[(validIndex + validIndex2*Const*count) % size].level != i || 
            this->tableArr[(validIndex + validIndex2*Const*count) % size].deleted == true)
                count++;    
            if(this->tableArr[(validIndex + validIndex2*Const*count) % size].name.empty())
                break;
            if(count == this->sizeTable) 
                break;
        }    
    }
    return -1; //Traversed all scopes but cannot be found
}

QuadraticHTable::QuadraticHTable(){};
QuadraticHTable::~QuadraticHTable(){};
long QuadraticHTable::Probing(const string& ID, int& passedBucket, Parser theParser){
    int initIndex = this->key2index(ID, 0, this->currentLevel), constFind1 = theParser.constFind, constFind2 = theParser.constFind2, i = 0;
    long validIndex = 0;
    while(true){
        if(i == this->sizeTable)
            OVERFLOW(theParser.input);
        validIndex = (initIndex + constFind1 * i + constFind2 * i * i) % this->sizeTable;
        if(this->tableArr[validIndex].name.empty() || this->tableArr[validIndex].deleted == true){
            passedBucket = i;
            break;
        }
        else i++;
    }
    return validIndex;
}
long QuadraticHTable::Lookup(const string& name, Parser theParser, int& numBuckets){
    auto size = this->sizeTable;
    auto Const = theParser.constFind;
    auto Const2 = theParser.constFind2;
    for(int i = this->currentLevel; i >= 0; --i){
        long validIndex = this->key2index(name,0, i), count = 0; //h(k)
        while(true)
        {
            if(this->tableArr[(validIndex + Const*count + Const2*count*count) % size].name == name &&
                 this->tableArr[(validIndex + Const*count + Const2*count*count) % size].level == i &&
                this->tableArr[(validIndex + Const*count + Const2*count*count) % size].deleted ==false){
                numBuckets += count;
                return (validIndex + Const*count + Const2*count*count) % size; // If same name and not be deleted -> found
            }
            if(this->tableArr[(validIndex + Const*count + Const2*count*count) % size].name != name || 
               this->tableArr[(validIndex + Const*count + Const2*count*count) % size].level != i ||
               this->tableArr[(validIndex + Const*count + Const2*count*count) % size].deleted == true)
                count++;
            if(this->tableArr[(validIndex + Const*count + Const2*count*count) % size].name.empty())
                break;
            if(count == this->sizeTable) 
                break;
        }
    }
    return -1; //Traversed all scopes but cannot be found
}
Identifier::Identifier() {
    this->name = this->type =  this->returnTypee = "";
    this->level = this->num_of_params = -1;
    this->deleted = false;
    this->type_of_args = nullptr;
}