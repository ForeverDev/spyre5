#ifndef LEX_H
#define LEX_H

#include <stddef.h>

typedef struct	spy_token			spy_token;
typedef struct	spy_tokenlist		spy_tokenlist;
typedef struct	spy_lexstate		spy_lexstate;
typedef enum	spy_tokentype		spy_tokentype;

enum spy_tokentype {
	UNKNOWN = 0x00,
	NUMBER_INT,
	NUMBER_FLOAT,
	STRING_LITERAL,
	ADD,
	SUBTRACT,
	ASTER, /* could be dereference or multiply */
	DIVIDE,
	MODULUS,
	INCREMENT_ONE,
	DECREMENT_ONE,
	ADD_INPLACE,
	SUBTRACT_INPLACE,
	MULTIPLY_INPLACE,
	DIVIDE_INPLACE,
	MODULUS_INPLACE,
	OPENPAR,
	CLOSEPAR,
	OPENSQ,
	CLOSESQ,
	OPENCURL,
	CLOSECURL,
	COMMA,
	COLON,
	SEMICOLON,
	RETURN_ARROW,
	STRUCT,
	DATATYPE,
	CAST,
	FREE,
	NEW,
	IDENTIFIER
};


struct spy_token {
	char*			word;	
	size_t			word_len;
	unsigned int	type; 
	unsigned int	line;
	spy_token*		next;
	spy_token*		prev;
};

struct spy_tokenlist {
	spy_token*		head;
	spy_token*		tail;
	size_t			length;
};

struct spy_lexstate {
	spy_tokenlist*	tokens;
	size_t			tokens_len;
	size_t			source_len;
	char*			source;
	char*			at;
	unsigned int	current_line;
};

spy_tokenlist*		lex_generateTokens(char*);
static void			push_token(spy_lexstate*, const char*, spy_tokentype);
static void			do_lex(spy_lexstate*);
static void			dump_tokens(spy_lexstate*);
static void			die(spy_lexstate*, spy_token*, const char*);

#endif
