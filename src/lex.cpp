#include "lex.h"

using namespace Lex;

std::vector<Token>
Lexer::generateTokens(std::string& contents) {
	Lexer L(contents);

	size_t i = 0;
	std::string buf;
	TokenType type = TokenType::UNKNOWN;

	for (; i < contents.length(); i++) {
		/* check whitespace and tab */
		if (contents[i] == ' ' || contents[i] == '\t') {
			continue;
		/* check newline */
		} else if (contents[i] == '\n') {
			L.current_line++;
			continue;
		/* check number */
		} else if (std::isdigit(contents[i])) {
			bool isfloat = false;
			while (std::isdigit(contents[i]) || contents[i] == '.') {
				if (contents[i] == '.') isfloat = true;
				buf += contents[i++];
			}
			i--;
			type = isfloat ? TokenType::NUMBER_FLOAT : TokenType::NUMBER_INT;
		/* check string */
		} else if (contents[i] == '"') {
			i++;
			while (contents[i] != '"') {
				if (contents[i] == '\\') {
					switch (contents[i + 1]) {
						case 'n':
							buf += '\n';
							break;
						case 't':
							buf += '\t';
							break;
						default:
							buf += contents[i + 1];
					}
					i += 2;
				} else {
					buf += contents[i++];
				}
			}
			type = TokenType::STRING;
		/* check punctuation */
		} else if (std::ispunct(contents[i])) {
			buf += contents[i];
			switch (contents[i]) {
				case '+':
					if (contents[i + 1] == '+') {
						type = TokenType::INCREMENT;
						buf += contents[++i];
					} else if (contents[i + 1] == '=') {
						type = TokenType::ADD_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::ADD;
					}
					break;
				case '-':
					if (contents[i + 1] == '-') {
						type = TokenType::DECREMENT;
						buf += contents[++i];
					} else if (contents[i + 1] == '=') {
						type = TokenType::SUBTRACT_INPLACE;
					} else {
						type = TokenType::SUBTRACT;
					}
					break;
				case '*':
					if (contents[i + 1] == '=') {
						type = TokenType::MULTIPLY_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::MULTIPLY;
					}
					break;
				case '/':
					if (contents[i + 1] == '=') {
						type = TokenType::DIVIDE_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::DIVIDE;
					}
					break;
				case '%':
					if (contents[i + 1] == '=') {
						type = TokenType::MODULUS_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::MODULUS;
					}
					break;
				case '&':
					if (contents[i + 1] == '=') {
						type = TokenType::AND_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::AND;
					}
					break;
				case '|':
					if (contents[i + 1] == '=') {
						type = TokenType::OR_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::OR;
					}
					break;
				case '^':
					if (contents[i + 1] == '=') {
						type = TokenType::XOR_INPLACE;
						buf += contents[++i];
					} else {
						type = TokenType::XOR;
					}
					break;
				case ':':
					if (contents[i + 1] == ':') {
						type = TokenType::DOUBLE_COLON;
						buf += contents[++i];
					} else {
						type = TokenType::COLON;
					}
					break;

				default:
					type = (
						contents[i] == '{' ? TokenType::OPENCURL :
						contents[i] == '}' ? TokenType::CLOSECURL :
						contents[i] == '(' ? TokenType::OPENPAR :
						contents[i] == ')' ? TokenType::CLOSEPAR :
						contents[i] == '[' ? TokenType::OPENSQ :
						contents[i] == ']' ? TokenType::CLOSESQ :
						contents[i] == '.' ? TokenType::PERIOD :
						contents[i] == ';' ? TokenType::SEMICOLON :
						TokenType::UNKNOWN
					);
			}
		/* it is an identifier or unknown punctuation */	
		} else {
			while (std::isalnum(contents[i])) {
				buf += contents[i++];
			}
			if (buf == "struct") {
				type = TokenType::STRUCT;
			} else {
				type = TokenType::IDENTIFIER;
			}
			i--;
		}
		L.tokens.push_back(Token(buf, L.current_line, type));
		buf.clear();
	}

	L.dumpTokens();

	return L.tokens;	
}

void 
Lexer::dumpTokens() {
	for (auto i = tokens.begin(); i != tokens.end(); i++) {
		std::cout << "TOKEN (type: " << static_cast<int>(i->type) << ", word: '" << i->word << "')\n";
	}
}

