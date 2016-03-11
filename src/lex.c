#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

spy_tokenlist*
lex_generateTokens(char* source) {
	spy_lexstate* L = malloc(sizeof(spy_lexstate));	
	L->tokens = malloc(sizeof(spy_tokenlist));
	L->tokens->head = NULL;
	L->tokens->tail = NULL;
	L->tokens->length = 0;
	L->tokens_len = 0;
	L->source = source;
	L->current_line = 1;

	do_lex(L);

	return L->tokens;
}

static void
push_token(spy_lexstate* L, const char* word, spy_tokentype type) {
	spy_token* token = malloc(sizeof(spy_token));
	size_t len = strlen(word);
	token->word = malloc(len + 1);
	strcpy(token->word, word);
	token->word[len] = 0;
	token->word_len = len;
	token->type = type;
	token->line = L->current_line;
	token->next = NULL;
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
do_lex(spy_lexstate* L) {
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
			push_token(L, buf, is_fractional ? NUMBER_INT : NUMBER_FLOAT);
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
			push_token(L, buf, STRING_LITERAL);
		/* is an operator or some sort of punctuation */
		} else if (ispunct(*L->at)) {
			*bp++ = *L->at;
			spy_tokentype type;
			switch (*L->at) {
				case '+':
					if (*(L->at + 1) == '+') {
						type = INCREMENT_ONE;
						*bp++ = '+';
						L->at++;
					} else if (*(L->at + 1) == '=') {
						type = ADD_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = ADD;
					}
					break;
				case '-':
					if (*(L->at + 1) == '-') {
						type = DECREMENT_ONE;
						*bp++ = '-';
						L->at++;
					} else if (*(L->at + 1) == '=') {
						type = SUBTRACT_INPLACE;
						*bp++ = '=';
						L->at++;
					} else if (*(L->at + 1) == '>') {
						type = RETURN_ARROW,
						*bp++ = '>';
						L->at++;
					} else {
						type = SUBTRACT;
					}
					break;
				case '*':
					if (*(L->at + 1) == '=') {
						type = MULTIPLY_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = ASTER;
					}
					break;
				case '/':
					if (*(L->at + 1) == '=') {
						type = DIVIDE_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = DIVIDE;
					}
					break;
				case '%':
					if (*(L->at + 1) == '=') {
						type = MODULUS_INPLACE;
						*bp++ = '=';
						L->at++;
					} else {
						type = MODULUS;
					}
					break;
				case '(':
					type = OPENPAR;
					break;
				case ')':
					type = CLOSEPAR;
					break;
				case '[':
					type = OPENSQ;
					break;
				case ']':
					type = CLOSESQ;
					break;
				case '{':
					type = OPENCURL;
					break;
				case '}':
					type = CLOSECURL;
					break;
				case ',':
					type = COMMA;
					break;
				case ':':
					type = COLON;
					break;
				case ';':
					type = SEMICOLON;
					break;
				default:
					/* TODO throw some sort of error? */
					printf("UNKNOWN %c\n", *L->at);
					type = UNKNOWN;
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
			spy_tokentype type = IDENTIFIER;
			if (!strncmp(buf, "struct", 6)) {
				type = STRUCT;
			} else if (!strncmp(buf, "cast", 4)) {
				type = CAST;	
			} else if (!strncmp(buf, "free", 4)) {
				type = FREE;
			} else if (!strncmp(buf, "new", 3)) {
				type = NEW;
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
				type = DATATYPE;	
			}

			push_token(L, buf, type);
		}
		L->at++;	
	}
	
	spy_token* token = L->tokens->head;

	/* first make a pass to mark datatypes */
	while (token->next) {
		if (token->type == STRUCT && token->next->type == IDENTIFIER) {
			/* TODO mark token->next->word as a datatype */
			token = token->next;
		}
		token = token->next;
	}
	
	token = L->tokens->head;

	/* now make a pass to validate syntax */	
	while (token->next) {

		switch (token->type) {
			case RETURN_ARROW:
				/* validate that there is a return type to the function */
				if (token->next->type != DATATYPE) {
					die(L, token, "Expected return type after token `->`"); 
				/* validate that there is a function body after the return type */
				} else if (token->next->next && token->next->next->type != OPENCURL) {
					die(L, token, "Expected function body after return type");
				}
				break;
			default:
				break;
		}

		token = token->next;
	}

	dump_tokens(L);
}

static void
die(spy_lexstate* L, spy_token* token, const char* message) {
	printf("\n\nSpyre Lex Error:\n\nMESSAGE: %s\n   LINE: %d\n\n\n", message, token->line);
	exit(1);
}

static void
dump_tokens(spy_lexstate* L) {
	spy_token* token = L->tokens->head;
	while (token) {
		printf("TOKEN (type: %02d, line: %03d, word: '%s')\n", 
			token->type,
			token->line, 
			token->word
		);
		token = token->next;
	}
}
