#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

lex_tokenlist*
lex_generateTokens(char* source) {
	lex_state* L = malloc(sizeof(lex_state));	
	L->tokens = malloc(sizeof(lex_tokenlist));
	L->tokens->head = NULL;
	L->tokens->tail = NULL;
	L->datatypes = malloc(sizeof(lex_tokenlist));
	L->datatypes->head = NULL;
	L->datatypes->tail = NULL;
	L->source = source;
	L->current_line = 1;

	do_lex(L);

	return L->tokens;
}

static void
push_token(lex_state* L, const char* word, lex_tokentype type) {
	lex_token* token = newtoken(L, word, type);
	if (!L->tokens->head && !L->tokens->tail) {
		L->tokens->head = L->tokens->tail = token;
		token->prev = NULL;
	} else {
		token->prev = L->tokens->tail;
		L->tokens->tail->next = token;
		L->tokens->tail = token;
	}
}

static void
push_datatype(lex_state* L, const char* word) {
	lex_token* token = newtoken(L, word, TOK_DATATYPE);
	if (!L->datatypes->head && !L->datatypes->tail) {
		L->datatypes->head = L->datatypes->tail = token;
		token->prev = NULL;
	} else {
		token->prev = L->datatypes->tail;
		L->datatypes->tail->next = token;
		L->datatypes->tail = token;
	}
}

static int
is_datatype(lex_state* L, const char* word) {
	lex_token* token = L->datatypes->head;
	while (token) {
		if (!strcmp(token->word, word)) return 1;
		token = token->next;
	}
	return 0;
}

static lex_token*
newtoken(lex_state* L, const char* word, lex_tokentype type) {
	lex_token* token = malloc(sizeof(lex_token));
	size_t len = strlen(word);
	token->word = malloc(len + 1);
	strcpy(token->word, word);
	token->word[len] = 0;
	token->type = type;
	token->line = L->current_line;
	token->next = NULL;
	token->prev = NULL;
	return token;
}


static void
die(lex_state* L, lex_token* token, const char* message) {
	printf("\n\nSpyre Lex Error:\n\nMESSAGE: %s\n   LINE: %d\n\n\n", message, token->line);
	exit(1);
}

static void
dump_tokens(lex_state* L) {
	lex_token* token = L->tokens->head;
	while (token) {
		printf("TOKEN (type: %02d, line: %03d, word: '%s')\n", 
			token->type,
			token->line, 
			token->word
		);
		token = token->next;
	}
}

static void
do_lex(lex_state* L) {
	L->at = L->source;	
	char* buf = malloc(65536 * sizeof(char));
	char* bp = buf;
	while (*L->at) {
		/* is whitespace */
		if (*L->at == ' ') {
			L->at++;
			continue;
		/* is newline */
		} else if (*L->at == '\n') {
			L->current_line++;
			L->at++;
			continue;
		/* is a number */
		} else if (isdigit(*L->at)) {
			int is_fractional = 0;
			*bp++ = *L->at;
			L->at++;
			while (isdigit(*L->at) || *L->at == '.') {
				if (*L->at == '.') 
					is_fractional = 1;
				*bp++ = *L->at++;	
			}
			*bp = 0;
			L->at--;
			push_token(L, buf, is_fractional ? TOK_NUMBER_INT : TOK_NUMBER_FLOAT);
			bp = buf;
		/* is a string */
		} else if (*L->at == '"') {
			L->at++;
			while (*L->at != '"') {
				if (*L->at == '\\') {
					switch (*++L->at) {
						case 'n':
							*bp++ = '\n';
							break;
						case 't':
							*bp++ = '\t';
							break;
						default:
							*bp++ = *L->at;
							break;
					}
					L->at++;
				} else {
					*bp++ = *L->at++;
				}
			}
			*bp = 0;
			bp = buf;
			push_token(L, buf, TOK_STRING_LITERAL);
		/* is an operator or some sort of punctuation */
		} else if (ispunct(*L->at)) {
			*bp++ = *L->at;
			lex_tokentype type;
			switch (*L->at) {
				case '+':
					if (*(L->at + 1) == '+') {
						type = TOK_INCREMENT_ONE;
						*bp++ = '+';
						L->at++;
					} else if (*(L->at + 1) == '=') {
						type = TOK_ADD_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = TOK_ADD;
					}
					break;
				case '-':
					if (*(L->at + 1) == '-') {
						type = TOK_DECREMENT_ONE;
						*bp++ = '-';
						L->at++;
					} else if (*(L->at + 1) == '=') {
						type = TOK_SUBTRACT_INPLACE;
						*bp++ = '=';
						L->at++;
					} else if (*(L->at + 1) == '>') {
						type = TOK_RETURN_ARROW,
						*bp++ = '>';
						L->at++;
					} else {
						type = TOK_SUBTRACT;
					}
					break;
				case '*':
					if (*(L->at + 1) == '=') {
						type = TOK_MULTIPLY_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = TOK_ASTER;
					}
					break;
				case '/':
					if (*(L->at + 1) == '=') {
						type = TOK_DIVIDE_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = TOK_DIVIDE;
					}
					break;
				case '%':
					if (*(L->at + 1) == '=') {
						type = TOK_MODULUS_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = TOK_MODULUS;
					}
					break;
				case '(':
					type = TOK_OPENPAR;
					break;
				case ')':
					type = TOK_CLOSEPAR;
					break;
				case '[':
					type = TOK_OPENSQ;
					break;
				case ']':
					type = TOK_CLOSESQ;
					break;
				case '{':
					type = TOK_OPENCURL;
					break;
				case '}':
					type = TOK_CLOSECURL;
					break;
				case ',':
					type = TOK_COMMA;
					break;
				case ':':
					if (*(L->at + 1) == ':') {
						type = TOK_DOUBLE_COLON;
						*bp++ = ':';
						L->at++;
					} else {
						type = TOK_COLON;
					}
					break;
				case ';':
					type = TOK_SEMICOLON;
					break;
				default:
					/* TODO throw some sort of error? */
					printf("UNKNOWN %c\n", *L->at);
					type = TOK_UNKNOWN;
					break;
			}
			*bp = 0;
			bp = buf;
			push_token(L, buf, type);
		/* is identifier or keyword */
		} else if (isalnum(*L->at) || *L->at == '_') {
			while (*L->at && (isalnum(*L->at) || *L->at == '_'))
				*bp++ = *L->at++;
			L->at--;
			*bp = 0;
			bp = buf;
			lex_tokentype type = TOK_IDENTIFIER;
			if (!strncmp(buf, "struct", 6)) {
				type = TOK_STRUCT;
			} else if (!strncmp(buf, "cast", 4)) {
				type = TOK_CAST;	
			} else if (!strncmp(buf, "free", 4)) {
				type = TOK_FREE;
			} else if (!strncmp(buf, "new", 3)) {
				type = TOK_NEW;
			} else if (	!strncmp(buf, "u8", 2) ||
						!strncmp(buf, "u16", 3) ||
						!strncmp(buf, "u32", 3) ||
						!strncmp(buf, "u64", 3) ||
						!strncmp(buf, "s8", 2) ||
						!strncmp(buf, "s16", 3) ||
						!strncmp(buf, "s32", 3) ||
						!strncmp(buf, "s64", 3) ||
						!strncmp(buf, "float32", 7) ||
						!strncmp(buf, "float64", 7) ||
						!strncmp(buf, "int", 3))
			{
				type = TOK_DATATYPE;	
			}

			push_token(L, buf, type);
		}
		L->at++;	
	}
	
	lex_token* token = L->tokens->head;

	/* first make a pass to mark datatypes */
	while (token->next) {
		if (token->type == TOK_STRUCT && token->next->type == TOK_IDENTIFIER) {
			push_datatype(L, token->next->word);
			token = token->next;
		}
		token = token->next;
	}
	
	token = L->tokens->head;

	/* now make a pass to validate syntax */	
	while (token->next) {
		
		/* TODO add much more validataion */
		switch (token->type) {
			case TOK_RETURN_ARROW:
				/* validate that there is a return type to the function */
				if (!is_datatype(L, token->next->word)) {
					die(L, token, "Expected return type after token `->`"); 
				/* validate that there is a function body after the return type */
				} else if (token->next->next && token->next->next->type != TOK_OPENCURL) {
					die(L, token, "Expected function body after return type");
				}
				break;
			case TOK_DOUBLE_COLON:
				/* validate that there is an argument list after DOUBLE_COLON */
				if (token->next->type != TOK_OPENPAR) {
					die(L, token, "Expected argument list after token '::'");
				}
				break;
			default:
				break;
		}

		token = token->next;
	}

	dump_tokens(L);
}
