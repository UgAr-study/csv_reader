#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "expression.hpp"

class CSV
{
public:
    CSV() {}
    void load_from_file(std::string filename);

private:

private:
    std::unordered_map<std::string, std::vector<Node*>> columns; // name of column: column values
    std::unordered_map<int, int> rows; // name of row: idx of element in columns
};

std::ostream& operator<<(std::ostream & os, const CSV& csv);