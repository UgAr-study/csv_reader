#include "expression.hpp"

int Expression::calculate(VarValues &values) const
{
    int result = 0;
    try {
        result = TreeCalculator(top, values);
    } catch (std::exception & e) {
        throw e;
    }
    return result;
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
            throw //TODO mb my own exception for cases when var is not available yet
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
            case BinOp_t::DIV:
                result = left / right;
                break;
        }
        return result;
    }

    throw std::runtime_error("Something went wrong when calculating the expression\n");
}