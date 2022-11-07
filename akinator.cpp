#include "akinator.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
	Tree_t* tree = DataDownload();

	int choice = 0;

	AkinPrint("Hello, I am Akinator! Choose the game mode and let us go play!\n");

	while ((choice = MainMenu()) != 'q' && choice != 'e')
    {
		switch (choice)
		{
			case 'g': GuessCharacter(tree); break;
			case 'd': GetDefinition(tree);  break;
			case 'v': TreeDump(tree);       break;
			case 'c': GetComparison(tree);  break;
			default: break;
		}
	}
	
	if (choice == 'q')
	{
		DataUpload(tree);
	}

    AkinPrint("The program is completed. Goodbye!\n");

	TreeCleaning(tree->root);
	free(tree);

#ifdef LOG_MODE
    endLog(LogFile);
#endif

	return 0;
}

int MainMenu(void)
{
    int choice = 0;
	
	printf("\n\n****");
    AkinPrint("Select mode:\n");

	printf("[g]uess the character\n"
		   "[d]efinition\n"
		   "[c]omparison\n"
		   "[v]isualize the database\n"
		   "[q]uit the program\n"
		   "[e]xit without saving\n\n");

    while ((choice = getchar()) != EOF)
    {
		while (getchar() != '\n')
			continue;

		if (strchr("gdcvqe", choice) == NULL)
		{
			AkinPrint("Invalid value entered. Enter e, g, d, c, v, or q:\n");
		}

		else
			break;
    }

	if (choice == EOF)
	{
        AkinPrint("The end of the file has been reached\n");
        choice = 'e';
    }

    return choice;
}

Tree_t* DataDownload(void)
{
	FILE* data = fopen("obj/database", "r");
	if (data == NULL)
	{
		AkinPrint("I can not connect to the database\n");
		return NULL;
	}

	Tree_t* tree = (Tree_t*) calloc(1, sizeof(Tree_t));
	TreeCtor(tree);

	TEXT dataBase = {};
	textCtor(&dataBase, data);
	fclose(data);

	ReadTree(&dataBase, tree);

	textDtor(&dataBase);

	return tree;
}

int ReadTree(TEXT* database, Tree_t* tree)
{
	if (database == NULL) return NULL_DATABASE;
	if (tree     == NULL) return NULL_TREE;
	
	Stack_t parentStk = {};
	StackCtor(&parentStk, 10);
	
	const char* open = strchr(database->lines[0].lineStart, '{');
	if (open == NULL)
		return WRONG_DATA;

	char rootValue[STR_MAX_SIZE] = "";
	
	if (MakeNodeValue(rootValue, open + 3))	return WRONG_DATA;

	InsMode     curMode = LEFT;
	TreeNode_t* curNode = TrNodeInsert(tree, tree->root, rootValue, curMode);

	if (strchr(open, '}') != NULL)
	{
		StackDtor(&parentStk);
		return STATUS_OK;
	}
		
	for (size_t index = 1; index < database->nLines; ++index)
	{
		char value[STR_MAX_SIZE] = "";

		open = strchr(database->lines[index].lineStart, '{');
		if (open != NULL)
		{
			StackPush(&parentStk, curNode);

			if (MakeNodeValue(value, open + 3))	return WRONG_DATA;

			curNode = TrNodeInsert(tree, curNode, value, curMode);

			if (strchr(open, '}') != NULL)
			{
				curNode = StackPop(&parentStk);

				curMode = (curMode == LEFT) ? RIGHT : LEFT;
			}
			else
			{
				curMode = LEFT;
			}
		}
		else if (strchr(database->lines[index].lineStart, '}'))
		{
			curNode = StackPop(&parentStk);
			curMode = RIGHT;
		}
		
	}

	StackDtor(&parentStk);

	return STATUS_OK;

}

int MakeNodeValue(char* value, const char* data)
{
	if (value == NULL || data == NULL) return BAD_NODE;
	
	const char* end = strchr(data, '"');
	if (end == NULL)
	{
		return WRONG_DATA;
	}
	
	strncpy(value, data, end - data);

	return 0;
}

int DataUpload(Tree_t* tree)
{
	if (tree == NULL) return NULL_TREE;

	FILE* data = fopen("obj/database", "w");
	if (data == NULL)
		return NULL_DATABASE;

	TreePrint(tree->root, data);
	fclose(data);

	return STATUS_OK;
}

int GuessCharacter(Tree_t* tree)
{
	if (tree == NULL) return NULL_DATABASE;

	Stack_t questionStk = {};
	StackCtor(&questionStk, 10);
	
	TreeNode_t* curNode = tree->root;
	int         choice  = 0;

	while (curNode != NULL)
	{
		StackPush(&questionStk, curNode);

		AkinPrint("Is it %s?", curNode->value);
		printf("\n\t[y]es\n\t[n]o\n");

		while ((choice = getchar()) != EOF)
		{
			while (getchar() != '\n')
				continue;

			if (strchr("yn", choice) == NULL)
				AkinPrint("Invalid value entered. Enter y, or n:\n");
			else
				break;
		}	
		
		if (choice == 'y')	
		{
			curNode = curNode->left;
		}
		else 
			curNode = curNode->right;
	}
	
	curNode = StackPop(&questionStk);
	if (choice == 'y')
	{
		AkinPrint("Hurray! I guessed your character! This is %s\n", curNode->value);
		
		StackDtor(&questionStk);
		return STATUS_OK;
	}
	
	AddInBase(tree, curNode);
	
	StackDtor(&questionStk);

	return STATUS_OK;
}

int AddInBase(Tree_t* tree, TreeNode_t* curNode)
{
	if (curNode == NULL) return NULL_NODE;
	if (tree    == NULL) return NULL_TREE;
	
	AkinPrint("Okay, who was that?\n");

	char newCharacter[STR_MAX_SIZE] = "";
	ReadName(newCharacter);

	TrNodeInsert(tree, curNode, newCharacter, LEFT);
	TrNodeInsert(tree, curNode, curNode->value, RIGHT);

	AkinPrint("How does %s differ from the %s\n (Try to describe without using \"Not\"):\n", 
			newCharacter, curNode->value);

	char difference[STR_MAX_SIZE] = "";
	ReadName(difference);

	while (strstr(difference, "Not") != NULL)
	{
		AkinPrint("Try to describe without using \"Not\":\n");
		fgets(difference, STR_MAX_SIZE - 1, stdin);
	}
	
	strncpy(curNode->value, difference, STR_MAX_SIZE - 1);
	
	AkinPrint("Thanks for the help! Now I know about %s\n", curNode->left->value);
	
	return STATUS_OK;
}

int GetDefinition(Tree_t* tree)
{
	if (tree == NULL) return NULL_TREE;

	AkinPrint("Who do you want to know the whole truth about?\n");

	char object[STR_MAX_SIZE] = "";
	ReadName(object);

	Stack_t routeStk = {};
	StackCtor(&routeStk, 10);
	
	size_t count = 0;
	
	int findResult = FindObject(object, tree, tree->root, &count, &routeStk);

	switch (findResult)
	{
		case NON_EXISTABLE_OBJ: AkinPrint("I am sorry but I do not know who is it...\n"); break;
		case STATUS_OK:         PrintDefinition(&routeStk); break;
		default:                AkinPrint("There are some problems with this opeartion...\n"); break;
	}
	
	StackDtor(&routeStk);

	return STATUS_OK;
}

int FindObject(char* object, Tree_t* tree, TreeNode_t* node, size_t* count, Stack_t* routeStk)
{
	if (!object && !tree && !node && !count && !routeStk) return WRONG_DATA;
	
	if (*count > tree->size) return NON_EXISTABLE_OBJ;
	
	++(*count);
	StackPush(routeStk, node);
	
	if (node->left == NULL && node->right == NULL)
	{
		if (strcmp(node->value, object) == 0)
		{
			StackPush(routeStk, node);
			return STATUS_OK;
		}
		else
		{
			StackPush(routeStk, NULL);
			return STATUS_OK;
		}
	}

	if (node->left)
	{
		StackPush(routeStk, node);
		FindObject(object, tree, node->left, count, routeStk);
	}
	
	TreeNode_t* last = StackPop(routeStk);
	if (last != NULL)
	{
		StackPush(routeStk, last);

		return STATUS_OK;
	}
	
	StackPop(routeStk);
	StackPop(routeStk);

	if (node->right)
	{
		StackPush(routeStk, NULL);
		FindObject(object, tree, node->right, count, routeStk); 
	}

	last = StackPop(routeStk);
	if (last != NULL)
	{
		StackPush(routeStk, last);
		return STATUS_OK;
	}
	
	StackPop(routeStk);
	StackPop(routeStk);
	StackPush(routeStk, NULL);

	return NON_EXISTABLE_OBJ;
	
}


int PrintDefinition(Stack_t* routeStk)
{
	if (routeStk == NULL) return WRONG_DATA;
	
	StackPop(routeStk);
	TreeNode_t* object = StackPop(routeStk);
	AkinPrint("%s is ", object->value);
	
	Stack_t reverseRoute = {};
	StackCtor(&reverseRoute, routeStk->size);

	ReverseStack(routeStk, &reverseRoute);
	
	while (reverseRoute.size != 0)
	{
		TreeNode_t* curNode = StackPop(&reverseRoute);

		if (StackPop(&reverseRoute) == NULL)
			AkinPrint("not ");

		AkinPrint("%s", curNode->value);
		printf((reverseRoute.size == 0) ? "\n" : ", ");
	}
	
	StackDtor(&reverseRoute);
	return STATUS_OK;
}

Stack_t* ReverseStack(Stack_t* stk, Stack_t* reverseStk)
{
	if (stk == NULL) return NULL;

	while (stk->size != 0)
	{
		StackPush(reverseStk, StackPop(stk));
	}

	return reverseStk;
}
	
int GetComparison(Tree_t* tree)
{
	if (tree == NULL) return NULL_TREE;
	
	AkinPrint("Enter the name of the first object:\n");
	
	char object1[STR_MAX_SIZE] = "";
	ReadName(object1);
	
	Stack_t reverse1 = {};
	if (MakeRoute(object1, tree, tree->root, &reverse1))
	{
		AkinPrint("Error in first object. Stopped!\n");
		StackDtor(&reverse1);
		
		return NON_EXISTABLE_OBJ;
	}

	AkinPrint("Enter the name of the second object:\n");
	
	char object2[STR_MAX_SIZE] = "";
	ReadName(object2);
	
	Stack_t reverse2 = {};
	if (MakeRoute(object2, tree, tree->root, &reverse2))
	{
		AkinPrint("Error in second object. Stopped!\n");
		StackDtor(&reverse1);
		StackDtor(&reverse2);
		
		return NON_EXISTABLE_OBJ;
	}

	RouteCmp(&reverse1, &reverse2, object1, object2);

	StackDtor(&reverse1);
	StackDtor(&reverse2);

	return STATUS_OK;
}	

int ReadName(char* name)
{
	if (name == NULL) return WRONG_DATA;
	
	fgets(name, STR_MAX_SIZE - 1, stdin);

	size_t last = strlen(name) - 1;
	if (name[last] == '\n')	
		name[last] = '\0';

	return STATUS_OK;
}

int MakeRoute(char* object, Tree_t* tree, TreeNode_t* node, Stack_t* reverse)
{
	if (!object && !tree && !node && !reverse) return WRONG_DATA;

	Stack_t stk = {};
	StackCtor(&stk, 10);
	
	size_t count = 0;
	
	int findResult = FindObject(object, tree, tree->root, &count, &stk);	
	
	if (findResult != STATUS_OK)
	{
		StackDtor(&stk);
		return findResult;
	}

	StackPop(&stk);
	StackPop(&stk);

	StackCtor(reverse, stk.size);
	ReverseStack(&stk, reverse);

	StackDtor(&stk);

	return STATUS_OK;
}

int RouteCmp(Stack_t* route1, Stack_t* route2, char* object1, char* object2)
{
	if (route1 == NULL || route2 == NULL) return WRONG_DATA;
	
	int common = 0;


	AkinPrint("\nThey both are...\n");
	while (route1->size != 0 && route2->size != 0)
	{
		TreeNode_t* node1 = StackPop(route1);
		TreeNode_t* node2 = StackPop(route2);
		
		TreeNode_t* ans1 = StackPop(route1);
		TreeNode_t* ans2 = StackPop(route2);

		if (ans1 == ans2 && node1 == node2)
		{
			common = 1;

			if (ans1 == NULL) 
				AkinPrint("not ");
			AkinPrint("%s, ", node1->value);
		}
		else 
		{
			if (!common)
			{
				AkinPrint("\nOhh, sorry... They have no common features\n");
			}

			AkinPrint("\nBut %s is ", object1);
			if (ans1 == NULL) 
				AkinPrint("not ");

			AkinPrint("%s", node1->value);

			AkinPrint(", and %s is ", object2);
			if (ans2 == NULL) 
				AkinPrint("not ");

			AkinPrint("%s", node2->value);
		}
	}
	
	if (route2->size == 0 && route1->size != 0)
	{
		AkinPrint("\nAlso %s is ", object1);
		while (route1->size != 0)
		{
			TreeNode_t* curNode = StackPop(route1);

			if (StackPop(route1) == NULL)
				AkinPrint("not ");

			AkinPrint("%s", curNode->value);
			printf((route1->size == 0) ? "\n" : ", ");
		}
	}
	else if (route1->size == 0 && route2->size != 0)
	{
		AkinPrint("\nAlso %s is ", object2);
		while (route2->size != 0)
		{
			TreeNode_t* curNode = StackPop(route2);

			if (StackPop(route2) == NULL)
				AkinPrint("not ");

			AkinPrint("%s", curNode->value);
			printf((route2->size == 0) ? "\n" : ", ");
		}	
	}

	return STATUS_OK;
}

void AkinPrint(char text[TEXT_SIZE], ...)
{
	char festText[TEXT_SIZE * 2] = "";

	char* akinText = (char*) calloc(TEXT_SIZE, sizeof(char));

	va_list arg = {};
    va_start (arg, text);
	
	vsprintf(akinText, text, arg);
	sprintf(festText, "echo \"%s\" | festival --tts", akinText);
	
	printf(akinText);
	system(festText);

    va_end (arg);

	free(akinText);
}
