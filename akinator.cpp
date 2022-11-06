#include "akinator.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
	Tree_t* tree = DataDownload();
	
	int choice = 0;

	while ((choice = MainMenu()) != 'q' && choice != 'e')
    {
		switch (choice)
		{
			case 'g': GuessCharacter(tree); break;
			case 'd': GetDefinition(tree);  break;
			case 'v': TreeDump(tree);       break;
			default: break;
		}

	}
	
	if (choice == 'q')
	{
		DataUpload(tree);
	}

    puts("The program is completed");

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

    puts("\n****Select mode:\n"
		 "[g]uess the character\n"
		 "[d]efinition\n"
		 "[c]omparison\n"
		 "[v]isualize the database\n"
		 "[q]uit the program\n"
		 "[e]xit without saving\n");

    while ((choice = getchar()) != EOF)
    {
		while (getchar() != '\n')
			continue;

		if (strchr("gdcvqe", choice) == NULL)
		{
			puts("Invalid value entered. Enter 'e', 'g', 'd', 'c', 'v', or 'q':\n");
		}

		else
			break;
    }

	if (choice == EOF)
	{
        printf("The end of the file has been reached\n");
        choice = 'e';
    }

    return choice;
}

Tree_t* DataDownload(void)
{
	FILE* data = fopen("obj/database", "r");
	if (data == NULL)
	{
		fprintf(stderr, "I can't connect to the database\n");
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

		printf("Is/Does it %s?\n"
				"\t[y]es\n"
				"\t[n]o\n", curNode->value);

		while ((choice = getchar()) != EOF)
		{
			while (getchar() != '\n')
				continue;

			if (strchr("yn", choice) == NULL)
				puts("Invalid value entered. Enter 'y', or 'n':\n");
			else
				break;
		}	
		
		if (choice == 'y')	
		{
			curNode = curNode->left;
			StackPop(&questionStk);
		}
		else 
			curNode = curNode->right;

	}
	
	curNode = StackPop(&questionStk);
	if (choice == 'y')
	{
		printf("Hurray! I guessed your character! This is %s\n", curNode->value);
		
		StackDtor(&questionStk);
		return STATUS_OK;
	}
	
	AddInBase(curNode);
	
	StackDtor(&questionStk);

	return STATUS_OK;
}

int AddInBase(TreeNode_t* curNode)
{
	if (curNode == NULL) return NULL_NODE;
	
	printf ("Okay, who was that?\n");

	char newCharacter[STR_MAX_SIZE] = "";
	fgets(newCharacter, STR_MAX_SIZE - 1, stdin);
	
	size_t last = strlen(newCharacter) - 1;
	if (newCharacter[last] == '\n')	
		newCharacter[last] = '\0';	
	
	TreeNode_t* newLeft  = MakeNode(newCharacter);
	TreeNode_t* newRight = MakeNode(curNode->value);

	printf ("How does %s differ from the %s\n"
			"(Try to describe without using \"Not\"):\n", newLeft->value, curNode->value);

	char difference[STR_MAX_SIZE] = "";
	fgets(difference, STR_MAX_SIZE - 1, stdin);
	
	last = strlen(difference) - 1;
	if (difference[last] == '\n')	
		difference[last] = '\0';	

	while (strstr(difference, "Not") != NULL)
	{
		printf("Try to describe without using \"Not\":\n");
		fgets(difference, STR_MAX_SIZE - 1, stdin);
	}
	
	strncpy(curNode->value, difference, STR_MAX_SIZE - 1);
	
	curNode->left  = newLeft;
	curNode->right = newRight;

	printf("Thanks for the help! Now I know about %s\n", curNode->left->value);

	return STATUS_OK;
}

int GetDefinition(Tree_t* tree)
{
	if (tree == NULL) return NULL_TREE;

	printf("Who do you want to know the whole truth about?\n");

	char object[STR_MAX_SIZE] = "";

	fgets(object, STR_MAX_SIZE - 1, stdin);

	size_t last = strlen(object) - 1;
	if (object[last] == '\n')	
		object[last] = '\0';
	
	Stack_t routeStk = {};
	StackCtor(&routeStk, 10);
	
	size_t count = 0;
	
	int findResult = FindObject(object, tree, tree->root, &count, &routeStk);

	switch (findResult)
	{
		case NON_EXISTABLE_OBJ: printf("I'm sorry but I do not know who is it...\n"); break;
		case STATUS_OK:         PrintDefinition(&routeStk); break;
		default:                printf("There are some problems with this opeartion...\n"); break;
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
	printf("%s is ", object->value);

	Stack_t reverseRoute = {};
	StackCtor(&reverseRoute, routeStk->size);
	
	while (routeStk->size != 0)
	{
		StackPush(&reverseRoute, StackPop(routeStk));
	}
	
	while (reverseRoute.size != 0)
	{
		TreeNode_t* curNode = StackPop(&reverseRoute);

		if (StackPop(&reverseRoute) == NULL)
			printf("not ");

		printf("%s", curNode->value);
		printf((reverseRoute.size == 0) ? "\n" : ", ");
		
	}
	
	StackDtor(&reverseRoute);
	return STATUS_OK;
}
	
