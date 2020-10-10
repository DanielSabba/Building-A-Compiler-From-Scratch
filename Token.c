#include "Token.h"
/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

#define TOKEN_ARRAY_SIZE 1000

currentIndex = 0, backSteps = 0;
Node *currentNode = NULL;

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{
	int length = strlen(lexeme) + 1;

	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray =
			(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray =
				(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
#ifdef _WIN32
	strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
#else
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
#endif		
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token() {
	backSteps++;
	currentIndex--;
	if (currentIndex < 0) {
		return &(currentNode->tokensArray[0]);
	}
	return &(currentNode->tokensArray[currentIndex]);		
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token):
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token *next_token()
{
	if (backSteps == 0)
	{
		int i = yylex();
		if (i == 0) {
			printf("Could'nt read next token\n");// return NULL;
			Token *tok = (Token*)malloc(sizeof(Token));
			tok->kind = EOF_tok;
			tok->lexeme = "EOF";
			tok->lineNumber = 0;
			return tok;
		}
		return &(currentNode->tokensArray[currentIndex]);
	}
	currentIndex++;
	backSteps--;
	return &(currentNode->tokensArray[currentIndex]);
	
}


/* this function translates the kind integer of a token into a string (for printing)*/
const char *kindToStringConverter(eTOKENS num) {
	char* valid_tokens[] = {
		"COMMA_tok",
		"COLON_tok",
		"SEMICOLON_tok",
		"L_PARENTHES_tok",
		"R_PARENTHES_tok",
		"L_BRACKETS_tok",
		"R_BRACKETS_tok",
		"L_CURLY_tok",
		"R_CURLY_tok",
		"EQUAL_tok",
		"N_EQUAL_tok", 
		"SMALLER_tok",
		"SMALLER_EQUAL_tok",
		"BIGGER_tok",
		"BIGGER_EQUAL_tok",
		"MULTIPLE_tok",
		"ADDITION_tok",
		"ASSIGN_tok",
		"INT_tok",
		"FLOAT_tok",
		"if_tok",
		"float_KEY_tok",
		"int_KEY_tok",
		"return_tok",
		"void_tok",
		"ID_tok",
		"EOF_tok"
	};
	return(valid_tokens[num]);
}

void printError(int yyline, char * yytext)
{
	fprintf(yyout, "The character '{%s}' at line: {%d} does not begin any legal token in the language.\n", yytext, yyline);
}

void printAcceptedTok(eTOKENS kind, int yyline, char * yytext)
{
	fprintf(yyout, "Token of kind '{%s}' was found at line : {%d}, lexeme : '{%s}'.\n", kindToStringConverter(kind), yyline, yytext);
}

