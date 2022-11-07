#pragma once
#include "tree/tree.h"
#include "stack/stack.h"
#include "filework/filework.h"
#include "log/LOG.h"
#include <stdarg.h>

const size_t TEXT_SIZE = 100;

enum AkinatorStatus
{
	STATUS_OK         = 0 << 0,
	NULL_DATABASE     = 1 << 0,
	NULL_TREE         = 1 << 1,
	BAD_NODE          = 1 << 2,
	WRONG_DATA        = 1 << 3, 
	NULL_NODE         = 1 << 4,
	NON_EXISTABLE_OBJ = 1 << 5,
};

Tree_t* DataDownload(void);
int MainMenu(void);
int MakeNodeValue(char* value, const char* data);
int ReadTree(TEXT* database, Tree_t* tree);
int DataUpload(Tree_t* tree);
int GuessCharacter(Tree_t* tree);
int AddInBase(Tree_t* tree, TreeNode_t* curNode);
int FindObject(char* object, Tree_t* tree, TreeNode_t* node, size_t* count, Stack_t* routeStk);
int PrintDefinition(Stack_t* routeStk);
int GetDefinition(Tree_t* tree);
int GetComparison(Tree_t* tree);
Stack_t* ReverseStack(Stack_t* stk, Stack_t* reverseStk);
int ReadName(char* name);
int MakeRoute(char* object, Tree_t* tree, TreeNode_t* node, Stack_t* reverse);
int RouteCmp(Stack_t* route1, Stack_t* route2, char* object1, char* object2);

void AkinPrint(char text[TEXT_SIZE], ...);
