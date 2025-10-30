//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 5
		 Implementing 'remove node' operation for BST*/
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
void postOrderIterativeS2(BSTNode *root);

void insertBSTNode(BSTNode **node, int value);

void push(Stack *stack, BSTNode *node);
BSTNode *pop(Stack *s);
BSTNode *peek(Stack *s);
int isEmpty(Stack *s);
void removeAll(BSTNode **node);
BSTNode *removeNodeFromTree(BSTNode *root, int value);
void runTest();

///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = 1;

	// Initialize the Binary Search Tree as an empty Binary Search Tree
	// BSTNode *root;
	// root = NULL;

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
	// 		postOrderIterativeS2(root); // You need to code this function
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
	// 이진 탐색 트리 초기화
	BSTNode *root = NULL;

	// 요청된 값으로 노드 삽입
	insertBSTNode(&root, 20);
	insertBSTNode(&root, 15);
	insertBSTNode(&root, 50);
	insertBSTNode(&root, 10);
	insertBSTNode(&root, 18);
	insertBSTNode(&root, 25);
	insertBSTNode(&root, 80);

	// postOrderIterativeS2 함수 호출하여 테스트
	printf("Post-order traversal (S2): ");
	postOrderIterativeS2(root);
	printf("\n");

	// 테스트 후 메모리 해제 (필요 시)
	removeAll(&root);
}

/*
	두 개의 스택을 사용하여 이진탐색트리의 후위순회 결과를 출력하는 함수.
*/
void postOrderIterativeS2(BSTNode *root)
{
	Stack s1 = {0};
	Stack s2 = {0};

	while (!isEmpty(&s1))
	{
		pop(&s1);
	}

	while (!isEmpty(&s2))
	{
		pop(&s2);
	}

	if (root == NULL)
		return;

	push(&s1, root);
	BSTNode *node;

	// s1에서 노드를 꺼내서 s2에 넣고, s1에 자식노드를 넣는다
	while (!isEmpty(&s1))
	{
		node = pop(&s1);
		push(&s2, node);

		if (node->left != NULL)
			push(&s1, node->left);
		if (node->right != NULL)
			push(&s1, node->right);
	}

	while (!isEmpty(&s2))
	{
		node = pop(&s2);
		printf("%d ", node->item);
	}
	printf("\n");
}

/* Given a binary search tree and a key, this function
	 deletes the key and returns the new root. Make recursive function. */
// BSTNode *removeNodeFromTree(BSTNode *root, int value)
// {
// 	/* add your code here */
// }
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
