#include "lex.h"

using namespace Lex;

std::vector<Token>
Lexer::generateTokens() {

	size_t i = 0;
	std::string buf;
	TokenType type = TokenType::UNKNOWN;

	std::string content = *contents;

	for (; i < content.length(); i++) {
		/* check whitespace and tab */
		if (content[i] == ' ' || content[i] == '\t') {
			continue;
		/* check newline */
		} else if (content[i] == '\n') {
			current_line++;
			continue;
		/* check number */
		} else if (std::isdigit(content[i])) {
			bool isfloat = false;
			while (std::isdigit(content[i]) || content[i] == '.') {
				if (content[i] == '.') isfloat = true;
				buf += content[i++];
			}
			i--;
			type = isfloat ? TokenType::NUMBER_FLOAT : TokenType::NUMBER_INT;
		/* check string */
		} else if (content[i] == '"') {
			i++;
			while (content[i] != '"') {
				if (content[i] == '\\') {
					switch (content[i + 1]) {
						case 'n':
							buf += '\n';
							break;
						case 't':
							buf += '\t';
							break;
						default:
							buf += content[i + 1];
					}
					i += 2;
				} else {
					buf += content[i++];
				}
			}
			type = TokenType::STRING;
		/* check punctuation */
		} else if (std::ispunct(content[i])) {
			buf += content[i];
			switch (content[i]) {
				case '+':
					if (content[i + 1] == '+') {
						type = TokenType::INCREMENT;
						buf += content[++i];
					} else if (content[i + 1] == '=') {
						type = TokenType::ADD_INPLACE;
						buf += content[++i];
					} else {
						type = TokenType::ADD;
					}
					break;
				case '-':
					if (content[i + 1] == '-') {
						type = TokenType::DECREMENT;
						buf += content[++i];
					} else if (content[i + 1] == '=') {
						type = TokenType::SUBTRACT_INPLACE;
						buf += content[++i];
					} else if (content[i + 1] == '>') {
						type = TokenType::RETURN_ARROW;
						buf += content[++i];
					} else {
						type = TokenType::SUBTRACT;
					}
					break;
				case '*':
					if (content[i + 1] == '=') {
						type = TokenType::MULTIPLY_INPLACE;
						buf += content[++i];
					} else {
						type = TokenType::MULTIPLY;
					}
					break;
				case '/':
					if (content[i + 1] == '=') {
						type = TokenType::DIVIDE_INPLACE;
						buf += content[++i];
					} else {
						type = TokenType::DIVIDE;
					}
					break;
				case '%':
					if (content[i + 1] == '=') {
						type = TokenType::MODULUS_INPLACE;
						buf += content[++i];
					} else {
						type = TokenType::MODULUS;
					}
					break;
				case '&':
					if (content[i + 1] == '=') {
						type = TokenType::AND_INPLACE;
						buf += content[++i];
					} else {
						type = TokenType::AND;
					}
					break;
				case '|':
					if (content[i + 1] == '=') {
						type = TokenType::OR_INPLACE;
						buf += content[++i];
					} else {
						type = TokenType::OR;
					}
					break;
				case '^':
					type = TokenType::POINTER;
					break;
				case ':':
					if (content[i + 1] == ':') {
						type = TokenType::DOUBLE_COLON;
						buf += content[++i];
					} else {
						type = TokenType::COLON;
					}
					break;

				default:
					type = (
						content[i] == '{' ? TokenType::OPENCURL :
						content[i] == '}' ? TokenType::CLOSECURL :
						content[i] == '(' ? TokenType::OPENPAR :
						content[i] == ')' ? TokenType::CLOSEPAR :
						content[i] == '[' ? TokenType::OPENSQ :
						content[i] == ']' ? TokenType::CLOSESQ :
						content[i] == '.' ? TokenType::PERIOD :
						content[i] == ';' ? TokenType::SEMICOLON :
						content[i] == ',' ? TokenType::COMMA :
						TokenType::UNKNOWN
					);
			}
		/* it is an identifier or unknown punctuation */	
		} else {
			while (std::isalnum(content[i])) {
				buf += content[i++];
			}
			if (buf == "struct") {
				type = TokenType::STRUCT;
			} else {
				type = TokenType::IDENTIFIER;
			}
			i--;
		}
		tokens.push_back(Token(buf, current_line, type));
		buf.clear();
	}

	return tokens;	
}

void 
Lexer::dumpTokens() {
	for (auto i = tokens.begin(); i != tokens.end(); i++) {
		std::cout << "TOKEN (type: " << static_cast<int>(i->type) << ", word: '" << i->word << "')\n";
	}
}

