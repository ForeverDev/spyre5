#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "parse.h"

void print_usage() {
	printf("USAGE:\n");
}

int main(int argc, char** argv) {

	if (argc <= 1) {
		print_usage();
		return 0;
	}

	const size_t len = strlen(argv[1]);
	if (strncmp(&argv[1][len - 4], ".spy", 4)) {
		printf("Expected input file with extension '.spy'\n");
		return 1;
	}

	/* create a temporary C file */	
	char* cfilename = malloc(len + 2);
	cfilename[0] = '.';
	strncat(cfilename, argv[1], len - 4);
	strncat(cfilename, ".c", 2);
	/* FOR SAFETY: ensure the output filename doesnt
	 * already exist so we don't override some existing
	 * C file!!!!!
	 */

	/* read the contents of the .spy file to compile
	 * pass the contents to the lexer
	 */
	unsigned long long flen;
	char* fcontents;
	FILE* finput;
	finput = fopen(argv[1], "r");
	if (!finput) {
		printf("Couldn't open file '%s'\n", argv[1]);
		return 1;
	}
	fseek(finput, 0, SEEK_END);
	flen = ftell(finput);
	fseek(finput, 0, SEEK_SET);
	fcontents = malloc(flen + 1);
	fread(fcontents, 1, flen, finput);
	fclose(finput);

	/* execute the lexer */
	lex_tokenlist* tokens = lex_generateTokens(fcontents);
	/* execute the parser */
	parse_ast* syntax = parse_generateSyntaxTree(tokens);
	
	remove(cfilename);	

	return 0;

}
