#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

parse_ast* 
parse_generateSyntaxTree(lex_tokenlist* tokens) {
	
	parse_ast* P;

	/* setup the parse state */
	P = malloc(sizeof(parse_ast));
	
	/* setup root */
	P->root = malloc(sizeof(parse_node));
	P->root->type = BLOCK;
	P->root->next_in_block = NULL;	/* not applicable */
	P->root->prev_in_block = NULL;	/* not applicable */
	P->root->inners = NULL;			/* only init when we insert */
	P->root->parent = NULL;			/* not applicable */
	P->root->depth = 0;				/* root has depth of 0 */
	P->focus = root;				/* we're inserting into ROOT's block */
	memset(root->general, NULL, sizeof(root->general));
	
	do_parse(P);

	return P;
}

/* inserts a node into the current block */
static void
push_into_block(parse_ast* P, parse_node* node) {
	if (!P->focus->inners) {
		P->focus->inners = node;
		node->next_in_block = NULL;
		node->inners->prev_in_block = NULL;
		node->depth = P->focus->depth + 1;
	} else {
		parse_node* at = P->focus->inners;
		while (at->next) at = at->next;
		at->next_in_block = node;
		node->prev_in_block = at;
		node->next_in_block = NULL;
		node->depth = at->depth;
	}
}

static parse_node*
init_node(parse_ast* P, parse_nodetype type) {
	return (parse_node *)NULL;	
}

static void
dump_ast(parse_ast* P) {
	while (P) {
		for (int i = 0; i < P->depth; i++) {
			putchar('\t'); 
		}
		printf("%d\n", P->type);
		dump_ast(P->inners);
		P = P->next_in_block;
	}
}

static void
do_parse(parse_ast* P) {
	dump_ast(P);
}
