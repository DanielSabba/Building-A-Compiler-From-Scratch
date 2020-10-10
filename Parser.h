#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Parser.h"
#include "Hash.c"

void Parser();
int match(eTOKENS t);
void errorHandler(eTOKENS* follow_tokens_array, int size);
FILE *syntatic_out,*semantic_out;
int checkIfFullDef();
int checkIfFuncDef();
void Parse_PROG();
int Parse_GLOBAL_VARS();
void Parse_GLOBAL_VARS_TAG();
void Parse_FUNC_PREDEFS();
void Parse_FUNC_PREDEFS_TAG();
void Parse_FUNC_FULL_DEFS();
int Parse_FUNC_FULL_DEFS_TAG();
void Parse_VAR_DEC();
struct attribute Parse_VAR_DEC_TAG();
struct attribute Parse_TYPE();
struct attribute Parse_DIM_SIZES();
struct attribute Parse_DIM_SIZES_TAG();
struct attribute Parse_FUNC_PROTOTYPE();
void Parse_PARAMS();
struct attribute Parse_PARAM();
struct attribute Parse_PARAM_TAG();
void Parse_FUNC_WITH_BODY();
void Parse_COMP_STMT(struct attribute T);
void Parse_PARAM_LIST();
struct attribute Parse_PARAM_LIST_TAG();
void Parse_VAR_DEC_LIST();
void Parse_VAR_DEC_LIST_TAG();
void Parse_STMT_LIST();
void Parse_STMT_LIST_TAG();
void Parse_STMT();
struct attribute Parse_VAR();
struct attribute Parse_VAR_TAG();
struct attribute Parse_EXPR();
struct attribute Parse_EXPR_TAG();
void Parse_IF_STMT();
struct attribute Parse_CALL();
struct attribute Parse_RETURN_STMT();
struct attribute Parse_RETURN_STMT_TAG();
void Parse_rel_op();
void Parse_CONDITION();
struct attribute Parse_ARGS();
struct attribute Parse_ARG_LIST();
struct attribute Parse_ARG_LIST_TAG();
struct attribute Parse_EXPR_LIST();
struct attribute Parse_EXPR_LIST_TAG();
struct attribute Parse_TERM();
struct attribute Parse_TERM_TAG();
struct attribute Parse_FACTOR();
#endif