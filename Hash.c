#pragma once
#ifndef HASHTABLE
#define HASHTABLE

#define tb_en_size 15
#define max_sizes 5
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "Token.h"

struct table_ptr *currentTable,*globalTable;
struct table_entry *dummyItem;
typedef enum { Variable, error_type, FuncPredef, FunFullDef,innerBlock,ret_var } var_type;

typedef struct func_params {
	eTOKENS type;
	char* name;
	int *listOfSizes;
	int size;
	float value;
}func_params;

struct attribute {
	char* name;
	float value;
	var_type role;
	eTOKENS type; // integer/float or for function - return type.
	int *sizes;
	int count;
	struct func_params **parameters;
};
inline void print_sem_error(char* err, int line, char* lex) {
	fprintf(semantic_out, "%s lexeme : %s, in line %d \n", err,lex, line); //semantic_out,
}


typedef struct table_entry {
	char *name;
	float value;
	var_type role;
	eTOKENS type;
	int *listOfSizes;
	int size;
	struct func_params **parameters;
}table_entry;

typedef struct table_ptr {
	struct table_entry *arr[tb_en_size];
	struct table_ptr *father;
}table_ptr;

inline void printTable() {
	int i, count;
	for (i = 0; i < tb_en_size; i++) {
		if (currentTable->arr[i] != NULL) {
			printf("%s ", currentTable->arr[i]->name);
			//if (currentTable->arr[i]->listOfSizes != NULL)
			//for (int j = 0; j < currentTable->arr[i]->size; j++)
			//	printf("%d ", currentTable->arr[i]->listOfSizes[j]);
			printf("\t");
		}
	}
	printf("\n");
}

inline struct table_ptr * pop_table(struct table_ptr *current_table) {
	//printTable();
	return (current_table->father);
}

inline int hashCode(char* name) {
	int sum = 0, j, i;
	for (j = 0; name[j] != '\0'; ++j);
	for (i = 0; i < j; i++)
		sum += name[i] - 'a';
	sum = sum % tb_en_size;
	if (sum < 0)
		sum *= -1;
	if(sum > tb_en_size)
		sum = sum % tb_en_size;
	return sum;
}

inline struct table_ptr *make_table() {
	struct table_ptr *tab = (struct table_ptr*)malloc(sizeof(struct table_ptr));
	int i;
	for (i = 0; i < tb_en_size; i++) {
		tab->arr[i] = NULL;
	}
	tab->father = currentTable;
	currentTable = tab;
	return tab;
}


inline struct table_entry *lookup(struct table_ptr *tab_ptr, char* name) {
	//get the hash 
	int hashIndex = hashCode(name);

	//move in array until an empty 
	while (tab_ptr->arr[hashIndex] != NULL) {
		if (strcmp(tab_ptr->arr[hashIndex]->name, name) == 0)
			return tab_ptr->arr[hashIndex];
		//go to next cell
		++hashIndex;
		//wrap around the table
		hashIndex %= tb_en_size;
	}
	return NULL;
}

inline struct table_entry* insert(char* id_name) {
	struct table_entry *item = (struct table_entry*)malloc(sizeof(struct table_entry));
	item->name = id_name;
	item->size = 0;
	item->parameters = NULL;
	//for (int i = 0; i < max_sizes; i++) { // making all the sizes NULL;
	//	item->listOfSizes[i] = NULL;
	//}
	//get the hash 
	int hashIndex = hashCode(id_name);

	//move in array until an empty or deleted cell  && currentTable->arr[hashIndex]->tab.name != "LIST END"
	while (currentTable->arr[hashIndex] != NULL) {
		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= tb_en_size;
	}
	currentTable->arr[hashIndex] = item;
	return currentTable->arr[hashIndex];
}

inline struct table_entry *deletee(struct table_entry *item) {
	char* key = item->name;
	//get the hash 
	int hashIndex = hashCode(key);

	//move in array until an empty
	while (currentTable->arr[hashIndex] != NULL) {

		if (currentTable->arr[hashIndex]->name == key) {
			struct table_entry *temp = currentTable->arr[hashIndex];

			//assign a dummy item at deleted position
			currentTable->arr[hashIndex] = dummyItem;
			return temp;
		}

		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= tb_en_size;
	}

	return NULL;
}

//inline char* get_last_ID() {
//	Token *next_tok, *back_tok = back_token();
//	char* temp = NULL;
//	int i; // to count how many token we passed, to get back to the original location.
//	for (i = 1; back_tok->kind != EOF_tok; i++) {
//		if (back_tok->kind == ID_tok) {
//			temp = back_tok->lexeme;
//			break;
//		}
//		back_tok = back_token();
//	}
//	for (; i > 0; i--) {
//		next_tok = next_token();
//	} // returning back to the original location
//	return temp;
//}


inline struct table_entry *find(struct table_ptr *current_table, char* id_name) {
	/* hierarchical search is implemented as a series of local searches */
	struct table_ptr *tab = current_table;
	struct table_entry *id_entry;
	while (tab->arr != NULL)
	{
		id_entry = lookup(tab, id_name);
		if (id_entry != NULL)
			return (id_entry);
		else
			tab = pop_table(tab);
	}
	//printf("ERROR: undeclared identifier %s \n", id_name);
	return NULL;
}

inline void set_id_type(struct table_entry *id_entry, eTOKENS id_type) {
	id_entry->type = id_type;
}

inline eTOKENS get_id_type(struct table_entry *id_entry) {
	return id_entry->type;
}

inline void set_arr_size(struct table_entry *id_entry, struct attribute T) {
	id_entry->listOfSizes = calloc(T.count,sizeof(int));
	int j = 0;
	//int total_size = sizeof(size) / 4;
	for (int i = T.count-1; i >= 0 ; i--)
		id_entry->listOfSizes[j++] = T.sizes[i];
	id_entry->size = T.count;
	//free(T.sizes);
	//id_entry->listOfSizes = &size;
	//free(size);
}

inline int get_arr_size(struct table_entry *id_entry) {
	return id_entry->listOfSizes;
}

inline struct attribute allocate_new_sizes(char* num, struct attribute T) {
	int* temp = calloc(T.count, sizeof(int)),i;
	for (i = 0; i < T.count-1; i++)
		temp[i] = T.sizes[i];
	temp[i] = atoi(num);
	//free(T.sizes);
	T.sizes = temp;
	return T;
}

inline void insert_attribute_as_entr(struct attribute T, struct table_ptr *tab) {
	struct table_entry *entr;
	struct table_ptr *temp = NULL;
	entr = find(currentTable, T.name);
	if (entr == NULL) {
		if (T.role == FunFullDef) {
			temp = currentTable;
			currentTable = tab;
		}
		entr = insert(T.name);
		if (T.role == FunFullDef)
			currentTable = temp;
	}

	entr->role = T.role;
	if (dummyItem == NULL)
		entr->parameters = NULL;
	else {
		entr->parameters = (struct func_params*)malloc(sizeof(struct func_params)*tb_en_size);
		entr->size = 0;
		for (int j = 0; j < tb_en_size; j++) {
			entr->parameters[j] = (struct func_params*)malloc(sizeof(struct func_params));
			entr->parameters[j]->size = dummyItem->parameters[j]->size;
			entr->parameters[j]->name = dummyItem->parameters[j]->name;
			entr->parameters[j]->type = dummyItem->parameters[j]->type;
			entr->parameters[j]->listOfSizes = malloc(sizeof(int)*entr->parameters[j]->size);
			for (int k = 0; k < entr->parameters[j]->size; k++) {
				entr->parameters[j]->listOfSizes[k] = dummyItem->parameters[j]->listOfSizes[k];
			}
			if (dummyItem->parameters[j]->name != NULL)
				entr->size += 1;
			//free(dummyItem->parameters[j]);
		}
	}
	entr->type = T.type;

}


inline int check_signature(struct table_entry *entr, struct attribute T) {
	if (entr->type != T.type) // return type isnt the same
		return 0;
	for (int i = 0; i < tb_en_size; i++) { // checking whether all params are defined;
		if (dummyItem->parameters[i]->name == NULL && entr->parameters[i]->name == NULL)
			return 1;
		if (dummyItem->parameters[i]->name == NULL && entr->parameters[i]->name != NULL)
			return 0;
		if (dummyItem->parameters[i]->name != NULL && entr->parameters[i]->name == NULL)
			return 0;
		if (dummyItem->parameters[i]->name != NULL && entr->parameters[i]->name != NULL) {
			if (dummyItem->parameters[i]->type == entr->parameters[i]->type)
			{
				for (int j = 0; j < dummyItem->parameters[i]->size && j < entr->parameters[i]->size; j++) {
					if (dummyItem->parameters[i]->listOfSizes[j] != entr->parameters[i]->listOfSizes[j])
						return 0;
				}
			}
			else
				return 0;
		}
	}
}

inline void insert_params_as_vars() {
	struct table_entry *entr;
	int temp;
	if (dummyItem != NULL)
		for (int i = 0; dummyItem->parameters[i]->name != NULL ; i++) {
			entr = insert(dummyItem->parameters[i]->name);
			entr->role = Variable;
			entr->type = dummyItem->parameters[i]->type;
			entr->size = dummyItem->parameters[i]->size;
			temp = entr->size -1;
			entr->listOfSizes = malloc(entr->size * sizeof(int));
			for (int j = 0; j < entr->size; j++) {
				entr->listOfSizes[j] = dummyItem->parameters[i]->listOfSizes[temp];
				temp -= 1;
			}
		}
	free(dummyItem);
	dummyItem = NULL;
}

inline void make_room_for_params() {
	//struct table_entry *item;
	dummyItem = (struct table_entry*)malloc(sizeof(struct table_entry));
	dummyItem->parameters = (struct func_params*)malloc(sizeof(struct func_params)*tb_en_size);
	for (int i = 0; i < tb_en_size; i++) {
		dummyItem->parameters[i] = (struct func_params*)malloc(sizeof(struct func_params));
		dummyItem->parameters[i]->size = 0;
		dummyItem->parameters[i]->listOfSizes = NULL;
		dummyItem->parameters[i]->name = NULL;
	}
	//dummyItem = item;
}

inline void insert_param(struct attribute T) {
	for (int i = 0; i < tb_en_size; i++) {
		if (dummyItem->parameters[i]->name != NULL && strcmp(dummyItem->parameters[i]->name, T.name) == 0)
			print_sem_error("parameter name already defined", currentNode->tokensArray[currentIndex].lineNumber, T.name);
		if (dummyItem->parameters[i]->name == NULL) {
			dummyItem->parameters[i]->name = T.name;
			dummyItem->parameters[i]->type = T.type;
			dummyItem->parameters[i]->size = T.count;
			dummyItem->parameters[i]->listOfSizes = T.sizes;
			break;
		}
	}
}

//int check_fun_call_params(struct table_entry *entr,struct attribute T) // this function is the same as check_signature but without the return param;
//{ 
//	for (int i = 0; i < tb_en_size; i++) { // checking whether all params are defined;
//		if (dummyItem->parameters[i]->name == NULL && entr->parameters[i]->name == NULL)
//			return 1;
//		if (dummyItem->parameters[i]->name != NULL && entr->parameters[i]->name != NULL) {
//			if (dummyItem->parameters[i]->type == entr->parameters[i]->type)
//			{
//				for (int j = 0; j < dummyItem->parameters[i]->size && j < entr->parameters[i]->size; j++) {
//					if (dummyItem->parameters[i]->listOfSizes[j] != entr->parameters[i]->listOfSizes[j])
//						return 0;
//				}
//			}
//			else
//				return 0;
//		}
//	}
//}

inline struct attribute insert_expr(struct attribute T, struct attribute N) { // add N size to T
	struct attribute B;
	if (N.count == 0) {
		B.sizes = malloc((1) * sizeof(int));
		B.count = 1;
		B.sizes[0] = T.value;
		return B;
	}
	else {
		B.sizes = malloc((N.count + 1) * sizeof(int));
		B.count = N.count + 1;
		B.sizes[0] = T.value;
		for (int i = 0; i < N.count; i++)
			B.sizes[i+1] = N.sizes[i];
		if (N.count == 0)
			B.type = T.type;
		else {
			if (N.type == T.type)
				B.type = N.type;
			if ((N.type == FLOAT_tok && T.type == INT_tok) || (T.type == FLOAT_tok && N.type == INT_tok))
				B.type = FLOAT_tok;
			if (N.type == void_tok || T.type == void_tok)
				B.type = void_tok;
		}
	}
	return B;
}

inline int check_if_array_correct(struct table_entry *ent, struct attribute T) { // 0 means num of indexes doesnt match, 1 means there are indexes larger then expected, 2  indexes are good and smaller
	if (ent->size != T.count)
		return 0;
	for (int i = 0; i < T.count; i++) {
		if (ent->listOfSizes[i] < T.sizes[i])
			return 1;
	}
	return 2;
}

inline int check_if_args_correct(struct table_entry *ent, struct attribute T) { // 0 means num of indexes doesnt match, 1 means there are indexes larger then expected, 2  indexes are good and smaller
	if (ent->size != T.count)
		return 0;
	for (int i = 0; i < T.count; i++) {
		if (ent->parameters[i]->type == FLOAT_tok)
			continue;
		if (ent->parameters[i]->type != T.parameters[i]->type)
			return 0;
	}
	return 1;
}
// this is wrong - from parse call function !!!!
inline struct attribute insert_args(struct attribute T, struct attribute N) { // add T to N
	struct attribute B;
	B.parameters = (struct func_params*)malloc(sizeof(struct func_params)*tb_en_size);
	B.parameters[0] = (struct func_params*)malloc(sizeof(struct func_params));
	B.count = N.count + 1;
	B.parameters[0]->name = T.name;
	B.parameters[0]->value = T.value;
	B.parameters[0]->type = T.type;
	B.parameters[0]->size = T.count;						// copying T into parameter list of B;
	if (T.count != 0) { // at first T.count is for the length of the sizes of the array;
		B.parameters[0]->listOfSizes = malloc(sizeof(int)*T.count);
		for (int i = 0; i < T.count; i++) {
			B.parameters[0]->listOfSizes[i] = T.sizes[i];
		}
	}
	if (N.count == 0)
		return B;// this N.count is for the number of arguments in the func_params;

	for (int i = 0; i < N.count; i++) {
		//B.parameters[i]->name = N.parameters[i]->name;
		B.parameters[i + 1] = (struct func_params*)malloc(sizeof(struct func_params));
		B.parameters[i + 1]->value = N.parameters[i]->value;
		B.parameters[i + 1]->type = N.parameters[i]->type;
		B.parameters[i + 1]->size = N.parameters[i]->size;
		if (B.parameters[i + 1]->size != 0) {
			B.parameters[i + 1]->listOfSizes = malloc(sizeof(int)*B.parameters[i]->size);
			for (int j = 0; j < B.parameters[i]->size; j++) {
				B.parameters[i + 1]->listOfSizes[j] = N.parameters[i + 1]->listOfSizes[j];
			}
		}
	}
	return B;
}

inline void check_if_func_predef_exists() {
	while (currentTable != NULL) {
		for (int i = 0; i < tb_en_size; i++) {
			if (currentTable->arr[i] != NULL) {
				if (currentTable->arr[i]->role == FuncPredef)
					print_sem_error("function decleration has no full defenition", 0, currentTable->arr[i]->name);
			}
		}
		currentTable = pop_table(currentTable);
	}
}

#endif