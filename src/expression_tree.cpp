#include "expression_tree.hpp"

Node* BuildTree (std::vector<Node *>::iterator &cur_iter) 
{
    auto e_left = *(cur_iter);
    
    if (e_left != nullptr) {
        if (e_left->getType() == Node_t::END) {
            return e_left;
        }
        // in case "-A1"
        if (e_left->getType() == Node_t::BINOP) {
            auto binop = static_cast<BinOp*>(e_left);
            if (binop->getOperation() == BinOp_t::SUB) {
                auto rhs = *(++cur_iter);
                if (rhs->getType() != Node_t::VARNAME && rhs->getType() != Node_t::NUM) {
                    throw std::invalid_argument("Expected number or variable after \'-\'");
                }
                binop->setRhs(rhs);
                e_left = static_cast<Node *>(binop);
            } else if (binop->getOperation() == BinOp_t::ASSIGN) {
                e_left = BuildTree(++cur_iter);
                return e_left;
            } else {
                throw std::invalid_argument("only \'-\' or \'=\'can be set before the expression");
            }
        }

        Node* cur_lex = *(++cur_iter);
        // if there is only one lexem
        if (cur_lex->getType() == Node_t::END) {
            --cur_iter;
            return e_left;
        }

        if (cur_lex->getType() == Node_t::BINOP) {  
            auto binop = static_cast<BinOp*>(cur_lex);
            binop->setLhs(e_left);
            
            Node* rhs = *(++cur_iter);

            if (rhs->getType() != Node_t::NUM && rhs->getType() != Node_t::VARNAME) {
                throw std::invalid_argument("Expected variable or number after binary operation");
            }

            binop->setRhs(rhs);
            e_left = static_cast<Node *>(binop);
        }
    }

    return e_left;
}

int TreeCalculator (const Node* top, VarValues & values) 
{
    if (top == nullptr) {
        throw std::out_of_range("top is nullptr");
    }

    int result = 0, left = 0, right = 0;

    if (top->getType() == Node_t::NUM) {
        auto num = static_cast<const Number*>(top);
        return num->getNum();
    }

    if (top->getType() == Node_t::VARNAME) {
        auto var = static_cast<const Variable*>(top);
        
        if (values.find(var->getName()) == values.end()) {
            throw VariableNotFoundException(std::string{"TreeCalculator: unknown variable \""}+var->getName()+"\"");
        }
        
        if (values[var->getName()] == nullptr) {
            throw UnknownValue();
        }

        return values[var->getName()]->getNum();
    }


    if (top->getType() == Node_t::BINOP) {
        auto op = static_cast<const BinOp*>(top);

        if (op->getLhs() == nullptr && op->getOperation() == BinOp_t::SUB) {
            return -TreeCalculator(op->getRhs(), values);
        }

        left = TreeCalculator (op->getLhs(), values);
        right = TreeCalculator (op->getRhs(), values);
        switch (op->getOperation()) {
            case BinOp_t::ADD:
                result = left + right;
                break;
            case BinOp_t::SUB:
                result = left - right;
                break;
            case BinOp_t::MUL:
                result = left * right;
                break;
            case BinOp_t::DIV: {
                if (right == 0) {
                    throw std::runtime_error("TreeCalculator: division by zero encountered");
                }
                result = left / right;
                break;
            }
        }
        return result;
    }

    // e.g. expression is empty
    throw std::runtime_error("TreeCalculator: something went wrong when calculating the expression");
}