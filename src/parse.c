#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parse.h"

parse_ast* 
parse_generateSyntaxTree(lex_tokenlist* tokens) {
	
	parse_ast* P;

	/* setup the parse state */
	P = malloc(sizeof(parse_ast));
	
	/* setup root */
	P->root = malloc(sizeof(parse_node));
	P->root->type = NODE_BLOCK;
	P->root->next_in_block = NULL;	/* not applicable */
	P->root->prev_in_block = NULL;	/* not applicable */
	P->root->inners = NULL;			/* only init when we insert */
	P->root->parent = NULL;			/* not applicable */
	P->root->depth = 0;				/* root has depth of 0 */
	P->focus = P->root;				/* we're inserting into ROOT's block */
	P->tokens = tokens;
	P->datatypes = malloc(sizeof(lex_tokenlist));
	P->datatypes->head = NULL;
	P->datatypes->tail = NULL;
	for (int i = 0; i < 4; i++) {
		P->root->general[i] = calloc(1, sizeof(lex_tokenlist));
	}
	
	do_parse(P);

	return P;
}

/* inserts a node into the current block */
static void
push_into_block(parse_ast* P, parse_node* node) {
	if (!P->focus->inners) {
		P->focus->inners = node;
		node->next_in_block = NULL;
		node->prev_in_block = NULL;
		node->depth = P->focus->depth + 1;
		node->parent = P->focus;
	} else {
		parse_node* at = P->focus->inners;
		while (at->next_in_block) at = at->next_in_block;
		at->next_in_block = node;
		node->prev_in_block = at;
		node->next_in_block = NULL;
		node->depth = at->depth;
		node->parent = P->focus;
	}
}

static inline void
jump_into_block(parse_ast* P, parse_node* node) {
	node->parent = P->focus;
	P->focus = node;
}

static parse_node*
node_init(parse_nodetype type) {
	parse_node* node = calloc(1, sizeof(parse_node));
	node->type = type;
	for (int i = 0; i < 4; i++) {
		node->general[i] = calloc(1, sizeof(lex_tokenlist));
	}
	return node;
}

static void
dump_ast(parse_node* P) {
	while (P) {
		if (P->depth > 0) {
			char* indent = malloc(P->depth + 1);
			memset(indent, '\t', P->depth - 1);
			indent[P->depth] = 0;
			printf("%s%d\n", indent, P->type);
			for (int i = 0; i < 4; i++) {
				if (P->general[i]->head) {
					printf("%sgeneral%d\n", indent, i);
					lex_token* token = P->general[i]->head;
					while (token) {
						printf("%s(type: %02d, line: %03d, word: '%s')\n",
							indent, token->type, token->line, token->word
						);
						token = token->next;
					}
				}	
			}
			free(indent);
		}
		dump_ast(P->inners);
		P = P->next_in_block;
	}
}

static int
is_datatype(parse_ast* P, const char* identifier) {
	lex_token* token = P->datatypes->head;
	while (token) {
		if (!strcmp(token->word, identifier)) {
			return 1;
		}
	}
	return 0;
}

static inline lex_tokenlist*
tokenlist_init() {
	return (lex_tokenlist *)calloc(1, sizeof(lex_tokenlist));
}

static void
tokenlist_push(lex_tokenlist* general, lex_token* token) {
	/* create a copy of the token */
	lex_token* copy = malloc(sizeof(lex_token));
	memcpy(copy, token, sizeof(lex_token));
	copy->next = NULL;
	if (!general->head && !general->tail) {
		general->head = general->tail = copy;
		copy->prev = NULL;
	} else {
		general->tail->next = copy;
		copy->prev = general->tail->next;
		general->tail = copy;
	}
}

static void
do_parse(parse_ast* P) {
	
	P->at = P->tokens->head;	

	while (P->at->next) {
		/* branch to the correct functions */	
		if (P->at->type == TOK_IDENTIFIER && P->at->next->type == TOK_DOUBLE_COLON) {
			handle_function(P);
		} else if (P->at->type == TOK_STRUCT) {
			handle_struct(P);
		}
		P->at = P->at->next;
	}
	
	dump_ast(P->root);
}

/* should leave the parser one token after the declaration */
static void
handle_declaration(parse_ast* P, lex_tokenlist* put) {
	tokenlist_push(put, P->at);
	P->at = P->at->next;
}

/* should leave the parser one token after the datatype */
static void
handle_datatype(parse_ast* P, lex_tokenlist* put) {

}

/* should leave the parser one token after the struct */
static void
handle_struct(parse_ast*) {
		
}

/* should leave the parser one token after the function arguments */
static void
handle_function(parse_ast* P) {
	/* expects to be on the identifier of the function */
	parse_node* node = node_init(NODE_FUNCTION);
	/* store the function identifier inside genera[0] */
	tokenlist_push(node->general[0], P->at);  	
	P->at = P->at->next;
	/* now we're on the '::' token... skip ahead two so we land
	 * on the first token of the first argument or the closing
	 * of the arguments list
	 */
	P->at = P->at->next->next;
	/* layout of function arguments in node->general[1]
	 * general[1][0..n-1] = modifiers
	 * general[1][n] = datatype
	 * general[1][n+1] = identifier
	 * ...
	 */
	while (1) {
		handle_declaration(P, node->general[1]);	
		/* expects to be on last node of expression
		 * skip over that token, should land on either
		 * ')' or ','
		 */
		if (P->at->type == TOK_COMMA) {
			P->at = P->at->next;
			continue;
		} else if (P->at->type == TOK_CLOSEPAR) {
			P->at = P->at->next;
			break;
		} else {
			die(P->at, "Unexpected token in function argument list '%s', expected comma or closing parenthesis", P->at->word);
		}
	}
	
	/* if this is true then the user specified a return type.....
	 * otherwise, the function is assumed to return null
	 */
	if (P->at->type == TOK_RETURN_ARROW) {
		P->at = P->at->next;
		handle_datatype(P, node->general[2]);
	} else {
		lex_token* token = malloc(sizeof(lex_token));
		token->type = TOK_NULL_DATA;
		token->word = "null";
		token->line = 0;
		token->next = NULL;
		token->prev = NULL;
		tokenlist_push(node->general[2], token);
	}

	push_into_block(P, node);
	jump_into_block(P, node);
}

