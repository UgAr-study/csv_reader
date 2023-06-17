#include "csv.hpp"
#include <sstream>

namespace 
{

bool isCorrectSymbForVarName (char c)
{
    return std::isalpha(c) || std::isdigit(c) || c == '_';
}

std::string getVarName(const std::string &str, int& pos)
{
    std::string name = "";
    while (isCorrectSymbForVarName(str[pos])) {
        name += str[pos++];
    }
    --pos;
    return name;
}

int getNumber(const std::string& str, int& pos)
{
    auto substr = str.substr(pos, str.size() - pos);
    size_t idx;
    auto num = std::stoi(substr, &idx);
    pos += idx - 1;
    return num;
}

Node* getLexem(const std::string& str, int& pos)
{
    while (pos < str.size() && std::isspace(str[pos])) {
        ++pos;
    }
    if (pos == str.size()) {
        auto res = new End();
        return res;
    }
    if (std::isalpha(str[pos])) {
        auto res = new Variable(getVarName(str, pos));
        return res;
    }
    if (std::isdigit(str[pos])) {
        auto res = new Number(getNumber(str, pos));
        return res;
    }

    switch (str[pos]) {
        case '+': {
            auto res = new BinOp{BinOp_t::ADD};
            return res;
        }
        case '-': {
            auto res = new BinOp{BinOp_t::SUB};
            return res;
        }
        case '*': {
            auto res = new BinOp{BinOp_t::MUL};
            return res;
        }
        case '/': {
            auto res = new BinOp{BinOp_t::DIV};
            return res;
        }
        case '=': {
            auto res = new BinOp{BinOp_t::ASSIGN};
            return res;
        }
        default:
            throw std::invalid_argument("Unknown symbol: " + str[pos]);
    }
    return nullptr;
}

std::vector<Node*> lexer(const std::string& str)
{
    std::vector<Node*> lexems;
    for (int cur_pos = 0; cur_pos <= str.size(); ++cur_pos) {
        Node * lexem = getLexem(str, cur_pos);
        if (lexem == nullptr) {
            throw std::invalid_argument("unknown lexem in \""+str+"\"\n");
        }
        lexems.push_back(lexem);
    }
    return lexems;
}

// get unique variable names in the semantic
void getVariablesFromExpression(const Node* top, std::set<std::string>& variables) {
    
    if (top == nullptr)
        return;

    switch (top->getType())
    {
    case Node_t::BINOP: {
        auto op = static_cast<const BinOp*>(top);
        getVariablesFromExpression(op->getLhs(), variables);
        getVariablesFromExpression(op->getRhs(), variables);
        return;
    }
    case Node_t::EXPR:{
        auto expr = static_cast<const Expression*>(top);
        getVariablesFromExpression(expr->getTop(), variables);
        return;
    }
    case Node_t::VARNAME: {
        auto var = static_cast<const Variable*>(top);
        variables.insert(var->getName());
        return;
    }
    case Node_t::NUM:
        return;
    case Node_t::END:
        return;
    default:
        throw std::invalid_argument("getVariablesFromExpression: unknown type of node");
    }
}

} // anonymus namespace

void CSV::load_from_file(std::string filename)
{
    std::ifstream file(filename);
    std::string line;
    // column names
    std::vector<std::string> col_names;
    // std::vector<int> row_names;
    std::getline(file, line);
    std::stringstream ss(line);
    std::string word;
    std::getline(ss, word, ','); // skip first empty column
    while (std::getline(ss, word, ',')) {
        columns[word] = std::vector<Node*>();
        col_names.push_back(word);
    }
    // content
    int row_count = 0;
    while (std::getline(file, line)) {
        ss = std::stringstream(line);
        std::getline(ss, word, ',');
        rows[word] = row_count++;
        for (auto && key: col_names) {
            std::getline(ss, word, ',');
            auto lexems = lexer(word);
            columns[key].push_back(new Expression(lexems.begin()));
        }
    }
}

// calculate all the cells in csv table, throws CalculationFail exception, if there are some unsolvable cells
void CSV::process()
{
    VarValues values;
    // get all column and row names
    std::vector<std::string> col_names;
    std::vector<std::string> row_names;
    for (auto && kv: columns) {
        col_names.push_back(kv.first);
    }
    for (auto && kv: rows) {
        row_names.push_back(kv.first);
    }

    for (auto && col: col_names) {
        for (auto && row: row_names) {
            values[col+row] = nullptr;
        }
    }

    // check cycles in dependecies
    auto graph = makeDependencyGraph(col_names, row_names);
    if (graph.isCyclic()) {
        throw CalculationFail("There is an infinity recursion in table\n");
    }

    // process all the cells
    for (auto && col: col_names) {
        for (auto && row: row_names) {
            // TODO: process via queue
        }
    }

}

DirectedGraph<std::string> CSV::makeDependencyGraph(const std::vector<std::string>& col_names, const std::vector<std::string>& row_names)
{
    DirectedGraph<std::string> graph;
    for (auto && col: col_names) {
        for (auto && row: row_names) {
            std::set<std::string> dependencies;
            getVariablesFromExpression(columns[col][rows[row]], dependencies);
            for (auto && d: dependencies) {
                graph.addEdge(col+row, d);
            }
        }
    }
    return graph;
}

std::ostream& operator<<(std::ostream & os, const CSV& csv) {
    //TODO
    return os;
}


