#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "expression.hpp"
#include "graph.hpp"

class CalculationFail: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class CSV
{
    std::unordered_map<std::string, std::vector<Node*>> columns; // name of column: column values
    std::unordered_map<std::string, int> rows; // name of row: idx of element in columns
public:
    CSV() {}
    void load_from_file(std::string filename);
    void process();
private:
    DirectedGraph<std::string> makeDependencyGraph(const std::vector<std::string>& col_names,
                                                   const std::vector<std::string>& row_names);
};

std::ostream& operator<<(std::ostream & os, const CSV& csv);