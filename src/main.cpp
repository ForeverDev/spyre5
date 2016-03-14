#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include "lex.h"
#include "parse.h"

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

	/* generate the output file name */
	unsigned int len;
	std::string output_name(argv[1]);
	len = output_name.length();
	output_name[len - 3] = 'c';
	output_name[len - 2] = 0;
	output_name[len - 1] = 0;
	
	std::cout << "compiling..." << std::endl;	

	Lex::Lexer* lexer;
	Parse::Parser* parser;
	lexer = new Lex::Lexer(&contents);
	std::vector<Lex::Token> tokens = lexer->generateTokens();
	parser = new Parse::Parser(&tokens, output_name);
	parser->generateSyntaxTree();

	std::cout << "compiling C output..." << std::endl;
	std::string command = "gcc ";
	command += output_name;
	system(command.c_str());


	return 0;

}
