%{

FILE *syntatic_out,*semantic_out;
errno_t err;
int yyline = 0;
#include "stdio.h"
#include "stdlib.h"
#include "Token.h"
#include "Parser.h"
%}
DIGIT [0-9]
LOW_ALPHA [a-z]
LOW_ALPHA_NUM [a-z0-9]
LARGE_ALPHA [A-Z]
LARGE_ALPHA_NUM [A-Z0-9]
ALPHA [a-zA-Z]
ALPHA_NUM [a-zA-Z0-9]

COMMA ","
COLON ":"
SEMICOLON ";"
L_PARENTHES "("
R_PARENTHES ")"
L_BRACKETS "["
R_BRACKETS "]"
L_CURLY "{"
R_CURLY "}"
EQUAL "=="
N_EQUAL "!="
SMALLER "<"
SMALLER_EQUAL "<="
BIGGER ">"
BIGGER_EQUAL ">="
MULTIPLE "*"
ADDITION "+"
ASSIGN "="

IF	"if"
FLOAT "float"
INT "int"
RETURN "return"
VOID "void"

%x comment1 comment2
%%


"/*"  {BEGIN(comment1);}
<comment1>[^*\n]*
<comment1>"*"+[^*/\n]*
<comment1>\n yyline++;
<comment1>"*"+"/" {BEGIN(INITIAL);}

"//"  {BEGIN(comment2);}
<comment2>"\n" {yyline++; BEGIN(INITIAL);}

{EQUAL} {create_and_store_token(EQUAL_tok,yytext,yyline); printAcceptedTok(EQUAL_tok,yyline,yytext);return 1;}

\t 
\n   {yyline++;}
{INT} {create_and_store_token(int_KEY_tok,yytext,yyline); printAcceptedTok(int_KEY_tok,yyline,yytext);return 1;}
{RETURN} {create_and_store_token(return_tok,yytext,yyline); printAcceptedTok(return_tok,yyline,yytext);return 1;}
{VOID} {create_and_store_token(void_tok,yytext,yyline); printAcceptedTok(void_tok,yyline,yytext);return 1;}
{IF} {create_and_store_token(if_tok,yytext,yyline); printAcceptedTok(if_tok,yyline,yytext);return 1;}
{FLOAT} {create_and_store_token(float_KEY_tok,yytext,yyline); printAcceptedTok(float_KEY_tok,yyline,yytext);return 1;}

{LOW_ALPHA}{ALPHA_NUM}*(("_")?{ALPHA_NUM}+)* {create_and_store_token(ID_tok,yytext,yyline); printAcceptedTok(ID_tok,yyline,yytext);return 1;}
" "

{MULTIPLE} {create_and_store_token(MULTIPLE_tok,yytext,yyline); printAcceptedTok(MULTIPLE_tok,yyline,yytext);return 1;}
{ADDITION} {create_and_store_token(ADDITION_tok,yytext,yyline); printAcceptedTok(ADDITION_tok,yyline,yytext);return 1;}
{ASSIGN} {create_and_store_token(ASSIGN_tok,yytext,yyline); printAcceptedTok(ASSIGN_tok,yyline,yytext);return 1;}

("-"|"+")?[1-9][0-9]*"."[1-9][0-9]* {create_and_store_token(FLOAT_tok,yytext,yyline); printAcceptedTok(FLOAT_tok,yyline,yytext);return 1;}
("+"|"-")?("0"|[1-9]{DIGIT}*)"."{DIGIT}*("e+"|"e-"|"e")?[1-9]{DIGIT}+ {create_and_store_token(FLOAT_tok,yytext,yyline); printAcceptedTok(FLOAT_tok,yyline,yytext);return 1;}
   
(0|[1-9]{DIGIT}*) {create_and_store_token(INT_tok,yytext,yyline); printAcceptedTok(INT_tok,yyline,yytext);return 1;}   

{COMMA} {create_and_store_token(COMMA_tok,yytext,yyline); printAcceptedTok(COMMA_tok,yyline,yytext);return 1;}
{COLON} {create_and_store_token(COLON_tok,yytext,yyline); printAcceptedTok(COLON_tok,yyline,yytext);return 1;}
{SEMICOLON} {create_and_store_token(SEMICOLON_tok,yytext,yyline); printAcceptedTok(SEMICOLON_tok,yyline,yytext);return 1;}
{L_PARENTHES} {create_and_store_token(L_PARENTHES_tok,yytext,yyline); printAcceptedTok(L_PARENTHES_tok,yyline,yytext);return 1;}
{R_PARENTHES} {create_and_store_token(R_PARENTHES_tok,yytext,yyline); printAcceptedTok(R_PARENTHES_tok,yyline,yytext);return 1;}
{L_BRACKETS} {create_and_store_token(L_BRACKETS_tok,yytext,yyline); printAcceptedTok(L_BRACKETS_tok,yyline,yytext);return 1;}
{R_BRACKETS} {create_and_store_token(R_BRACKETS_tok,yytext,yyline); printAcceptedTok(R_BRACKETS_tok,yyline,yytext);return 1;}
{L_CURLY} {create_and_store_token(L_CURLY_tok,yytext,yyline); printAcceptedTok(L_CURLY_tok,yyline,yytext);return 1;}
{R_CURLY} {create_and_store_token(R_CURLY_tok,yytext,yyline); printAcceptedTok(R_CURLY_tok,yyline,yytext);return 1;}


{N_EQUAL} {create_and_store_token(N_EQUAL_tok,yytext,yyline); printAcceptedTok(N_EQUAL_tok,yyline,yytext);return 1;}
{SMALLER} {create_and_store_token(SMALLER_tok,yytext,yyline); printAcceptedTok(SMALLER_tok,yyline,yytext);return 1;}
{SMALLER_EQUAL} {create_and_store_token(SMALLER_EQUAL_tok,yytext,yyline); printAcceptedTok(SMALLER_EQUAL_tok,yyline,yytext);return 1;}
{BIGGER} {create_and_store_token(BIGGER_tok,yytext,yyline); printAcceptedTok(BIGGER_tok,yyline,yytext); return 1;}
{BIGGER_EQUAL} {create_and_store_token(BIGGER_EQUAL_tok,yytext,yyline); printAcceptedTok(EOF_tok,yyline,yytext);return 1;}
. {printError(yyline,yytext);return 1;}
<<EOF>> {create_and_store_token(EOF_tok,"", yyline);printAcceptedTok(EOF_tok,yyline,yytext);yyline = 1;  return 0;}

%%
int main(int argc, char** argv)
{
	err = fopen_s(&yyin, "C:\\temp\\test1.txt","r");
	if(err != 0){printf("file not found");}

	err = fopen_s(&yyout, "C:\\temp\\test1_311500227_315961979_207632977_lex.txt","w");
	if(err != 0){printf("file not found");}

	err = fopen_s(&syntatic_out, "C:\\temp\\test1_311500227_315961979_207632977_syntatic.txt", "w");
	if (err != 0) { printf("file not found"); }

	err = fopen_s(&semantic_out, "C:\\temp\\test1_311500227_315961979_207632977_semantic.txt", "w");
	if (err != 0) { printf("file not found"); }	

	yyline = 1;
	Parser();

	fflush(yyin);
	fclose(yyin);
	fflush(yyout);
	fclose(yyout);
	fflush(syntatic_out);
	fclose(syntatic_out);
	//deleteArraysAndLinkedList();
	err = fopen_s(&yyin, "C:\\temp\\test2.txt", "r");
	if (err != 0) { printf("file not found"); }
	yyrestart(yyin);

	err = fopen_s(&yyout, "C:\\temp\\test2_311500227_315961979_207632977_lex.txt", "w");
	if (err != 0) { printf("file not found"); }

	err = fopen_s(&syntatic_out, "C:\\temp\\test2_311500227_315961979_207632977_syntatic.txt", "w");
	if (err != 0) { printf("file not found"); }

	err = fopen_s(&semantic_out, "C:\\temp\\test2_311500227_315961979_207632977_semantic.txt", "w");
	if (err != 0) { printf("file not found"); }	

	yyline = 1;
	Parser();

	fclose(yyin);
	fclose(yyout);
	fclose(syntatic_out);
	//deleteArraysAndLinkedList();

return 1;
}
int yywrap(){return 1;}