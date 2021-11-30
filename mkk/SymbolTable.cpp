#include "SymbolTable.h"

#include <utility>

void SymbolTable::run(const string& filename) {
    HashTable   table;
    LinkedLisst lisst;
    int         level = 0;
    int         i = 0;

    regex linear_double(R"(^(LINEAR|DOUBLE)[ ](\d+)[ ](\d+)$)");
    regex quadratic(R"(^(QUADRATIC)[ ](\d+)[ ](\d+)[ ](\d+)$)");
    regex insert_ID(R"(^INSERT ([a-z]\w*)$)");
    regex insert_function(R"(^INSERT ([a-z]\w*)[ ](\d+)$)");
    regex assign_table(R"(^ASSIGN ([a-z]\w*) (([a-z]\w*)\((('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*)*\)|'[a-zA-Z\d ]*'|\d+|[a-z]\w*)$)");
    regex call_table(R"(^CALL (([a-z]\w*)\((('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*)*\))$)");
    regex lookup_table(R"(^LOOKUP[ ]([a-z]\w*))");

    ifstream myfile;
    myfile.open(filename);
    if (myfile.is_open()) {
        string line;
        while (getline(myfile, line)) {
            smatch data;
            if (i == 0) {
                if (regex_match(line, data, linear_double)) {
                    int *c;
                    c    = new int[1];
                    c[0] = stoi(data[3]);
                    table.set_table(data[1], stoi(data[2]),  c);
                    i++;
                }
                else if (regex_match(line, data, quadratic)) {
                    int *c;
                    c    = new int[2];
                    c[0] = stoi(data[3]);
                    c[1] = stoi(data[4]);
                    table.set_table(data[1], stoi(data[2]),  c);
                    i++;
                }
                else      throw InvalidInstruction(line);

            } else {
                if (regex_match(line, data, insert_ID)) {
                    string ID;
                    int num = -1;
                    string out = table.insert(data[1], level, num, ID);
                    if (out == "invalid")               throw InvalidInstruction(line);
                    if (out == "overflown")             throw Overflow(line);
                    if (out == "invaliddeclaration")    throw InvalidDeclaration(data[1]);
                    if (out == "redeclared")            throw Redeclared(ID);
                    cout << out << endl;
                    lisst.insert_lisst(ID, level);
                } else if (regex_match(line, data, insert_function)) {
                    string ID;
                    int num = stoi(data[2]);
                    string out = table.insert(data[1], level, num, ID);
                    if (out == "overflown")             throw Overflow(line);
                    if (out == "invaliddeclaration")    throw InvalidDeclaration(data[1]);
                    if (out == "redeclared")            throw Redeclared(ID);
                    cout << out << endl;
                    lisst.insert_lisst(ID, level);
                } else if (regex_match(line, data, assign_table)) {
                    string ID;
                    string out = table.assign(data[1], data[2], ID, level);
                    if (out == "undeclared")            throw Undeclared(ID);
                    if (out == "mismatch")              throw TypeMismatch(line);
                    if (out == "cannot")                throw TypeCannotBeInfered(line);
                    cout << out << endl;
                } else if (regex_match(line, data, call_table)) {
                    string ID;
                    string out = table.call(data[2], data[3], level, ID);
                    if (out == "undeclared")            throw Undeclared(ID);
                    if (out == "mismatch")              throw TypeMismatch(line);
                    cout << out << endl;
                } else if (line == "BEGIN") {
                    level++;
                } else if (line == "END") {
                    if (level == 0) throw UnknownBlock();
                    table.end(lisst, level);
                    lisst.delete_level(level);
                    level--;
                } else if (regex_match(line, data, lookup_table)) {
                    string out = table.lookup(data[1], level, 0, -1);
                    if (out == "no") throw Undeclared(data[1]);
                    cout << out << endl;
                } else if (line == "PRINT") {
                    table.print();
                } else {
                    throw InvalidInstruction(line);
                }
            }
        }
        if (level > 0) throw UnclosedBlock(level);
    }
    myfile.close();
}

void Data::set_data(string &ID_name, int ID_parameter, int ID_level) {
    this->ID    = ID_name;
    this->level = ID_level;
    this->num_of_parameters  = ID_parameter;
    if (this->num_of_parameters != -1) {
        this->type = "function";
    }
}

string getpass(string &ID, int level) {
    string pass_tmp = to_string(level);
    for (int j = 0; j < (int) ID.size(); j++) {
        int tmp = (int) (unsigned char) ID[j];
        pass_tmp.append(to_string(tmp - 48));
    }
    return pass_tmp;
}

int HashTable::linear(string ID, int level, int i) {
    string      pass = getpass(ID, level);
    long long   k    = stoll(pass);

    return static_cast<int> ((k % size + c[0] * i) % size);
}

int HashTable::quadratic(string &ID, int level, int i) {
    string      pass = getpass(ID, level);
    long long   k    = stoll(pass);

    return static_cast<int> ((k % size + c[0] * i + c[1] * i * i) % size);
}

int HashTable::double_hash(string &ID, int level, int i) {
    string      pass = getpass(ID, level);
    long long   k    = stoll(pass);
    int         tmp1 = static_cast<int> (k % size);
    int         tmp2 = static_cast<int> (1 + k % (size - 2));

    return (tmp1 + c[0] * i * tmp2) % size;
}

void HashTable::set_table(string gettype, int getsize, int *getc) {
    this->size    = getsize;
    this->node    = new Data[size];
    this->empty   = new bool[size];
    this->deleted = new bool[size];
    this->type    = move(gettype);
    this->c       = getc;
    for (int i = 0; i < this->size; i++) {
        empty[i]   = true;
        deleted[i] = false;
    }
}

string HashTable::insert(string ID, int level, int num, string &tmp) {
    if (ID == "number" || ID == "string") {
        return "invalid";
    }

    if (this->size == this->count) {
        return "overflown";
    }

    if (level != 0 && num != -1) {
        return "invaliddeclaration";
    }

    Data new_data;
    new_data.set_data(ID, num, level);

    for (int i = 0; i < this->size - 1; i++) {
        int index = 0;
        if (this->type == "LINEAR")     index = linear(ID, level, i);
        if (this->type == "QUADRATIC")  index = quadratic(ID, level, i);
        if (this->type == "DOUBLE")     index = double_hash(ID, level, i);
        if (!this->empty[index]) {
            if (node[index].ID == new_data.ID && node[index].level == new_data.level) {
                tmp = new_data.ID;
                return "redeclared";
            }
        } else {
            this->node[index]       = new_data;
            this->empty[index]      = false;
            this->deleted[index]    = false;
            this->node[index].i     = i;
            this->count++;
            tmp = new_data.ID;
            return to_string(i);
        }
    }

    return "overflown";
}

string type_of_value(const string& value) {
    regex number(R"(\d+)");
    regex string(R"('[a-zA-Z\d ]*')");
    regex id(R"([a-z]\w*)");
    if (regex_match(value, number))         return "number";
    if (regex_match(value, string))         return "string";
    if (regex_match(value, id))             return "ID";
    return "invalid";
}

string HashTable::assign(const string& ID, const string& value, string &tmp, int level) {
    if (this->count == 0) {
        if (type_of_value(value) == "ID")   tmp = value;
        else                                tmp = ID;
        return "undeclared";
    }

    int num_com = 0;
    regex function(R"(([a-z]\w*)(\((('[a-zA-Z\d ]*'|\d+|[a-z]\w*)(,('[a-zA-Z\d ]*'|\d+|[a-z]\w*))*)*\))$)");
    smatch data;
    if (regex_match(value, data, function)) {           // for function
        string out = this->lookup(data[1], level, 0, -1);
        if (out == "no") {
            tmp = data[1];
            return "undeclared";
        }
        int idx = stoi(out);
        if (node[idx].num_of_parameters == -1 || node[idx].type_func == "void") {
            return "mismatch";
        }
        num_com += node[idx].i;
        if (node[idx].type == "function") {
            int idx_ID;
            out = this->lookup(ID, level, 0, -1);
            if (out == "no") {
                tmp = ID;
                return "undeclared";
            } else {
                idx_ID = stoi(out);
                if (node[idx_ID].type.empty()) {
                    return "cannot";
                }
                if (node[idx_ID].num_of_parameters != -1) {
                    return "mismatch";
                }
            }
            num_com+=node[idx_ID].i;

            string func = data[3];
            node[idx].type.clear();
            int count_tmp = 0;

            for (int i = 0; i < (int) func.size(); i++) {
                int j = (int) func.find(',',i);
                if (j == -1) {
                    j = (int) func.size();
                }
                string tmp1 = func.substr(i, j - i);
                string type_tmp1 = type_of_value(tmp1);
                if (type_tmp1 == "ID") {
                    int idx_tmp1;
                    string out1 = this->lookup(tmp1, level, 0, -1);
                    if (out1 == "no") {
                        tmp = tmp1;
                        return "undeclared";
                    } else {
                        idx_tmp1 = stoi(out1);
                        if (node[idx_tmp1].type.empty()) {
                            return "cannot";
                        }
                        if (node[idx_tmp1].num_of_parameters != -1) {
                            return "mismatch";
                        }
                    }
                    num_com+=node[idx_tmp1].i;
                    node[idx].type+=type_of_value(node[idx_tmp1].type);
                    node[idx].type+=",";
                    if (count_tmp > node[idx].num_of_parameters) {
                        return "mismatch";
                    }
                } else {
                    node[idx].type+=type_tmp1;
                    node[idx].type+=",";
                    if (count_tmp > node[idx].num_of_parameters) {
                        return "mismatch";
                    }
                }
                count_tmp++;
                i = j + 1;
            }

            node[idx].type_func = node[idx_ID].type;
        } else {
            string func = node[idx].type;
            LinkedLisst Lisst;
            Lisst.head = nullptr;
            for (int i = 0; i < (int) func.length(); i++) {     // token (type),(type),(type)
                if (func[i] != ',') {
                    int j;
                    for (j = i + 1; j < (int) func.length(); j++) {
                        if (func[j] == ',') {
                            break;
                        }
                    }
                    Lisst.insert_lisst(func.substr(i, j - i), 1);
                    i = i + (j - i);
                }
            }                                                   // lisst tail is the result if right check for all variable(s)
            string var = data[2];
            LinkedLisst Lisst1;                                 // lisst of value(s)
            for (int i = 0; i < (int) var.length(); i++) {
                if (var[i] != ',' && var[i] != '(' && var[i] != ')') {
                    int j;
                    for (j = i + 1; j < (int) var.length(); j++) {
                        if (var[j] == ',' || var[j] == ')') {
                            break;
                        }
                    }
                    Lisst1.insert_lisst(var.substr(i, j - i), 1);
                    i = i + (j - i);
                }
            }
            if (Lisst.size != Lisst1.size) {                // if number of variables is not the same
                return "mismatch";
            }
            if (Lisst.size != 1) {
                auto *h = Lisst.head, *h1 = Lisst1.head;
                for (int i = 0; i < Lisst1.size; i++) {
                    if (h1 == nullptr) break;
                    if (type_of_value(h1->ID) == "ID") {
                        int idx_ID;
                        string out1 = this->lookup(h1->ID, level, 0, -1);
                        if (out1 == "no") {
                            tmp = h1->ID;
                            return "undeclared";
                        } else {
                            idx_ID = stoi(out1);
                            if (node[idx_ID].type.empty()) {
                                node[idx_ID].type = h->ID;
                            } else {
                                if (node[idx_ID].num_of_parameters != -1) {
                                    return "mismatch";
                                }
                                if (node[idx_ID].type != h->ID) {
                                    return "mismatch";
                                }
                            }
                        }
                        num_com+=node[idx_ID].i;

                    } else if (type_of_value(h1->ID) != h->ID) {
                        return "mismatch";
                    }
                    h = h->next;
                    h1 = h1->next;
                }
            }

            int idx_ID;
            string out1 = this->lookup(ID, level, 0, -1);
            if (out1 == "no") {
                tmp = ID;
                return "undeclared";
            } else {
                idx_ID = stoi(out1);
                if (node[idx_ID].num_of_parameters != -1) {
                    return "mismatch";
                }
                if (node[idx_ID].type.empty()) {
                    node[idx_ID].type = Lisst.tail->ID;
                    num_com+=node[idx_ID].i;
                    return to_string(num_com);
                }
                if (node[idx_ID].type != node[idx].type_func) {
                    return "mismatch";
                }
            }
        }
    } else if (type_of_value(value) == "ID") {                 // value is another ID
        int idx_val;
        string out = this->lookup(value, level, 0, -1);      //lookup value node
        if (out == "no") {
            tmp = value;
            return "undeclared";
        } else {
            idx_val = stoi(out);
            if (node[idx_val].num_of_parameters != -1) {
                return "mismatch";
            }
        }
        num_com+=node[idx_val].i;

        int idx_ID;
        string out1 = this->lookup(ID, level, 0, -1);       // lookup ID node
        if (out1 == "no") {
            tmp = ID;
            return "undeclared";
        } else {
            idx_ID = stoi(out1);
            if (node[idx_val].type.empty() && node[idx_ID].type.empty()) {
                return "cannot";
            }
            if (node[idx_ID].num_of_parameters != -1) {
                return "mismatch";
            }
        }
        num_com+=node[idx_ID].i;

        if (node[idx_ID].type.empty() && !node[idx_val].type.empty()) {
            node[idx_ID].type = node[idx_val].type;
            return to_string(num_com);
        } else if (!node[idx_ID].type.empty() && node[idx_val].type.empty()) {
            node[idx_val].type = node[idx_ID].type;
            return to_string(num_com);
        }
        if (node[idx_val].type != node[idx_val].type) {
            return "mismatch";
        }

    } else {
        int idx_ID;
        string out1 = this->lookup(ID, level, 0, -1);
        if (out1 == "no") {
            tmp = ID;
            return "undeclared";
        } else {
            idx_ID = stoi(out1);
            if (node[idx_ID].num_of_parameters != -1) {
                return "mismatch";
            }
        }
        num_com+=node[idx_ID].i;

        if (node[idx_ID].type.empty()) {
            node[idx_ID].type = type_of_value(value);
            return to_string(num_com);
        }

        if (type_of_value(value) != node[idx_ID].type) {
            return "mismatch";
        }
    }

    return to_string(num_com);
}

string HashTable::call(const string& ID, const string& func, int level, string &tmp) {
    int num_com = 0;

    string out = this->lookup(ID, level, 0, -1);
    if (out == "no") {
        tmp = ID;
        return "undeclared";
    }

    int idx = stoi(out);
    if (node[idx].num_of_parameters == -1) {
        return "mismatch";
    }

    num_com += node[idx].i;
    if (node[idx].type == "function") {

        node[idx].type_func = "void";
        int count_tmp = 0;
        node[idx].type.clear();

        for (int i = 0; i < (int) func.size(); i++) {
            int j = (int) func.find(',',i);
            if (j == -1) {
                j = (int) func.size();
            }
            string tmp1 = func.substr(i, j - i);
            string type_tmp1 = type_of_value(tmp1);
            if (type_tmp1 == "ID") {
                int idx_tmp1;
                string out1 = this->lookup(tmp1, level, 0, -1);
                if (out1 == "no") {
                    tmp = tmp1;
                    return "undeclared";
                } else {
                    idx_tmp1 = stoi(out1);
                    if (node[idx_tmp1].type.empty()) {
                        return "cannot";
                    }
                    if (node[idx_tmp1].num_of_parameters != -1) {
                        return "mismatch";
                    }
                }
                num_com+=node[idx_tmp1].i;
                if (count_tmp != 0) {
                    node[idx].type+=",";
                }
                node[idx].type+=type_of_value(node[idx_tmp1].type);
                if (count_tmp > node[idx].num_of_parameters) {
                    return "mismatch";
                }
            } else {
                if (count_tmp != 0) {
                    node[idx].type+=",";
                }
                node[idx].type+=type_tmp1;
                if (count_tmp > node[idx].num_of_parameters) {
                    return "mismatch";
                }
            }
            count_tmp++;
            i = j;
        }

    } else {
        if (node[idx].type_func != "void") {
            return "mismatch";
        }

        string var = node[idx].type;
        LinkedLisst Lisst;
        Lisst.head = nullptr;
        for (int i = 0; i < (int) var.length(); i++) {     // token (type),(type),(type)
            if (var[i] != ',') {
                int j;
                for (j = i + 1; j < (int) var.length(); j++) {
                    if (var[j] == ',') {
                        break;
                    }
                }
                Lisst.insert_lisst(var.substr(i, j - i), 1);
                i = i + (j - i);
            }
        }                                                   // lisst tail is the result if right check for all variable(s)

        LinkedLisst Lisst1;                                 // lisst of value(s)
        for (int i = 0; i < (int) func.length(); i++) {
            if (func[i] != ',' && func[i] != '(' && func[i] != ')') {
                int j;
                for (j = i + 1; j < (int) func.length(); j++) {
                    if (func[j] == ',' || func[j] == ')') {
                        break;
                    }
                }
                Lisst1.insert_lisst(func.substr(i, j - i), 1);
                i = i + (j - i);
            }
        }

        if (Lisst.size != Lisst1.size) {                // if number of variables is not the same
            return "mismatch";
        }

        if (Lisst.size != 1) {
            auto *h = Lisst.head, *h1 = Lisst1.head;
            for (int i = 0; i < Lisst1.size; i++) {
                if (h1 == nullptr) break;
                if (type_of_value(h1->ID) == "ID") {
                    int idx_ID;
                    string out1 = this->lookup(h1->ID, level, 0,-1);
                    if (out1 == "no") {
                        tmp = h1->ID;
                        return "undeclared";
                    } else {
                        idx_ID = stoi(out1);
                        if (node[idx_ID].type.empty()) {
                            node[idx_ID].type = h->ID;
                        } else {
                            if (node[idx_ID].num_of_parameters != -1) {
                                return "mismatch";
                            }
                            if (node[idx_ID].type != h->ID) {
                                return "mismatch";
                            }
                        }
                    }
                    num_com+=node[idx_ID].i;

                } else {
                    if (type_of_value(h1->ID) != h->ID) {
                        return "mismatch";
                    }
                }
                h = h->next;
                h1 = h1->next;
            }
        }
    }
    return to_string(num_com);
}

string HashTable::lookup(string ID, int level, int i_tmp, int tmp) {
    if (this->count == 0) return "no";

    for (int i = i_tmp; i < this->size - 1; i++) {
        int index = 0;
        if (this->type == "LINEAR")         index = linear(ID, level, i);
        if (this->type == "QUADRATIC")      index = quadratic(ID, level, i);
        if (this->type == "DOUBLE")         index = double_hash(ID, level, i);
        if (!this->empty[index]) {
            if (node[index].ID == ID) {
                node[index].i = i;
                tmp = index;
                return lookup(ID, level, i + 1, tmp);
            }
        } else {
            if (!deleted[index]) {
                if (level == 0) {
                    if (tmp == -1) {
                        return "no";
                    } else {
                        return to_string(tmp);
                    }
                } else {
                    level--;
                    return this->lookup(ID, level, 0, tmp);
                }
            }
        }
    }

    return "no";
}

void HashTable::end(const LinkedLisst& lisst, int level) {
    if (lisst.head == nullptr) return;
    auto *h = lisst.tail;
    while (h != nullptr && h->level == level) {
        for (int i = 0; i < this->size - 1; i++) {
            int index;
            if (this->type == "LINEAR")     index = linear(h->ID, level, i);
            if (this->type == "QUADRATIC")  index = quadratic(h->ID, level, i);
            if (this->type == "DOUBLE")     index = double_hash(h->ID, level, i);
            if (!this->empty[index]) {
                if (node[index].ID == h->ID) {
                    this->empty[index]   = true;
                    this->deleted[index] = true;
                    this->count--;
                    break;
                }
            }
        }
        h = h->prev;
    }
}

void HashTable::print() {
    if (this->count == 0) return;
    int tmp = 0;
    for (int i = 0; i < this->size; i++) {
        if (!empty[i]) {
            if (tmp == 0) {
                cout << i << " " << node[i].ID << "//" << node[i].level;
                tmp++;
            } else {
                cout << ";" << i << " " << node[i].ID << "//" << node[i].level;
                tmp++;
            }
            if (tmp == this->count) {
                cout << endl;
                return;
            }
        }
    }
}

void LinkedLisst::insert_lisst(string ID, int level) {
    if (level == 0) return;

    auto *new_node = new Node(move(ID), level, nullptr, nullptr);
    if (this->head == nullptr) {
        this->head = new_node;
        this->tail = this->head;
        this->size = 1;
        return;
    }
    this->tail->next = new_node;
    new_node->prev   = this->tail;
    this->tail       = this->tail->next;
    this->size++;
}

void LinkedLisst::delete_level(const int& level) {
    if (this->head == nullptr) return;
    auto *h = this->tail;
    while (h->level == level) {
        if (this->head == h) {
            delete h;
            this->head = nullptr;
            this->tail = nullptr;
            return;
        }
        this->tail = this->tail->prev;
        this->tail->next = nullptr;
        delete h;
        h = this->tail;
    }
}

LinkedLisst::~LinkedLisst() {
    while (this->head) {
        auto *h = this->head;
        this->head = this->head->next;
        delete h;
    }
}

HashTable::~HashTable() {
    delete[] this->node;
    delete[] this->c;
    delete[] this->empty;
    delete[] this->deleted;

    this->node      = nullptr;
    this->c         = nullptr;
    this->empty     = nullptr;
    this->deleted   = nullptr;
}