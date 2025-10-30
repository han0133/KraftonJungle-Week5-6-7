//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 2 */

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
void inOrderTraversal(BSTNode *node);
void runTest();

void insertBSTNode(BSTNode **node, int value);

void push(Stack *stack, BSTNode *node);
BSTNode *pop(Stack *s);
BSTNode *peek(Stack *s);
int isEmpty(Stack *s);
void removeAll(BSTNode **node);

///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = 1;

	// Initialize the Binary Search Tree as an empty Binary Search Tree
	BSTNode *root;
	root = NULL;

	// printf("1: Insert an integer into the binary search tree;\n");
	// printf("2: Print the in-order traversal of the binary search tree;\n");
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
	// 		printf("The resulting in-order traversal of the binary search tree is: ");
	// 		inOrderTraversal(root); // You need to code this function
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

void runTest()
{
	BSTNode *root = NULL;
	printf("Creating a pre-defined test tree with values: 20, 15, 50, 1, 18\n");

	insertBSTNode(&root, 20);
	insertBSTNode(&root, 15);
	insertBSTNode(&root, 50);
	insertBSTNode(&root, 10);
	insertBSTNode(&root, 18);

	printf("The resulting in-order traversal of the test tree is: ");
	inOrderTraversal(root);
	printf("\n");

	removeAll(&root);
}

//////////////////////////////////////////////////////////////////////////////////
/**
	스택을 사용해서 이진 탐색 트리의 중위 순회 결과를 출력하는 함수. 반복문 사용.
 */
void inOrderTraversal(BSTNode *root)
{
	// 1. 스택선언 및 초기화
	Stack stack;
	stack.top = NULL;

	// 스택이 비어있지 않다면 비우기
	while (!isEmpty(&stack))
	{
		pop(&stack);
	}

	// 2. 현재 노드를 루트로 초기화
	BSTNode *curNode = root;

	// 3. 현재 노드가 NULL이 아니거나, 스택이 비어있지 않은 동안 반복.
	while (curNode != NULL || !isEmpty(&stack))
	{
		// 왼쪽 노드부터. 현재 노드가 NULL이 아닐 동안 왼쪽으로 계속 이동하며 스택에 넣는다
		while (curNode != NULL)
		{
			push(&stack, curNode);
			curNode = curNode->left;
		}

		// 가장 왼쪽 노드까지 갔으면 스택에서 노드를 pop.
		curNode = pop(&stack);
		printf("%d ", curNode->item);

		// 오른쪽 처리.
		curNode = curNode->right;
	}
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
