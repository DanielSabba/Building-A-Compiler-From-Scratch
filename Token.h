#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
//#include "Token.c"
//#include "Parser.h"

extern FILE *yyin, *yyout,*semantic_out;

typedef enum eTOKENS
{
	COMMA_tok,
	COLON_tok,
	SEMICOLON_tok,
	L_PARENTHES_tok,
	R_PARENTHES_tok,
	L_BRACKETS_tok,
	R_BRACKETS_tok,
	L_CURLY_tok,
	R_CURLY_tok,
	EQUAL_tok,
	N_EQUAL_tok,
	SMALLER_tok,
	SMALLER_EQUAL_tok,
	BIGGER_tok,
	BIGGER_EQUAL_tok,
	MULTIPLE_tok,
	ADDITION_tok,
	ASSIGN_tok,
	INT_tok,
	FLOAT_tok,
	if_tok,
	float_KEY_tok,
	int_KEY_tok,
	return_tok,
	void_tok,
	ID_tok,
	EOF_tok
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();

int currentIndex, backSteps;
Node* currentNode;

/* this function translates the kind integer of a token into a string (for printing)*/
const char *kindToStringConverter(eTOKENS num);
void printError(int line, char* lexeme);
#endif