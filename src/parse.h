#ifndef PARSE_H
#define PARSE_H

#include "lex.h"

typedef struct parse_state		parse_state;
typedef struct parse_node		parse_node;
typedef struct parse_ast		parse_ast;
typedef enum parse_nodetype		parse_nodetype;

enum parse_nodetype {
	NODE_UNKNOWN = 0,
	NODE_ROOT,
	NODE_FUNCTION,
	NODE_IF,
	NODE_WHILE,
	NODE_FOR,
	NODE_STATEMENT,
	NODE_BLOCK,	
	NODE_VARIABLE_DECLARATION,
	NODE_VARIABLE_ASSIGNMENT,
	NODE_VARIABLE_DUAL
};

struct parse_node {
	parse_nodetype	type;					
	parse_node*		next_in_block;	/* only valid if in block */
	parse_node*		prev_in_block;	/* only valid if in block */
	parse_node*		inners;			/* list of children in block */
	parse_node*		parent;			/* only valid if not ROOT */
	unsigned int	depth;			/* root = 0 depth */
	lex_tokenlist*	general[4];		
};

struct parse_ast {
	parse_node*		root;
	parse_node*		focus;
	lex_tokenlist*	tokens;
	lex_token*		at;
};

parse_ast*						parse_generateSyntaxTree(lex_tokenlist*);
static void						do_parse(parse_ast*);
static void						push_into_block(parse_ast*, parse_node*);
static inline void				jump_into_block(parse_ast*, parse_node*);
static void						dump_ast(parse_node*);

static void						handle_function(parse_ast*);
static void						handle_declaration(parse_ast*, lex_tokenlist*);
static void						handle_datatype(parse_ast*, lex_tokenlist*);

static parse_node*				node_init(parse_nodetype); 

static inline lex_tokenlist*	general_init();
static void						general_pushtoken(lex_tokenlist*, lex_token*);

#endif
