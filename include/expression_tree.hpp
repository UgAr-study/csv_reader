#pragma once

#include "expression.hpp"

Node* BuildTree (std::vector<Node *>::iterator &cur_iter);
int TreeCalculator (const Node* top, VarValues & values);