#include "tree.h"

extern FILE* LogFile;

int TreeCtor(Tree_t* tree)
{
	if (tree == NULL) return TREE_NULL_PTR;
	
	tree->root   == NULL;
	tree->size   == 0;
	tree->status == TREE_IS_OK;

	return TREE_IS_OK;
}

int TreeDtor(Tree_t* tree)
{
	if (tree == NULL)								  return TREE_NULL_PTR;
	if (TreeIsDestructed(tree) == TREE_IS_DESTRUCTED) return TREE_IS_DESTRUCTED;

	TreeCleaning(tree->root);

	tree->size    = DEAD_SIZE;

	return TREE_IS_DESTRUCTED;
}

int TreeIsDestructed(Tree_t* tree)
{
	if (tree == NULL)            return TREE_NULL_PTR;
	if (tree->size == DEAD_SIZE) return TREE_IS_DESTRUCTED;

	return TREE_UB;
}

int TreeIsEmpty(Tree_t* tree)
{
	if (tree       == NULL)                    return TREE_NULL_PTR;
	if (tree->root == NULL && tree->size == 0) return TREE_IS_EMPTY;

	return TREE_UB;
}

int TreeCleaning(TreeNode_t* root)
{
	if (root == NULL) return NODE_NULL_PTR;

	if (root->left != NULL)
		TreeCleaning(root->left);

	if (root->right != NULL)
		TreeCleaning(root->right);

	free(root);

	return TREE_IS_OK;
}

TreeNode_t* TrNodeInsert(Tree_t* tree, TreeNode_t* node, const Val_t* value, InsMode insMode)
{
	if (tree == NULL)						 return NULL;
	if (node == NULL && tree->root)		  	 return NULL;
	if (insMode != LEFT && insMode != RIGHT) return NULL;
	
	TreeNode_t* curNode = MakeNode(value);	

	strcpy(curNode->value, value);
	
	if (tree->root == node && tree->size == 0)
	{
		tree->root = curNode;
		tree->size = 1;

		return tree->root;
	}

	if (insMode == LEFT)
	{
		node->left = curNode;
	}
	else
	{
		node->right = curNode;
	}

	++tree->size;

	return curNode;
}

TreeNode_t* MakeNode(const Val_t* value)
{
	TreeNode_t* newNode = (TreeNode_t*) calloc(1, sizeof(TreeNode_t));
	if (newNode == NULL)
		return NULL;
	
	strcpy(newNode->value, value);
	newNode->left  = NULL;
	newNode->right = NULL;

	return newNode;
}
	

int TrNodeRemove(Tree_t* tree, TreeNode_t* node)
{
	if (tree == NULL) return TREE_NULL_PTR;
	if (node == NULL) return NODE_NULL_PTR;
	
	if (!node->left && !node->right)
	{
		TreeCleaning(node);
		--tree->size;
		return TREE_IS_OK;
	}
	
	return BAD_REMOVE;
}

int TreePrint(TreeNode_t* root, FILE* stream, size_t tabCount)
{
	if (stream == NULL) return BAD_PRINT_FILE;
	if (root   == NULL) return NODE_NULL_PTR;
	
	for (size_t index = 0; index < tabCount; ++index)
	{
		fputc('\t', stream);
	}

	fprintf(stream, "{ \"");
	fprintf(stream, "%s\"", root->value);

	if (root->left == NULL && root->right == NULL)
		fprintf(stream, " }");
	
	fputc('\n', stream);

	if (root->left  != NULL) TreePrint(root->left, stream, tabCount + 1);
	if (root->right != NULL) TreePrint(root->right, stream, tabCount + 1);

	if (!(root->left == NULL && root->right == NULL))
	{
		//fputc('\n', stream);
		for (size_t index = 0; index < tabCount; ++index)
		{
			fputc('\t', stream);
		}
		
		fprintf(stream, " }\n");
	}

	return TREE_IS_OK;
}

int TreeVerify(Tree_t* tree)
{
    if (tree			       == NULL)               return TREE_NULL_PTR;
    if (TreeIsEmpty(tree)      == TREE_IS_EMPTY)      return TREE_IS_EMPTY;
    if (TreeIsDestructed(tree) == TREE_IS_DESTRUCTED) return TREE_IS_DESTRUCTED;
	if (tree->root             == NULL)               return TREE_ROOT_IS_NULL;

    int status = TREE_IS_OK;

    return status;
}
void NodeDump(TreeNode_t* node, size_t* nodeCount, FILE* file)
{
	ASSERT(node != NULL && nodeCount != NULL && file != NULL)
	
	fprintf(file, "node%lu [fillcolor=", *nodeCount);
	if	    (*nodeCount == 0)			                fprintf(file, "\"#C0C0C0\"");
	else if (node->left == NULL && node->right == NULL) fprintf(file, "\"#98FF98\"");
	else										        fprintf(file, "\"#FFB2D0\"");
	fprintf(file, ", label=\"%s\"];\n", node->value);

	size_t curNodeIndex = *nodeCount;

	if (node->left != NULL)
	{
		++(*nodeCount);
		fprintf(file, "edge [color=\"red\", label=\"YES\"]; node%lu -> node%lu;\n", curNodeIndex, *nodeCount);
		NodeDump(node->left, nodeCount, file);
	}

	if (node->right != NULL)
	{
		++(*nodeCount);
		fprintf(file, "edge [color=\"blue\", label=\"NO\"]; node%lu -> node%lu;\n", curNodeIndex, *nodeCount);
		NodeDump(node->right, nodeCount, file);
	}
}

void TreeDump(Tree_t* tree)
{   
    ASSERT(tree != NULL);

    fprintf(LogFile, "\n<hr>\n");
   
    fprintf(LogFile, "<h1> Tree status: </h1>\n");

    FILE* DumpFile = fopen("obj/dump", "w+");

    fprintf(DumpFile, "digraph G{\n");
    fprintf(DumpFile, "node [color=black, shape=box, style=\"rounded, filled\"];\n");

    fprintf(DumpFile, "size [fillcolor=\"#FFFEB6\", "
                     "label=\"SIZE = %lu\"];\n",
                      tree->size);
    
    fprintf(DumpFile, "node [color=black, shape=box, style=\"rounded, filled\"];\n");
    fprintf(DumpFile, "\n");
    fprintf(DumpFile, "edge [style=solid, constraint=false];\n");
	
	size_t nodeCount = 0;

	NodeDump(tree->root, &nodeCount, DumpFile);

    fprintf(DumpFile, "}\n");
    
    fclose(DumpFile);

    static char pngIndex        = 1;
    char dumpName[STR_MAX_SIZE] = "";

    MakePngName(dumpName, pngIndex);

    char buff[100] = "";
    sprintf(buff, "dot obj/dump -T png -o %s", dumpName);
    system(buff);
	
    fprintf(LogFile, "<img src = %s>\n", dumpName + 4);

	char showpic[100] = "";
	sprintf(showpic, "eog %s\n", dumpName);
	system(showpic);

    ++pngIndex;

}

void MakePngName(char* name, char num)
{
    ASSERT(name != NULL);
	
	sprintf(name, "obj/dump%03d.png", num);
} 
