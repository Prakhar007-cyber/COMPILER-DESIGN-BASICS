#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
#include <cctype>

using namespace std;

// Token types
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    POWER,
    LPAREN,
    RPAREN,
    END,
    INVALID
};

// Token structure
struct Token {
    TokenType type;
    double value;
    string str;

    Token(TokenType t) : type(t), value(0) {}
    Token(TokenType t, double v) : type(t), value(v) {}
    Token(TokenType t, const string& s) : type(t), value(0), str(s) {}
};

// Lexer class for tokenizing input
class Lexer {
private:
    string input;
    size_t position;
    char current_char;

    void advance() {
        position++;
        if (position < input.length()) {
            current_char = input[position];
        } else {
            current_char = '\0';
        }
    }

    void skip_whitespace() {
        while (current_char != '\0' && isspace(current_char)) {
            advance();
        }
    }

    double parse_number() {
        string num_str;
        while (current_char != '\0' && (isdigit(current_char) || current_char == '.')) {
            num_str += current_char;
            advance();
        }
        return stod(num_str);
    }

public:
    Lexer(const string& input) : input(input), position(0) {
        current_char = input.empty() ? '\0' : input[0];
    }

    Token get_next_token() {
        while (current_char != '\0') {
            if (isspace(current_char)) {
                skip_whitespace();
                continue;
            }

            if (isdigit(current_char) || current_char == '.') {
                return Token(TokenType::NUMBER, parse_number());
            }

            if (current_char == '+') {
                advance();
                return Token(TokenType::PLUS);
            }

            if (current_char == '-') {
                advance();
                return Token(TokenType::MINUS);
            }

            if (current_char == '*') {
                advance();
                if (current_char == '*') {
                    advance();
                    return Token(TokenType::POWER);
                }
                return Token(TokenType::MULTIPLY);
            }

            if (current_char == '/') {
                advance();
                return Token(TokenType::DIVIDE);
            }

            if (current_char == '(') {
                advance();
                return Token(TokenType::LPAREN);
            }

            if (current_char == ')') {
                advance();
                return Token(TokenType::RPAREN);
            }

            throw runtime_error("Invalid character: " + string(1, current_char));
        }

        return Token(TokenType::END);
    }
};

// Parser class for building AST and evaluating
class Parser {
private:
    Lexer lexer;
    Token current_token;

    void eat(TokenType type) {
        if (current_token.type == type) {
            current_token = lexer.get_next_token();
        } else {
            throw runtime_error("Unexpected token");
        }
    }

    double factor() {
        Token token = current_token;
        
        if (token.type == TokenType::NUMBER) {
            eat(TokenType::NUMBER);
            return token.value;
        } else if (token.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            double result = expr();
            eat(TokenType::RPAREN);
            return result;
        } else if (token.type == TokenType::PLUS) {
            eat(TokenType::PLUS);
            return +factor();
        } else if (token.type == TokenType::MINUS) {
            eat(TokenType::MINUS);
            return -factor();
        }
        
        throw runtime_error("Invalid factor");
    }

    double term() {
        double result = factor();

        while (current_token.type == TokenType::MULTIPLY || 
               current_token.type == TokenType::DIVIDE) {
            Token token = current_token;
            
            if (token.type == TokenType::MULTIPLY) {
                eat(TokenType::MULTIPLY);
                result *= factor();
            } else if (token.type == TokenType::DIVIDE) {
                eat(TokenType::DIVIDE);
                double divisor = factor();
                if (divisor == 0) {
                    throw runtime_error("Division by zero");
                }
                result /= divisor;
            }
        }

        return result;
    }

    double power() {
        double result = term();

        if (current_token.type == TokenType::POWER) {
            eat(TokenType::POWER);
            double exponent = power();
            result = pow(result, exponent);
        }

        return result;
    }

    double expr() {
        double result = power();

        while (current_token.type == TokenType::PLUS || 
               current_token.type == TokenType::MINUS) {
            Token token = current_token;
            
            if (token.type == TokenType::PLUS) {
                eat(TokenType::PLUS);
                result += power();
            } else if (token.type == TokenType::MINUS) {
                eat(TokenType::MINUS);
                result -= power();
            }
        }

        return result;
    }

public:
    Parser(Lexer& lexer) : lexer(lexer) {
        current_token = lexer.get_next_token();
    }

    double parse() {
        return expr();
    }
};

// Main function with REPL (Read-Eval-Print Loop)
int main() {
    cout << "CODTECH Simple Arithmetic Compiler" << endl;
    cout << "Enter arithmetic expressions or 'exit' to quit" << endl;
    cout << "Supported operations: + - * / ** ( )" << endl;
    cout << "Example: 3 + 5 * (10 - 4) / 2" << endl;
    cout << "====================================" << endl;

    string input;
    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "exit" || input == "quit") {
            break;
        }

        if (input.empty()) {
            continue;
        }

        try {
            Lexer lexer(input);
            Parser parser(lexer);
            double result = parser.parse();
            cout << "Result: " << result << endl;
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    cout << "Goodbye!" << endl;
    return 0;
}
