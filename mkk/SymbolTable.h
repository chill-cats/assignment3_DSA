#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <utility>

#include "main.h"

class SymbolTable {
public:
    SymbolTable() = default;
    static void run(const string& filename);
};

class Node {
public:
    Node(string ID, int level, Node* next, Node* prev): ID(std::move(ID)), level(level), next(next), prev(prev) {}
    string  ID;
    int     level;
    Node    *next;
    Node    *prev;
};

class LinkedLisst {
public:
    Node*   head;
    Node*   tail;
    int     size;

public:
    LinkedLisst(): head(nullptr), tail(nullptr), size(0) {};
    ~LinkedLisst();

    void insert_lisst(string ID, int level);
    void delete_level(const int& level);
};

class Data {
public:
    Data()  = default;
    string  ID;
    string  type;
    string  type_func;
    int     num_of_parameters = -1;
    int     level = 0;
    int     i = 0;

public:
    void    set_data(string &ID_name, int ID_parameter, int ID_level);
};

class HashTable {
    int     size   = 0;
    int     count  = 0;
    bool    *empty{};
    bool    *deleted{};
    int     *c{};
    Data    *node{};

public:
    HashTable() = default;
    ~HashTable();

    string  type;
public:
    void    set_table(string gettype, int getsize, int *getc);
    int     linear(string ID, int level, int i);
    int     quadratic(string &ID, int level, int i);
    int     double_hash(string &ID, int level, int i);
    string  insert(string ID, int level, int num, string &tmp);
    string  assign(const string& ID, const string& value, string &tmp, int level);
    string  call(const string& ID, const string& func, int level, string &tmp);
    void    end(const LinkedLisst& lisst, int level);
    string  lookup(string ID, int level, int i_tmp, int tmp);
    void    print();
};
#endif