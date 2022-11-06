#include "akinator.h"

#ifdef LOG_MODE
    FILE* LogFile = startLog(LogFile);
#endif

int main(void)
{
	Tree_t* tree = DataDownload();
	
	/*FILE* data = fopen("obj/database", "w");
	TreePrint(tree->root, data);
	fclose(data);
	
	TreeDump(tree);

	TreeCleaning(tree->root);
	free(tree);
	*/
	int choice = 0;

	while ((choice = MainMenu()) != 'q' && choice != 'e')
    {
		switch (choice)
		{
			case 'g': GuessCharacter(tree); break;
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

    puts("****Select mode:\n"
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
		
		if (choice == 'y')	curNode = curNode->left;
		else				curNode = curNode->right;

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
	if (newCharacter[last] = '\n')	
		newCharacter[last] = '\0';	
	
	TreeNode_t* newLeft  = MakeNode(newCharacter);
	TreeNode_t* newRight = MakeNode(curNode->value);

	printf ("How does %s differ from the %s\n"
			"(Try to describe without using \"Not\"):\n", newLeft->value, curNode->value);

	char difference[STR_MAX_SIZE] = "";
	fgets(difference, STR_MAX_SIZE - 1, stdin);
	
	last = strlen(difference) - 1;
	if (difference[last] = '\n')	
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







	
