#include "csv.hpp"
#include <sstream>

std::vector<Node*> lexer(const std::string& str);
Node* getLexem(const std::string& str, int& pos);
std::string getVarName(const std::string& str, int& pos);
int getNumber(const std::string& str, int& pos);

void CSV::load_from_file(std::string filename)
{
    std::ifstream file(filename);
    std::string line;
    // column names
    std::vector<std::string> col_names;
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
        rows[std::stoi(word)] = row_count++;
        for (auto && key: col_names) {
            std::getline(ss, word, ',');
            auto lexems = lexer(word);
            columns[key].push_back(new Expression(lexems.begin()));
        }
    }
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

std::ostream& operator<<(std::ostream & os, const CSV& csv) {
    //TODO
    return os;
}


