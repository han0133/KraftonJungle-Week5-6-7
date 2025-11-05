//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 1 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024
///////////////////////////////////////////////////////////////////////////////////

typedef struct _bstnode
{
	int item;
	struct _bstnode *left;
	struct _bstnode *right;
} BSTNode; // You should not change the definition of BSTNode

typedef struct _QueueNode
{
	BSTNode *data;
	struct _QueueNode *nextPtr;
} QueueNode; // You should not change the definition of QueueNode

typedef struct _queue
{
	QueueNode *head;
	QueueNode *tail;
} Queue; // You should not change the definition of queue

///////////////////////////////////////////////////////////////////////////////////

// You should not change the prototypes of these functions
void levelOrderTraversal(BSTNode *node);

void insertBSTNode(BSTNode **node, int value);

BSTNode *dequeue(QueueNode **head, QueueNode **tail);
void enqueue(QueueNode **head, QueueNode **tail, BSTNode *node);
int isEmpty(QueueNode *head);
void removeAll(BSTNode **node);
void runTestTree();

///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = 1;

	// Initialize the Binary Search Tree as an empty Binary Search Tree
	BSTNode *root;
	root = NULL;

	// printf("1: Insert an integer into the binary search tree;\n");
	// printf("2: Print the level-order traversal of the binary search tree;\n");
	// printf("3: Run test with pre-defined tree;\n");
	// printf("0: Quit;\n");

	// while (c != 0)
	// {
	// 	printf("Please input your choice(1/2/3/0): ");
	// 	scanf("%d", &c);

	// 	switch (c)
	// 	{
	// 	case 1:
	// 		printf("Input an integer that you want to insert into the Binary Search Tree: ");
	// 		scanf("%d", &i);
	// 		insertBSTNode(&root, i);
	// 		break;
	// 	case 2:
	// 		printf("The resulting level-order traversal of the binary search tree is: ");
	// 		levelOrderTraversal(root); // You need to code this function
	// 		printf("\n");
	// 		break;
	// 	case 3:
	// 		runTestTree();
	// 		break;
	// 	case 0:
	// 		removeAll(&root);
	// 		break;
	// 	default:
	// 		printf("Choice unknown;\n");
	// 		break;
	// 	}
	// }

	// 테스트용 함수
	runTestTree();

	return 0;
}

void runTestTree()
{
	BSTNode *root = NULL;
	printf("Creating a pre-defined test tree with values...\n");

	insertBSTNode(&root, 20);
	insertBSTNode(&root, 15);
	insertBSTNode(&root, 50);
	insertBSTNode(&root, 10);
	insertBSTNode(&root, 18);
	insertBSTNode(&root, 25);
	insertBSTNode(&root, 80);

	printf("The resulting level-order traversal of the test tree is: ");
	levelOrderTraversal(root);
	printf("\n");

	removeAll(&root);
}

//////////////////////////////////////////////////////////////////////////////////
/*
	큐를 사용해서 이진 트리를 레벨 순서대로 순회하고, 각 노드의 값을 출력하는 반복문 기반의 함수
*/
void levelOrderTraversal(BSTNode *root)
{
	// 1. 큐 선언 및 초기화
	Queue q = {0};
	while (!isEmpty(q.head))
		dequeue(&q.head, &q.tail);

	// 2. 루트가 NULL이면 순회할 게 없음. 함수를 종료함.
	if (root == NULL)
		return;

	// 3. 루트 노드를 큐에 넣음
	enqueue(&q.head, &q.tail, root);

	// 4. 큐가 빌 때까지 반복
	while (!isEmpty(q.head))
	{
		// 큐에서 노드를 꺼내서 노드의 값 출력
		BSTNode *curNode = dequeue(&q.head, &q.tail);
		printf("%d ", curNode->item);

		// 꺼낸 노드의 왼쪽 자식이 있으면 큐에 넣는다
		if (curNode->left != NULL)
		{
			enqueue(&q.head, &q.tail, curNode->left);
		}

		// 꺼낸 노드의 오른쪽 자식이 있으면 큐에 넣는다
		if (curNode->right != NULL)
		{
			enqueue(&q.head, &q.tail, curNode->right);
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

// enqueue node
void enqueue(QueueNode **headPtr, QueueNode **tailPtr, BSTNode *node)
{
	// dynamically allocate memory
	QueueNode *newPtr = malloc(sizeof(QueueNode));

	// if newPtr does not equal NULL
	if (newPtr != NULL)
	{
		newPtr->data = node;
		newPtr->nextPtr = NULL;

		// if queue is empty, insert at head
		if (isEmpty(*headPtr))
		{
			*headPtr = newPtr;
		}
		else
		{ // insert at tail
			(*tailPtr)->nextPtr = newPtr;
		}

		*tailPtr = newPtr;
	}
	else
	{
		printf("Node not inserted");
	}
}

BSTNode *dequeue(QueueNode **headPtr, QueueNode **tailPtr)
{
	BSTNode *node = (*headPtr)->data;
	QueueNode *tempPtr = *headPtr;
	*headPtr = (*headPtr)->nextPtr;

	if (*headPtr == NULL)
	{
		*tailPtr = NULL;
	}

	free(tempPtr);

	return node;
}

int isEmpty(QueueNode *head)
{
	return head == NULL;
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
