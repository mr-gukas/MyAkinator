#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
//#include "log/LOG.h"

#define LOG_MODE

#ifdef LOG_MODE
    #define ASSERT(condition)                                       \
        if (!(condition)){                                           \
            fprintf(stderr, "Error in %s:\n"                          \
                            "FILE: %s\n"                               \
                            "LINE: %d\n"                                \
                            "FUNCTION: %s\n",                            \
                   #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);  \
            abort();}

    #define ASSERT_OK(tree)                                                         \
        if (TreeVerify(tree) != TREE_IS_OK && TreeIsEmpty(tree) != TREE_IS_EMPTY)\
        {                                                                             \
            TreeDump(tree)                                                         \
            ASSERT(0 && "Crashed tree")                                                 \
        }

#else
    #define ASSERT(condition) ;
    #define ASSERT_OK(tree)   ;
#endif

typedef char Val_t;

enum InsMode
{
	LEFT  = -1,
	RIGHT =  1,
};


const size_t DEAD_SIZE      = 0xDEADBEAF;
const size_t STR_MAX_SIZE   = 50;


enum TreeStatus
{
    TREE_IS_OK              = 0 << 0,
    TREE_NULL_PTR           = 1 << 0,
    TREE_DATA_NULL_PTR      = 1 << 1,
    CANNOT_ALLOCATE_MEMORY  = 1 << 2,
    TREE_IS_DESTRUCTED      = 1 << 3,
    TREE_IS_EMPTY           = 1 << 4,
    TREE_ROOT_IS_NULL       = 1 << 5,
    BAD_PRINT_FILE          = 1 << 6,
    TREE_UB                 = 1 << 7,
    NODE_NULL_PTR           = 1 << 8,
    BAD_INS_MODE            = 1 << 9,
    BAD_REMOVE              = 1 << 10,
    TREE_RUINED             = 1 << 18,
};

struct TreeNode_t
{
    Val_t  value[STR_MAX_SIZE];
	TreeNode_t*			left;
	TreeNode_t*			right;
};

struct Tree_t
{
	TreeNode_t* root;
	size_t      size;
	int  status;
};

void MakePngName(char* name, char num);
void TreeDump(Tree_t* tree);
void NodeDump(Tree_t* tree, TreeNode_t* node, size_t* nodeCount, FILE* file);
int TreeVerify(Tree_t* tree);
int TreePrint(TreeNode_t* root, FILE* stream, size_t tabCount=0);
int TrNodeRemove(Tree_t* tree, TreeNode_t* node);
TreeNode_t* TrNodeInsert(Tree_t* tree, TreeNode_t* node, const Val_t* value, InsMode insMode);
int TreeIsEmpty(Tree_t* tree);
int TreeCleaning(TreeNode_t* root);
int TreeIsDestructed(Tree_t* tree);
int TreeDtor(Tree_t* tree);
int TreeCtor(Tree_t* tree);
TreeNode_t* MakeNode(const Val_t* value);

