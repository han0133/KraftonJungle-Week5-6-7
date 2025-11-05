//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 3 */

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
void preOrderIterative(BSTNode *root);

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
	// printf("2: Print the pre-order traversal of the binary search tree;\n");
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
	// 		printf("The resulting pre-order traversal of the binary search tree is: ");
	// 		preOrderIterative(root); // You need to code this function
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
	printf("Creating a pre-defined test tree with values: 20, 15, 50, 10, 18, 25, 80\n");

	insertBSTNode(&root, 20);
	insertBSTNode(&root, 15);
	insertBSTNode(&root, 50);
	insertBSTNode(&root, 10);
	insertBSTNode(&root, 18);
	insertBSTNode(&root, 25);
	insertBSTNode(&root, 80);

	printf("The resulting pre-order traversal of the test tree is: ");
	preOrderIterative(root);
	printf("\n");

	removeAll(&root);
}

//////////////////////////////////////////////////////////////////////////////////
/*
	스택을 사용해서 이진 탐색 트리의 전위 순회 결과를 출력하는 함수
*/
void preOrderIterative(BSTNode *root)
{
	// 스택 선언 및 초기화
	Stack s;
	s.top = NULL;

	// 스택이 비어있지 않다면 비우기
	while (!isEmpty(&s))
	{
		pop(&s);
	}

	if (root == NULL)
		return;

	// 시작점(루트)를 스택에 push
	push(&s, root);

	// 스택 빌 때까지 반복
	while (!isEmpty(&s))
	{
		// 스택에서 노드를 하나 꺼낸다
		BSTNode *currentNode = pop(&s);
		printf("%d ", currentNode->item);

		// 오른쪽 자식을 스택에 넣는다
		if (currentNode->right != NULL)
		{
			push(&s, currentNode->right);
		}

		// 왼쪽 자식을 스택에 넣는다
		if (currentNode->left != NULL)
		{
			push(&s, currentNode->left);
		}
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
