//#include "Parser.h"
#include "Token.h"
#include "Parser.h"

int match(eTOKENS t)
{
	Token *next_tok = next_token();
	if (next_tok->kind == t) {
		//printf("GOT MATCH %s, compared to %s\n", kindToStringConverter(next_tok->kind),kindToStringConverter(t));
		return 1;
	}
	else {
		//printf("GOT MISMATCH %s, expected %s\n", kindToStringConverter(next_tok->kind), kindToStringConverter(t));

		return 0;
	}
}

void errorHandler(eTOKENS *follow_tokens_array, int size)
{
	Token *current_token = next_token();
	int i;

	current_token = back_token();

	while (current_token->kind != EOF_tok) {
		for (i = 0; i < size; i++) {
			char* s = kindToStringConverter(follow_tokens_array[i]);
			char* b = kindToStringConverter(current_token->kind);
			//fprintf(syntatic_out,"current tok -> %s ,matched with -> %s\n",b ,s);
			if (current_token->kind == follow_tokens_array[i]) {
				current_token = back_token();
				return;
			}
		}
		current_token = next_token();
	}
}


struct attribute Parse_FACTOR()
{
	fprintf(syntatic_out, "RULE (FACTOR --> VAR | CALL | int_num | float_num | (EXPR))\n");
	eTOKENS follow_FACTOR[3] = { MULTIPLE_tok,ASSIGN_tok,SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	struct table_entry *entr;
	switch (next_tok->kind) {
	case ID_tok:
		entr = find(currentTable,next_tok->lexeme);
		//if (entr == NULL)
		//	print_sem_error("calling for an element that isn't declared", next_tok->lineNumber, next_tok->lexeme);
		next_tok = next_token();
		switch (next_tok->kind) {
		case L_PARENTHES_tok:
			back_tok = back_token();
			back_tok = back_token();
			T = Parse_CALL();
			if (entr != NULL) {
				T.type = entr->type;
				T.name = entr->name;
				T.value = entr->value; // this is the returned value from the function;
				//if (check_fun_call_params(entr, T) == 0) {
				//	print_sem_error("function arguments doesnt match to decleration", next_tok->lineNumber, next_tok->lexeme);
				//	T.type = void_tok;
				return T;
			}
			else {
				T.type = void_tok; // if cant find a function decleration, putting void in the returned factor;
				return T;
			}
		default:
			back_tok = back_token();
			back_tok = back_token();
			T = Parse_VAR();
			return T;
		}
		break;
	case FLOAT_tok:
		T.value = atoi(next_tok->lexeme);
		//T.name = next_tok->lexeme;
		T.type = FLOAT_tok;
		return T;
	case INT_tok:
		T.value = atoi(next_tok->lexeme);
		//T.name = next_tok->lexeme;
		T.type = INT_tok;
		return T;
	case L_PARENTHES_tok:
		T = Parse_EXPR();
		match(R_PARENTHES_tok);
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok,FLOAT_tok,INT_tok,L_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_FACTOR, 1);
		T.type = void_tok; // if cant find a function decleration, putting void in the returned factor;
		T.name = next_tok->lexeme;
		return T;
	}
}

struct attribute Parse_TERM_TAG()
{
	fprintf(syntatic_out, "RULE (TERM_TAG --> FACTOR TERM_TAG | EPS)\n");
	eTOKENS follow_TERM_TAG[10] = { ADDITION_tok,SEMICOLON_tok,L_PARENTHES_tok,SMALLER_tok,SMALLER_EQUAL_tok,EQUAL_tok,BIGGER_tok,BIGGER_EQUAL_tok,R_PARENTHES_tok,L_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct table_ptr *tab;
	struct attribute T,N,B;
	switch (next_tok->kind) {
	case MULTIPLE_tok:
		T = Parse_FACTOR();
		N = Parse_TERM_TAG();
		if (N.type == T.type)
			B.type = N.type;
		if ((N.type == FLOAT_tok && T.type == INT_tok) || (T.type == FLOAT_tok && N.type == INT_tok))
			B.type = FLOAT_tok;
		if (N.type == void_tok || T.type == void_tok)
		{
			B.type = void_tok;
			return B;
		}
		B.value = T.value * N.value;
		//B.name = malloc(sizeof(char)*(int)log10(temp) + 1);
		//sprintf_s(B.name, (int)log10(temp), "%d", temp); // rewriting the data as a string format;
	 // rewriting the data as a string format;
		return B;
	case R_CURLY_tok:
	case SEMICOLON_tok:
	case EQUAL_tok:
	case N_EQUAL_tok:
	case SMALLER_tok:
	case SMALLER_EQUAL_tok:
	case BIGGER_tok:
	case BIGGER_EQUAL_tok:
	case R_PARENTHES_tok:
		back_tok = back_token();
		T.value = 1;
		T.type = INT_tok;
		return T;
	default:
		back_tok = back_token();
		T.value = 1;
		T.type = INT_tok;
		return T;
	}
}

struct attribute Parse_TERM()
{
	fprintf(syntatic_out, "RULE (TERM --> FACTOR TERM_TAG)\n");
	eTOKENS follow_TERM[1] = { ADDITION_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N, B;
	switch (next_tok->kind) {
	case INT_tok:
	case FLOAT_tok:
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		T = Parse_FACTOR();
		N = Parse_TERM_TAG();
		if (N.type == T.type)
			B.type = N.type;
		if ((N.type == FLOAT_tok && T.type == INT_tok) || (T.type == FLOAT_tok && N.type == INT_tok))
			B.type = FLOAT_tok;
		if (N.type == void_tok || T.type == void_tok) {
			B.type = void_tok;
			return B;
		}
		B.value = T.value*N.value;
		//B.name = malloc(sizeof(char)*(int)log10(temp) + 1);
		//sprintf_s(B.name, (int)log10(temp), "%d", temp); // rewriting the data as a string format;
		return B;
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok,FLOAT_tok,INT_tok,L_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_TERM, 1);
		break;
	}
}

struct attribute Parse_EXPR_TAG()
{
	fprintf(syntatic_out, "RULE (EXPR_TAG --> + TERM EXPR_TAG | EPS)\n");
	eTOKENS follow_EXPR_TAG[10] = { COMMA_tok,SEMICOLON_tok,L_PARENTHES_tok,SMALLER_tok,SMALLER_EQUAL_tok,EQUAL_tok,BIGGER_tok,BIGGER_EQUAL_tok,R_PARENTHES_tok,R_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T,N, B;
	switch (next_tok->kind) {
	case SMALLER_tok:
	case SMALLER_EQUAL_tok:
	case BIGGER_tok:
	case BIGGER_EQUAL_tok:
	case R_PARENTHES_tok:
	case COMMA_tok:
	case SEMICOLON_tok:
	case L_PARENTHES_tok:
	case R_CURLY_tok:
		T.name = "1"; // this is a dummy var cause addition isnt sensitive to 0;
		T.value = 0;
		T.count = 0;
		T.type = INT_tok;
		back_tok = back_token();
		return T;
	case ADDITION_tok:
		T = Parse_TERM();
		N = Parse_EXPR_TAG();
		if (N.type == T.type)
			B.type = N.type;
		if ((N.type == FLOAT_tok && T.type == INT_tok) || (T.type == FLOAT_tok && N.type == INT_tok))
			B.type = FLOAT_tok;
		if (N.type == void_tok || T.type == void_tok)
		{
			B.type = void_tok;
			return B;
		}
		B.value = T.value + N.value;
		//B.name = malloc(sizeof(char)*(int)log10(temp) + 1);
		//sprintf_s(B.name, (int)log10(temp), "%d", temp); // rewriting the data as a string format;
		return B;
	default:
		back_tok = back_token();
		B.count = 0;
		return B;
	}
}

struct attribute Parse_EXPR()
{
	fprintf(syntatic_out, "RULE (EXPR --> TERM EXPR_TAG)\n");
	eTOKENS follow_EXPR[10] = { COMMA_tok,SEMICOLON_tok,L_PARENTHES_tok,SMALLER_tok,SMALLER_EQUAL_tok,EQUAL_tok,BIGGER_tok,BIGGER_EQUAL_tok,L_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N, B;
	switch (next_tok->kind) {
	case INT_tok:
	case FLOAT_tok:
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		T = Parse_TERM();
		T.count = 0;
		N = Parse_EXPR_TAG();
		if (N.count == 0)
			return T;
		else {
			if (N.type == T.type)
				B.type = N.type;
			if ((N.type == FLOAT_tok && T.type == INT_tok) || (T.type == FLOAT_tok && N.type == INT_tok))
				B.type = FLOAT_tok;
			if (N.type == void_tok || T.type == void_tok)
			{
				B.type = void_tok;
				return B;
			}
			B.value = T.value + N.value;
			//B.name = malloc(sizeof(char)*(int)log10(temp) + 1);
			//sprintf_s(B.name, (int)log10(temp), "%d", temp); // rewriting the data as a string format;
			return B;
		}
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok,FLOAT_tok,INT_tok,L_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_EXPR, 10);
		break;
	}
}

void Parse_rel_op() {
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case EQUAL_tok:
	case N_EQUAL_tok:
	case SMALLER_tok:
	case SMALLER_EQUAL_tok:
	case BIGGER_tok:
	case BIGGER_EQUAL_tok:
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{EQUAL_tok,N_EQUAL_tok,SMALLER_tok,SMALLER_EQUAL_tok,BIGGER_tok,BIGGER_EQUAL_tok,}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		break;
	}
}

void Parse_CONDITION()
{
	fprintf(syntatic_out, "RULE (CONDITION --> EXPR rel EXPR)\n");
	eTOKENS follow_CONDITION[1] = { R_PARENTHES_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case INT_tok:
	case FLOAT_tok:
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		Parse_EXPR();
		Parse_rel_op();
		Parse_EXPR();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok,FLOAT_tok,INT_tok,L_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_CONDITION, 1);
		break;
	}
}

struct attribute Parse_EXPR_LIST_TAG()
{
	fprintf(syntatic_out, "RULE (EXPR_LIST_TAG --> EXPR EXPR_LIST_TAG)\n");
	eTOKENS follow_EXPR_LIST_TAG[2] = { R_BRACKETS_tok, ASSIGN_tok};
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N,B;
	switch (next_tok->kind) {
	case R_BRACKETS_tok:
	case ASSIGN_tok:
		back_tok = back_token();
		//T.type = void_tok;
		T.count = 0;
		return T;
	case COMMA_tok:
		T = Parse_EXPR();  
		N = Parse_EXPR_LIST_TAG();
		B = insert_expr(T, N);
		//B.count += 1;
		return B;
	default:
		back_tok = back_token();
		//T.type = void_tok;
		T.count = 0;
		return T;
	}
}

struct attribute Parse_EXPR_LIST()
{
	fprintf(syntatic_out, "RULE (EXPR_LIST --> EXPR EXPR_LIST_TAG)\n");
	eTOKENS follow_EXPR_LIST[1] = { R_BRACKETS_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N,B;
	switch (next_tok->kind) {
	case R_BRACKETS_tok:
		T.count = 0;
		T.type = void_tok;
		return T;
	case INT_tok:
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		T = Parse_EXPR();
		T.count = 1;
		N = Parse_EXPR_LIST_TAG();
		B = insert_expr(T, N);
		return B;
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok,INT_tok,L_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_EXPR_LIST, 1);
		break;
	}
}

struct attribute Parse_VAR_TAG() {
	fprintf(syntatic_out, "RULE (VAR_TAG --> id VAR_TAG)\n");
	eTOKENS follow_VAR_TAG[10] = { ASSIGN_tok,SEMICOLON_tok,EQUAL_tok,N_EQUAL_tok,SMALLER_tok,SMALLER_EQUAL_tok, BIGGER_tok,BIGGER_EQUAL_tok,R_PARENTHES_tok,R_CURLY_tok};
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N;
	switch (next_tok->kind) {
	case L_BRACKETS_tok:
		T = Parse_EXPR_LIST();
		match(R_BRACKETS_tok);
		return T;
	case ASSIGN_tok:
	case SEMICOLON_tok:
	case ADDITION_tok:
	case MULTIPLE_tok:
	case EQUAL_tok:
	case N_EQUAL_tok:
	case SMALLER_tok:
	case SMALLER_EQUAL_tok:
	case BIGGER_tok:
	case BIGGER_EQUAL_tok:
	case R_PARENTHES_tok:
	case R_CURLY_tok:
		back_tok = back_token();
		T.count = 0;
		return T;
	default:
		T.count = 0;
		back_tok = back_token();
		return T;
	}
}

struct attribute Parse_VAR()
{
	fprintf(syntatic_out, "RULE (VAR --> id VAR_TAG)\n");
	eTOKENS follow_VAR[1] = { ASSIGN_tok }; 
	Token *next_tok = next_token(), *back_tok;
	struct table_entry *ent;
	struct attribute T,N;
	switch (next_tok->kind) {
	case ID_tok:
		T = Parse_VAR_TAG();
		T.name = next_tok->lexeme;
		ent = find(currentTable, T.name);
		if (ent == NULL) {
			print_sem_error("couldn't find variable", next_tok->lineNumber, T.name);
			T.type = void_tok;
			return T;
		}
		else {
			switch (check_if_array_correct(ent, T)) {
			case 0:
				print_sem_error("too many/few indexes for array", next_tok->lineNumber, T.name);
				T.type = ent->type;
				return T;
			case 1:
				print_sem_error("index is oversized", next_tok->lineNumber, T.name);
				T.type = ent->type;
				return T;
			case 2:
				T.name = ent->name;
				T.value = ent->value; // if this an array - check how to return the element in the index;
				T.type = ent->type;
				//T.sizes = ent->listOfSizes; // already have those
				//T.count = ent->size;
				return T;
			}
		}
		return T; // not must;
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_VAR, 1);
		break;
	}
}

struct attribute Parse_RETURN_STMT_TAG()
{
	fprintf(syntatic_out, "RULE (RETURN_STMT_TAG --> EXPR | EPS)\n");
	eTOKENS follow_RETURN_STMT_TAG[2] = { SEMICOLON_tok ,R_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case SEMICOLON_tok:
	case R_CURLY_tok:
		back_tok = back_token();
		T.type = void_tok;
		T.role = ret_var;
		return T;
	case INT_tok:
		T.type = INT_tok;
		T.value = atoi(next_tok->lexeme);
		T.role = ret_var;
		return T;
	case FLOAT_tok:
		T.type = FLOAT_tok;
		T.value = atoi(next_tok->lexeme);
		T.role = ret_var;
		return T;
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		T = Parse_EXPR();
		T.role = ret_var;
		return T;
	default:
		back_tok = back_token();
		break;
	}
}

struct attribute Parse_RETURN_STMT()
{
	fprintf(syntatic_out, "RULE (RETURN_STMT --> return RETURN_STMT_TAG)\n");
	eTOKENS follow_RETURN_STMT[1] = { SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case return_tok:
		T = Parse_RETURN_STMT_TAG();
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{return_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_RETURN_STMT, 1);
		break;
	}
}

struct attribute Parse_ARG_LIST_TAG()
{
	fprintf(syntatic_out, "RULE (ARG_LIST_TAG --> , EXPR ARG_LIST_TAG | EPS)\n");
	eTOKENS follow_ARG_LIST_TAG[1] = { R_PARENTHES_tok }; //need to check if this is true
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N, B;
	
	switch (next_tok->kind) {
	case COMMA_tok:
		T = Parse_EXPR();
		N = Parse_ARG_LIST_TAG();
		B = insert_args(T, N);
		return B;
	case R_PARENTHES_tok: // this too
		//B.parameters = (struct func_params*)malloc(sizeof(struct func_params)*tb_en_size);
		B.count = 0;
		return B;
	default:
		fprintf(syntatic_out, "Expected token of type '{COMMA_tok,R_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_ARG_LIST_TAG, 1);
		break;
	}
}

struct attribute Parse_ARG_LIST()
{
	fprintf(syntatic_out, "RULE (ARG_LIST --> EXPR ARG_LIST_TAG)\n");
	//eTOKENS follow_ARG_LIST[1] = { SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T, N, B;
	switch (next_tok->kind) {
	case INT_tok:
	case FLOAT_tok:
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		T = Parse_EXPR();
		N = Parse_ARG_LIST_TAG();
		B = insert_args(T, N);
		return B;
	default:
		fprintf(syntatic_out,"Expected token of type '{INT_tok,FLOAT_tok,L_PARENTHES_tok,ID_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		break;
	}
}

struct attribute Parse_ARGS()
{
	fprintf(syntatic_out, "RULE (ARGS --> ARG_LIST | EPS)\n");
	eTOKENS follow_ARGS[1] = { R_PARENTHES_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case R_PARENTHES_tok:
		T.type = void_tok;
		return T;
	case INT_tok:
	case FLOAT_tok:
	case L_PARENTHES_tok:
	case ID_tok:
		back_tok = back_token();
		T = Parse_ARG_LIST();
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{INT_tok,FLOAT_tok,L_PARENTHES_tok,ID_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_ARGS, 1);
		break;
	}
}

struct attribute Parse_CALL()
{
	fprintf(syntatic_out, "RULE (CALL --> id (ARGS))\n");
	eTOKENS follow_CALL[1] = { SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	struct table_entry *ent;
	int ans;
	switch (next_tok->kind) {
	case ID_tok:
		if(match(L_PARENTHES_tok == 0))
			back_tok = back_token();
		T = Parse_ARGS();
		if (match(R_PARENTHES_tok) == 0)
			back_tok = back_token();

		ent = find(currentTable, next_tok->lexeme);
		if (ent == NULL) {
			print_sem_error("cant find function decleration", next_tok->lineNumber, next_tok->lexeme);
			T.type = void_tok;
			return T;
		}
		else {
			ans = check_if_args_correct(ent, T);
			if(ans == 0)
				print_sem_error("parameters passed doesnt match function arguments", next_tok->lineNumber, next_tok->lexeme);
			T.value = ent->value;
			T.type = ent->type;
			return T;
		}
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_CALL, 1);
		break;
	}
}
void Parse_IF_STMT()
{
	fprintf(syntatic_out, "RULE (IF_STMT --> if (CONDITION) STMT)\n");
	eTOKENS follow_IF_STMT[1] = { SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case if_tok:
		if (match(L_PARENTHES_tok) == 0)
			back_tok = back_token();
		match(L_PARENTHES_tok);
		Parse_CONDITION();
		if (match(R_PARENTHES_tok) == 0)
			back_tok = back_token();
		Parse_STMT();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{if_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_IF_STMT, 1);
		break;
	}
}


void Parse_STMT()
{
	fprintf(syntatic_out, "RULE (STMT --> VAR = EXPR | COMP_STMT | IF_STMT | CALL | RETURN_STMT)\n");
	eTOKENS follow_STMT[2] = { SEMICOLON_tok,R_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	char* lex;
	struct table_entry *etr;
	struct attribute T,N;
	T.role = innerBlock;
	switch (next_tok->kind) {
	case ID_tok:
		lex = next_tok->lexeme;
		next_tok = next_token();
		switch (next_tok->kind) {
		case L_PARENTHES_tok:
			back_tok = back_token();
			back_tok = back_token();
			Parse_CALL();
			break;
		case L_BRACKETS_tok:
		case SEMICOLON_tok:
			back_tok = back_token();
			back_tok = back_token();
			T = Parse_VAR();
			etr = find(currentTable,T.name); // to see whether ther's a function declared;
			if (etr != NULL) {
				if (etr->size != T.count)
					print_sem_error("cannot assign into array", next_tok->lineNumber, T.name);
				if(etr->role == FuncPredef || etr->role == FunFullDef)
					print_sem_error("Variable already declared as a function", next_tok->lineNumber, T.name);
			}
			match(ASSIGN_tok);
			N = Parse_EXPR();
			if (N.type != T.type)
				print_sem_error("different types, cannot assign", next_tok->lineNumber, T.name);
			break;
		case ASSIGN_tok:
			etr = find(currentTable,lex);
			if (etr != NULL) {
				if (etr->size != 0)
					print_sem_error("cannot assign into array", next_tok->lineNumber, etr->name);
				if (etr->role == FuncPredef || etr->role == FunFullDef)
					print_sem_error("Variable already declared as a function", next_tok->lineNumber, etr->name);
			}

			T = Parse_EXPR();
			if(etr != NULL)
				if (etr->type != T.type)
					print_sem_error("different types, cannot assign", next_tok->lineNumber, etr->name);
			break;
		default:
			fprintf(syntatic_out, "Expected token of type '{L_PARENTHES_tok,L_BRACKETS_tok,SEMICOLON_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
			errorHandler(follow_STMT, 2);
			break;
		}
		break;
	case L_CURLY_tok:
		back_tok = back_token();
		Parse_COMP_STMT(T);
		break;
	case if_tok:
		back_tok = back_token();
		Parse_IF_STMT();
		break;
	case return_tok:
		back_tok = back_token();
		T = Parse_RETURN_STMT();
		//here i need to see whether the return type matches the function type;;
		break;
		default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok, L_CURLY_tok, if_tok, return_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_STMT, 2);
		break;
	}
}

void Parse_STMT_LIST_TAG()
{
	fprintf(syntatic_out, "RULE (STMT_LIST_TAG --> ; STMT STMT_LIST_TAG)\n");
	eTOKENS follow_STMT_LIST_TAG[2] = { R_CURLY_tok,SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case SEMICOLON_tok:
		Parse_STMT();
		Parse_STMT_LIST_TAG();  // check whther this is a statment that came after comp statement(comp_stmt took the "} )
		break;
	case R_CURLY_tok:
		back_tok = back_token();
		break;
	case if_tok:
	case ID_tok:
	case return_tok: // here need to check whether the return matches the function return stmt;
		back_tok = back_token();
		switch (back_tok->kind) {
		case R_CURLY_tok:
			currentTable = pop_table(currentTable);
			Parse_STMT_LIST();
			break;
		default:
			next_tok = next_token();
			Parse_STMT_LIST_TAG();
			break;
		}
		break;
	case EOF_tok:
		back_tok = back_token();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{SEMICOLON_tok, R_CURLY_tok, if_tok,ID_tok ,return_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_STMT_LIST_TAG, 1);
		break;
	}
}

void Parse_STMT_LIST()
{
	fprintf(syntatic_out, "RULE (STMT_LIST --> STMT STMT_LIST_TAG)\n");
	eTOKENS follow_STMT_LIST[1] = { R_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case ID_tok:
	case L_CURLY_tok:
	case if_tok:
	case return_tok:
		back_tok = back_token();
		Parse_STMT();
		Parse_STMT_LIST_TAG();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{ID_tok, L_CURLY_tok, if_tok,ID_tok ,return_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_STMT_LIST, 1);
		break;
	}
}

void Parse_VAR_DEC_LIST_TAG()
{
	fprintf(syntatic_out, "RULE (VAR_DEC_LIST_TAG --> VAR_DEC VAR_DEC_LIST_TAG)\n");
	//eTOKENS follow_COMP_STMT[4] = { void_tok , int_KEY_tok, float_KEY_tok, SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		Parse_VAR_DEC();
		Parse_VAR_DEC_LIST_TAG();
		break;
	default:
		back_tok = back_token();
		break;
	}
}
	void Parse_VAR_DEC_LIST()
	{
		fprintf(syntatic_out, "RULE (VAR_DEC_LIST --> VAR_DEC_LIST_TAG|EPS)\n");
		eTOKENS follow_VAR_DEC_LIST[4] = { ID_tok, R_CURLY_tok, if_tok, return_tok };
		Token *next_tok = next_token(), *back_tok;
		switch (next_tok->kind) {
		case float_KEY_tok:
		case int_KEY_tok:
			back_tok = back_token();
			Parse_VAR_DEC_LIST_TAG();
			break;
		case R_CURLY_tok:
			back_tok = back_token();
			break;
		case if_tok:
			back_tok = back_token();
			break;
		case ID_tok:
		case return_tok:
			back_tok = back_token();
			break;
		default:
			fprintf(syntatic_out, "Expected token of type '{float_KEY_tok,int_KEY_tok,ID_tok, R_CURLY_tok, if_tok,ID_tok ,return_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
			errorHandler(follow_VAR_DEC_LIST, 4);
			break;
		}
	}
void Parse_COMP_STMT(struct attribute T)
{
	fprintf(syntatic_out, "RULE (COMP_STMT --> {VAR_DEC_LIST STMT_LIST})\n");
	eTOKENS follow_COMP_STMT[4] = { void_tok , int_KEY_tok, float_KEY_tok, SEMICOLON_tok };
	Token *next_tok = next_token(), *back_tok;
	int ans;
	struct table_ptr *tab;
	struct table_entry *entr;
	switch (next_tok->kind) {
	case L_CURLY_tok:
		back_tok = back_token();
		next_tok = next_token();
		tab = make_table();
		if (T.role == FunFullDef) {
			entr = find(tab, T.name);
			if (entr == NULL) {
				insert_attribute_as_entr(T, pop_table(tab));
				insert_params_as_vars();
			}
			else
			{
				if(entr->role == FunFullDef) // already declared function with body;
					print_sem_error("function with body already declared", back_tok->lineNumber, T.name);
				ans = check_signature(entr, T); // checking wheter the signature and new info is the same;
				if (ans == 0)
					print_sem_error("function signatures doesn't match", back_tok->lineNumber, T.name);
				//updating the exsiting entry with new information;
				insert_attribute_as_entr(T, pop_table(tab));
				insert_params_as_vars();
				entr->role = FunFullDef; // changing the function declartion into full defenition
			}
		}
		Parse_VAR_DEC_LIST();
		Parse_STMT_LIST();
		next_tok = next_token();
		switch (next_tok->kind) {
		case R_CURLY_tok:
			currentTable = pop_table(tab);
			break;
		default:
			back_tok = back_token();
			break;
		}
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{L_CURLY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_COMP_STMT, 4);
		break;
	}
}
struct attribute Parse_PARAM_TAG()
{
	fprintf(syntatic_out, "RULE (PARAM --> TYPE id PARAM_TAG)\n");
	eTOKENS follow_PARAM_TAG[2] = { COMMA_tok,R_PARENTHES_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	T.count = 0;
	switch (next_tok->kind) {
	case L_BRACKETS_tok:
		T = Parse_DIM_SIZES();
		match(R_BRACKETS_tok);
		return T;
	case COMMA_tok:
		back_tok = back_token();
		return T;
	case R_PARENTHES_tok:
		back_tok = back_token();
		return T;
	default:
		back_tok = back_token();
		break;
	}
}
struct attribute Parse_PARAM()
{
	fprintf(syntatic_out, "RULE (PARAM --> TYPE id PARAM_TAG)\n");
	eTOKENS follow_PARAM[2] = { COMMA_tok,R_PARENTHES_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T,N;
	switch (next_tok->kind) {
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		N = Parse_TYPE();
		match(ID_tok);
		N.name = currentNode->tokensArray[currentIndex].lexeme;
		T = Parse_PARAM_TAG();
		N.count = T.count;
		if (N.count > 0)
			N.sizes = T.sizes;
		return N;
	default:
		fprintf(syntatic_out, "Expected token of type '{float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_PARAM, 2);
		break;
	}
}

struct attribute Parse_PARAM_LIST_TAG()
{
	fprintf(syntatic_out, "RULE (PARAM_LIST_TAG --> , PARAM PARAM_LIST_TAG)\n");
	eTOKENS follow_PARAM_LIST_TAG[1] = { R_PARENTHES_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T,N;
	switch (next_tok->kind) {
	case COMMA_tok:
		T = Parse_PARAM();
		insert_param(T);
		Parse_PARAM_LIST_TAG();
		break;
	case R_PARENTHES_tok:
		back_tok = back_token();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{COMMA_tok,R_PARENTHES_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_PARAM_LIST_TAG, 1);
		break;

	}
}

void Parse_PARAM_LIST()
{
	fprintf(syntatic_out, "RULE (PARAM_LIST --> PARAM PARAM_LIST_TAG)\n");
	eTOKENS follow_PARAM_LIST[1] = { R_PARENTHES_tok };
	Token *next_tok = next_token(), *back_tok;
	struct table_entry *ent;
	struct attribute T;
	switch (next_tok->kind) {
	case float_KEY_tok:
	case int_KEY_tok:
		make_room_for_params();
		back_tok = back_token();
		T = Parse_PARAM();
		insert_param(T);
		Parse_PARAM_LIST_TAG();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_PARAM_LIST, 1);
		break;
	}
}

struct attribute Parse_RETURNED_TYPE()
{
	fprintf(syntatic_out, "RULE (RETURNED_TYPE --> TYPE | void)\n");
	eTOKENS follow_RETURNED_TYPE[1] = { ID_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case void_tok:
		T.type = void_tok;
		return T;
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		T = Parse_TYPE();
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{void_tok,float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_RETURNED_TYPE, 1);
		break;
	}
}
void Parse_PARAMS()
{
	fprintf(syntatic_out, "RULE (PARAMS --> PARAM_LIST | EPS)\n");
	eTOKENS follow_PARAMS[1] = {R_PARENTHES_tok};
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	T.count = 0;
	switch (next_tok->kind) {
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		Parse_PARAM_LIST();
		break;
	case R_PARENTHES_tok:
		back_tok = back_token();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_PARAMS, 1);
		break;
	}
}

struct attribute Parse_FUNC_PROTOTYPE()
{
	fprintf(syntatic_out, "RULE (FUNC_PROTOTYPE --> RETURNED_TYPE id (PARAMS))\n");
	eTOKENS follow_FUNC_PROTOTYPE[1] = { L_CURLY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T,N;
	switch (next_tok->kind) {
	case void_tok:
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		N = Parse_RETURNED_TYPE();
		if (match(ID_tok) == 0)
			back_tok = back_token();
		T.name = currentNode->tokensArray[currentIndex].lexeme;
		T.role = FuncPredef;
		T.type = N.type;
		if (match(L_PARENTHES_tok) == 0)
			back_tok = back_token();
		Parse_PARAMS();
		if (match(R_PARENTHES_tok) == 0)
			back_tok = back_token();
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{void_tok,float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_FUNC_PROTOTYPE, 1);
		break;
	}
}
void Parse_FUNC_WITH_BODY()
{
	fprintf(syntatic_out, "RULE (FUNC_WITH_BODY --> FUNC_PROTOTYPE COMP STMT)\n");
	eTOKENS follow_FUNC_WITH_BODY[3] = { void_tok,int_KEY_tok,float_KEY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case void_tok:
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		T = Parse_FUNC_PROTOTYPE();
		T.role = FunFullDef;
		Parse_COMP_STMT(T);
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{void_tok,float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_FUNC_WITH_BODY, 1);
		back_tok = back_token();
		break;
	}
}
void Parse_FUNC_FULL_DEFS()
{
	fprintf(syntatic_out, "RULE (FUNC_FULL_DEFS --> FUNC_WITH_BODY FUNC_FULL_DEFS_TAG)\n");
	eTOKENS follow_FUNC_FULL_DEFS[1] = { EOF_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case void_tok:
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		Parse_FUNC_WITH_BODY();
		//match(SEMICOLON_tok);
		Parse_FUNC_FULL_DEFS_TAG();
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{void_tok,float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_FUNC_FULL_DEFS, 1);
		break;
	}
}
int Parse_FUNC_FULL_DEFS_TAG()
{
	fprintf(syntatic_out, "RULE (FUNC_FULL_DEFS_TAG --> FUNC_FULL_DEFS | EPS)\n");
	eTOKENS follow_FUNC_FULL_DEFS_TAG[1] = { EOF_tok };
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case void_tok:
	case float_KEY_tok:
	case int_KEY_tok:
		back_tok = back_token();
		Parse_FUNC_FULL_DEFS();
		break;
	case EOF_tok:
		back_tok = back_token();
		break;
	default:
		back_tok = back_token();
		break;
	}
}
int checkIfFullDef()
{
	Token *next_tok = next_token(), *back_tok;
	int i,ans = 2; // to count how many token we passed, to get back to the original location.
	for (i = 1; next_tok->kind != EOF_tok; i++) {
		if (next_tok->kind == L_CURLY_tok) { 
			ans = 1; break; 
		}
		if (next_tok->kind == SEMICOLON_tok) { 
			ans = 0; break; 
		}
		next_tok = next_token();
	}
	for (; i > 0; i--) {
		back_tok = back_token(); } // returning back to the original location
	
	if(ans < 2 & ans > 0)
		return ans;
}

void Parse_FUNC_PREDEFS_TAG()
{
	fprintf(syntatic_out, "RULE (FUNC_PREDEFS_TAG --> FUNC_PROTOTYPE ; FUNC PREDEFS_TAG | EPS)\n");
	eTOKENS follow_FUNC_PREDEFS_TAG[3] = { void_tok,int_KEY_tok,float_KEY_tok };
	Token *next_tok, *back_tok;
	struct attribute T;
	struct table_ptr *ptr;
	switch (checkIfFullDef()) {
	case 0: // this is a function decleration;
		next_tok = next_token();
		switch (next_tok->kind) {
		case void_tok:
		case float_KEY_tok:
		case int_KEY_tok:
			back_tok = back_token();
			T = Parse_FUNC_PROTOTYPE();
			T.role = FuncPredef;
			insert_attribute_as_entr(T,currentTable);
			match(SEMICOLON_tok);
			Parse_FUNC_PREDEFS_TAG();
			break;
		break;
		}
	case 1://this is a function with full defenition;;
		//next_tok = next_token();
		break;
	}
}

void Parse_FUNC_PREDEFS()
{
	fprintf(syntatic_out, "RULE (FUNC_PREDEFS --> FUNC PROTOTYPE FUNC_PREDEFS_TAG)\n");
	eTOKENS follow_FUNC_PREDEFS[3] = { void_tok,int_KEY_tok,float_KEY_tok };
	Token *next_tok = next_token(), *back_tok = back_token();
	struct attribute T;
	struct table_ptr *ptr = NULL;
	switch (checkIfFullDef()) {
	case 0: // this is a function decleration;
		next_tok = next_token();
		switch (next_tok->kind) {
		case void_tok:
		case float_KEY_tok:
		case int_KEY_tok:
			back_tok = back_token();
			T = Parse_FUNC_PROTOTYPE();
			T.role = FuncPredef;
			insert_attribute_as_entr(T,currentTable);
			match(SEMICOLON_tok);
			Parse_FUNC_PREDEFS_TAG();
			break;
		}
	case 1://this is a function with full defenition;;
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{void_tok,float_KEY_tok,int_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_FUNC_PREDEFS, 3);
		break;
	}
}
struct attribute Parse_DIM_SIZES()
{
	fprintf(syntatic_out, "RULE (Parse_DIM_SIZES --> int_num DIM_SIZES_TAG)\n");
	eTOKENS follow_DIM_SIZES[2] = { R_BRACKETS_tok,SEMICOLON_tok };
	struct attribute T;
	T.count = 0;
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case INT_tok:
		T = Parse_DIM_SIZES_TAG();
		T.count++;
		T = allocate_new_sizes(next_tok->lexeme, T);
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{INT_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_DIM_SIZES, 2);
		break;
	}
}
struct attribute Parse_DIM_SIZES_TAG()
{
	fprintf(syntatic_out, "RULE (Parse_DIM_SIZES_TAG -->, DIM_SIZES | EPS)\n");
	eTOKENS follow_DIM_SIZES_TAG[2] = { SEMICOLON_tok, R_BRACKETS_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case COMMA_tok:
		T = Parse_DIM_SIZES();
		return T;
	case R_BRACKETS_tok:
		T.count = 0;
		T.sizes = calloc(1, sizeof(int));
		back_tok = back_token();
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{COMMA_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_DIM_SIZES_TAG, 2);
		break;
	}
}


struct attribute Parse_TYPE()
{
	fprintf(syntatic_out, "RULE (Parse_TYPE --> int | float)\n");
	eTOKENS follow_TYPE[1] = { ID_tok };
	struct attribute T, N;
	Token *next_tok = next_token(), *back_tok;
	switch (next_tok->kind) {
	case int_KEY_tok:
		T.type = INT_tok;
		return T;
	case float_KEY_tok:
		T.type = FLOAT_tok;
		return T;
	default:
		fprintf(syntatic_out, "Expected token of type '{int_KEY_tok,float_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_TYPE, 1);
		break;
	}
}

struct attribute Parse_VAR_DEC_TAG()
{
	fprintf(syntatic_out, "RULE (Parse_VAR_DEC_TAG --> ID | ID[DIM_SIZES])\n");
	eTOKENS follow_VAR_DEC_TAG[3] = { int_KEY_tok,float_KEY_tok };
	Token *next_tok = next_token(), *back_tok;
	struct attribute T;
	switch (next_tok->kind) {
	case SEMICOLON_tok:
		back_tok = back_token();
		T.count = 0;
		return T;
	case L_BRACKETS_tok:
		T = Parse_DIM_SIZES();
		if (match(R_BRACKETS_tok) == 0)
			back_tok = back_token();
		return T;
	default:
		back_tok = back_token();
		break;
	}
}
void Parse_VAR_DEC()
{
	fprintf(syntatic_out, "RULE (Parse_VAR_DEC --> TYPE VAR_DEC_TAG)\n");
	eTOKENS follow_VAR_DEC[2] = {int_KEY_tok,float_KEY_tok};
	Token *next_tok, *back_tok;
	struct attribute T, N;
	struct table_entry *tab = NULL;
	next_tok = next_token();
	switch (next_tok->kind) {
	case float_KEY_tok:
	case int_KEY_tok:
		//notice here that there are no multy declerations like int a,b;
		back_tok = back_token();
		T = Parse_TYPE();
		N.type = T.type;
		if (match(ID_tok)) {
			tab = find(currentTable, currentNode->tokensArray[currentIndex].lexeme);
			if(tab != NULL)
				if (tab->role == FuncPredef || tab->role == FunFullDef)
					print_sem_error("Variable already declared as a function", next_tok->lineNumber, tab->name);
			tab = lookup(currentTable, currentNode->tokensArray[currentIndex].lexeme);
			if (tab != NULL) {
				print_sem_error("variable ID is already declared",next_tok->lineNumber, currentNode->tokensArray[currentIndex].lexeme);
				T = Parse_VAR_DEC_TAG();
				match(SEMICOLON_tok);
				break;
			}
			else {
				tab = insert(currentNode->tokensArray[currentIndex].lexeme);
				set_id_type(tab, N.type);
			}
			T = Parse_VAR_DEC_TAG();
			if (T.count != 0)
				set_arr_size(tab, T);
			match(SEMICOLON_tok);
		}
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{int_KEY_tok,float_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_VAR_DEC, 2);
		break;
	}
}

int checkIfFuncDef() {
	int i,ans = 2;
	Token *next_tok = next_token(), *back_tok = back_token();
	for (i = 0; i < 3 && next_tok->kind != EOF_tok; i++) {
		next_tok = next_token();
	}
	if (next_tok->kind == L_PARENTHES_tok || next_tok->kind == EOF_tok)
		ans = 1;
	if (next_tok->kind == SEMICOLON_tok)
		ans = 0;
	if (next_tok->kind == L_BRACKETS_tok)
		ans = 0;
	for (i; i > 0; i--)
		back_tok = back_token();

	if (ans > 1) { 
		printf("couldnt identify whether this is a var dec or a func dec"); return ans;
	}
	
	return ans;
}

void Parse_GLOBAL_VARS_TAG()
{
	fprintf(syntatic_out, "RULE (Parse_GLOBAL_VARS_TAG --> VAR_DEC GLOBAL_VARS_TAG | EPS)\n");
	eTOKENS follow_GLOBAL_VARS_TAG[3] = { void_tok,int_KEY_tok,float_KEY_tok };
	Token *next_tok, *back_tok;
	switch (checkIfFuncDef()) {
	case 0: // this is a function decleration;
		Parse_VAR_DEC();
		Parse_GLOBAL_VARS_TAG();
		break;
	case 1://this is a function with full defenition;;
		break;
	default:
		back_tok = back_token();
		break;
	}
}


int Parse_GLOBAL_VARS()
{
	fprintf(syntatic_out, "RULE (Parse_GLOBAL_VARS --> VAR_DEC GLOBAL_VARS_TAG)\n");
	eTOKENS follow_GLOBAL_VARS[3] = {void_tok,int_KEY_tok,float_KEY_tok};
	Token *next_tok = next_token(), *back_tok = back_token();
	switch (checkIfFuncDef()) {
	case 0: // this is a function decleration;
		Parse_VAR_DEC();
		Parse_GLOBAL_VARS_TAG();
		break;
	case 1://this is a function with full defenition;;
		break;
	default:
		fprintf(syntatic_out, "Expected token of type '{int_KEY_tok,float_KEY_tok}' at line: '{%d}' \nActual token of type '{%s}', lexeme: {%s}\n", next_tok->lineNumber, kindToStringConverter(next_tok->kind), next_tok->lexeme);
		errorHandler(follow_GLOBAL_VARS, 3);
		break;
	}
}

void Parse_PROG()
{
	fprintf(syntatic_out, "RULE (Parse_PROG --> GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
	eTOKENS follow_PROG[1] = {EOF_tok};
 	globalTable = make_table();
	Parse_GLOBAL_VARS();
	Parse_FUNC_PREDEFS();
	Parse_FUNC_FULL_DEFS();
	check_if_func_predef_exists();
	//currentTable = pop_table(globalTable);
}
void Parser()
{
	Parse_PROG();
	match(EOF_tok);
}








