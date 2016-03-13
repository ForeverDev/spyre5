#ifndef LEX_H
#define LEX_H

#include <stddef.h>

typedef struct	lex_token			lex_token;
typedef struct	lex_tokenlist		lex_tokenlist;
typedef struct	lex_state			lex_state;
typedef enum	lex_tokentype		lex_tokentype;

enum lex_tokentype {
	TOK_UNKNOWN = 0x00,
	TOK_NUMBER_INT,
	TOK_NUMBER_FLOAT,
	TOK_STRING_LITERAL,
	TOK_ADD,
	TOK_SUBTRACT,
	TOK_ASTER, /* could be dereference or multiply */
	TOK_DIVIDE,
	TOK_MODULUS,
	TOK_INCREMENT_ONE,
	TOK_DECREMENT_ONE,
	TOK_ADD_INPLACE,
	TOK_SUBTRACT_INPLACE,
	TOK_MULTIPLY_INPLACE,
	TOK_DIVIDE_INPLACE,
	TOK_MODULUS_INPLACE,
	TOK_OPENPAR,
	TOK_CLOSEPAR,
	TOK_OPENSQ,
	TOK_CLOSESQ,
	TOK_OPENCURL,
	TOK_CLOSECURL,
	TOK_COMMA,
	TOK_COLON,
	TOK_DOUBLE_COLON,
	TOK_PERIOD,
	TOK_SEMICOLON,
	TOK_RETURN_ARROW,
	TOK_STRUCT,
	TOK_DATATYPE,
	TOK_CAST,
	TOK_FREE,
	TOK_NEW,
	TOK_IDENTIFIER,
	TOK_NULL_DATA
};


struct lex_token {
	char*			word;	
	unsigned int	type; 
	unsigned int	line;
	lex_token*		next;
	lex_token*		prev;
};

struct lex_tokenlist {
	lex_token*		head;
	lex_token*		tail;
};

struct lex_state {
	lex_tokenlist*	tokens;
	lex_tokenlist*	datatypes;
	char*			source;
	char*			at;
	unsigned int	current_line;
};

lex_tokenlist*		lex_generateTokens(char*);
void				die(lex_token*, const char*, ...);

static void			push_token(lex_state*, const char*, lex_tokentype);
static lex_token*	newtoken(lex_state*, const char*, lex_tokentype);
static void			do_lex(lex_state*);
static void			dump_tokens(lex_state*);

#endif
