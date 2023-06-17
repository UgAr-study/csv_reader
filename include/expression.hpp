#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>

enum class Node_t { BINOP, EXPR, VARNAME, NUM, END };
enum class BinOp_t { ADD, MUL, SUB, DIV, ASSIGN };

class Node;
class End;
class Number;
class Variable;
class BinOp;
class Expression;

using VarValues = std::unordered_map<std::string, Number*>;

class VariableNotFoundException: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class Node {
    Node_t type;

public:
    Node (Node_t type)
        : type (type) {};
    Node_t getType () const { return type; }
    virtual ~Node() = 0;
};

class End: public Node {
public:
    End(): Node(Node_t::END) {}
    ~End() override = default;
};

class Number: public Node {
    int num;

public:
    Number(int number)
        : num (number), Node(Node_t::NUM) {};
    int getNum () const { return num; }

    ~Number() override = default;
};

class Variable: public Node {
    std::string name;

public:
    explicit Variable(std::string name)
        : Node(Node_t::VARNAME), name(std::move(name)) {};
    std::string getName () const { return name; }

    ~Variable() override = default;
};

class BinOp: public Node {
    BinOp_t operation;
    Node *lhs = nullptr, *rhs = nullptr;

public:
    explicit BinOp (BinOp_t type)
        : operation(type), Node(Node_t::BINOP) {};

    BinOp_t getOperation() const { return operation; }
    void setLhs (Node* Lhs) { lhs = Lhs; }
    void setRhs (Node* Rhs) { rhs = Rhs; }
    const Node* getLhs() const { return lhs; }
    const Node* getRhs() const { return rhs; }

    ~BinOp() override { delete lhs; delete rhs; }
};


class Expression: public Node {
    Node* top;

public:
    Expression(): Node(Node_t::EXPR) { top = nullptr; };
    Expression(std::vector<Node*>::iterator iter);
    int calculate(VarValues & values) const;
    Node_t getTopType() const { return top->getType(); }
    const Node* getTop() const { return top; }

    ~Expression() override { delete top; }

};