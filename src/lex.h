#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <vector>

namespace Lex {
	enum class TokenType {
		UNKNOWN = 0,
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		MODULUS,
		AND,
		OR,
		XOR,
		ADD_INPLACE,
		SUBTRACT_INPLACE,
		MULTIPLY_INPLACE,
		DIVIDE_INPLACE,
		MODULUS_INPLACE,
		AND_INPLACE,
		OR_INPLACE,
		XOR_INPLACE,
		INCREMENT,
		DECREMENT,
		OPENCURL,
		CLOSECURL,
		OPENPAR,
		CLOSEPAR,
		OPENSQ,
		CLOSESQ,
		PERIOD,
		SEMICOLON,
		COLON,
		DOUBLE_COLON,
		NUMBER_INT,
		NUMBER_FLOAT,
		STRING,
		STRUCT,
		IDENTIFIER
	};

	struct Token {
		std::string		word;
		unsigned int	line;
		TokenType		type;

		Token(std::string word, unsigned int line, TokenType type) : word(word), line(line), type(type) {}
	};

	class Lexer {

		private:
			Lexer(std::string& contents) : contents(contents) {};
			void						dumpTokens();

			std::string					contents;
			std::vector<Token>			tokens;
			unsigned int				current_line;

		public:
			static std::vector<Token>	generateTokens(std::string&);

	};
}

#endif


