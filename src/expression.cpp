#include "expression.hpp"
#include "expression_tree.hpp"

Node::~Node() noexcept {}

Expression::Expression(std::vector<Node *>::iterator iter) : Node(Node_t::EXPR)
{
    top = BuildTree(iter);
}

int Expression::calculate(VarValues &values) const
{
    return TreeCalculator(top, values);
}