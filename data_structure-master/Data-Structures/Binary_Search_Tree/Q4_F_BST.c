//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 4 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _bstnode
{
	int item;
	struct _bstnode *left;
	struct _bstnode *right;
} BSTNode; // You should not change the definition of BSTNode

typedef struct _stackNode
{
	BSTNode *data;
	struct _stackNode *next;
} StackNode; // You should not change the definition of StackNode

typedef struct _stack
{
	StackNode *top;
} Stack; // You should not change the definition of Stack

///////////////////////// function prototypes ////////////////////////////////////

// You should not change the prototypes of these functions
void postOrderIterativeS1(BSTNode *node);

void insertBSTNode(BSTNode **node, int value);

// You may use the following functions or you may write your own
void push(Stack *stack, BSTNode *node);
BSTNode *pop(Stack *s);
BSTNode *peek(Stack *s);
int isEmpty(Stack *s);
void removeAll(BSTNode **node);
void runTest();

///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = 1;

	// Initialize the Binary Search Tree as an empty Binary Search Tree
	BSTNode *root;
	root = NULL;

	// printf("1: Insert an integer into the binary search tree;\n");
	// printf("2: Print the post-order traversal of the binary search tree;\n");
	// printf("0: Quit;\n");

	// while (c != 0)
	// {
	// 	printf("Please input your choice(1/2/0): ");
	// 	scanf("%d", &c);

	// 	switch (c)
	// 	{
	// 	case 1:
	// 		printf("Input an integer that you want to insert into the Binary Search Tree: ");
	// 		scanf("%d", &i);
	// 		insertBSTNode(&root, i);
	// 		break;
	// 	case 2:
	// 		printf("The resulting post-order traversal of the binary search tree is: ");
	// 		postOrderIterativeS1(root); // You need to code this function
	// 		printf("\n");
	// 		break;
	// 	case 0:
	// 		removeAll(&root);
	// 		break;
	// 	default:
	// 		printf("Choice unknown;\n");
	// 		break;
	// 	}
	// }
	runTest();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

void runTest()
{
	BSTNode *root = NULL;
	insertBSTNode(&root, 20);
	insertBSTNode(&root, 15);
	insertBSTNode(&root, 50);
	insertBSTNode(&root, 10);
	insertBSTNode(&root, 18);
	insertBSTNode(&root, 25);
	insertBSTNode(&root, 80);

	postOrderIterativeS1(root);
}

/*
	스택 사용해서 이진 탐색 트리를 후위 순회(왼>오>루트)하는 함수
*/
void postOrderIterativeS1(BSTNode *root)
{
	if (root == NULL)
		return;

	Stack s1 = {0};
	Stack s2 = {0};

	push(&s1, root);
	BSTNode *node; // BSTNode는 이진 탐색 트리의 노드를 나타내는 구조체 타입. 각 노드는 int item, BSTNode* right/left

	// s1에서 팝하여 s2에 푸시하고, 자식들을 s1에 푸시
	while (!isEmpty(&s1))
	{
		node = pop(&s1);
		push(&s2, node);

		if (node->left)
		{
			push(&s1, node->left);
		}
		if (node->right)
		{
			push(&s1, node->right);
		}
	}

	// s2의 모든 노드를 팝하여 출력
	// printf("Post-order (iterative, 2 stacks): ");
	while (!isEmpty(&s2))
	{
		node = pop(&s2);
		printf("%d ", node->item);
	}
	printf("\n");
}

///////////////////////////////////////////////////////////////////////////////

void insertBSTNode(BSTNode **node, int value)
{
	if (*node == NULL)
	{
		*node = malloc(sizeof(BSTNode));

		if (*node != NULL)
		{
			(*node)->item = value;
			(*node)->left = NULL;
			(*node)->right = NULL;
		}
	}
	else
	{
		if (value < (*node)->item)
		{
			insertBSTNode(&((*node)->left), value);
		}
		else if (value > (*node)->item)
		{
			insertBSTNode(&((*node)->right), value);
		}
		else
			return;
	}
}

//////////////////////////////////////////////////////////////////////////////////

void push(Stack *stack, BSTNode *node)
{
	StackNode *temp;

	temp = malloc(sizeof(StackNode));

	if (temp == NULL)
		return;
	temp->data = node;

	if (stack->top == NULL)
	{
		stack->top = temp;
		temp->next = NULL;
	}
	else
	{
		temp->next = stack->top;
		stack->top = temp;
	}
}

BSTNode *pop(Stack *s)
{
	StackNode *temp, *t;
	BSTNode *ptr;
	ptr = NULL;

	t = s->top;
	if (t != NULL)
	{
		temp = t->next;
		ptr = t->data;

		s->top = temp;
		free(t);
		t = NULL;
	}

	return ptr;
}

BSTNode *peek(Stack *s)
{
	StackNode *temp;
	temp = s->top;
	if (temp != NULL)
		return temp->data;
	else
		return NULL;
}

int isEmpty(Stack *s)
{
	if (s->top == NULL)
		return 1;
	else
		return 0;
}

void removeAll(BSTNode **node)
{
	if (*node != NULL)
	{
		removeAll(&((*node)->left));
		removeAll(&((*node)->right));
		free(*node);
		*node = NULL;
	}
}
