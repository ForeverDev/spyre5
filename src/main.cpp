#include <iostream>
#include <sstream>
#include <fstream>
#include "lex.h"

int main(int argc, char** argv) {
	
	if (argc <= 1) {
		printf("Expected file name\n");
		exit(1);
	}

	/* read the file to lex */
	std::ifstream file(argv[1]);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string contents(buffer.str());

	Lex::Lexer::generateTokens(contents);	

	return 0;

}
