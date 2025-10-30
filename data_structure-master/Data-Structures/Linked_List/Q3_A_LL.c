//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 3 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode; // You should not change the definition of ListNode

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList; // You should not change the definition of LinkedList

//////////////////////// function prototypes /////////////////////////////////////

// You should not change the prototype of this function
void moveOddItemsToBack(LinkedList *ll);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);

//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;
	// Initialize the linked list 1 as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	printf("1: Insert an integer to the linked list:\n");
	printf("2: Move all odd integers to the back of the linked list:\n");
	printf("0: Quit:\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertNode(&ll, ll.size, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			moveOddItemsToBack(&ll); // You need to code this function
			printf("The resulting linked list after moving odd integers to the back of the linked list is: ");
			printList(&ll);
			removeAllItems(&ll);
			break;
		case 0:
			removeAllItems(&ll);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

void moveOddItemsToBack(LinkedList *ll)
{
	if (ll == NULL || ll->head)
	{
		printf("moveOddItemsToBack(): ll or ll->head is NULL.\n");
		return;
	}

	// 방법2. 링크드리스트 분할 후 병합
	ListNode *pEvenHead = NULL, *pEvenTail = NULL; // 짝수 리스트
	ListNode *pOddHead = NULL, *pOddTail = NULL;	 // 홀수 리스트

	ListNode *pCurrentNode = ll->head;

	// LL 분할
	while (pCurrentNode != NULL)
	{
		ListNode *pNextNode = pCurrentNode->next;

		if (pCurrentNode->item % 2 == 0) // 짝수
		{
			if (pEvenHead == NULL)
			{
				pEvenHead = pEvenTail = pCurrentNode;
			}
			else
			{
				pEvenTail->next = pCurrentNode;
				pEvenTail = pCurrentNode;
			}
		}
		else
		{ // 홀수
			if (pOddHead == NULL)
			{
				pOddHead = pOddTail = pCurrentNode;
			}
			else
			{
				pOddTail->next = pCurrentNode;
				pOddTail = pCurrentNode;
			}
		}
		pCurrentNode->next = NULL;
		pCurrentNode = pNextNode;
	}

	// 두 리스트 병합 : 짝수 + 홀수
	if (pEvenHead == NULL)
	{
		ll->head = pOddHead;
	}
	else
	{
		ll->head = pEvenHead;
		if (pOddHead != NULL)
		{
			pEvenTail->next = pOddHead;
		}
	}

	// pOddTail이 있으면 마지막을 NULL로
	if (pOddTail != NULL)
	{
		pOddTail->next = NULL;
	}

	///////////

	// 방법1. cycle 변수 사용
	// int index = 0;
	// ListNode *pCurrent = ll->head;

	// while (pCurrent != NULL)
	// {
	// 	ListNode *pNext = pCurrent->next;
	// 	int value = pCurrent->item;

	// 	if (value % 2 != 0) // 홀수면 뒤에 추가 후 삭제한다
	// 	{
	// 		insertNode(ll, ll->size, value); // 1. 먼저 뒤에 추가
	// 		removeNode(ll, index);					 // 2. 그 다음 원본 삭제
	// 	}
	// 	else
	// 	{
	// 		index++; // 짝수일 때만 index 증가
	// 	}
	// 	pCurrent = pNext;
	// }
}

///////////////////////////////////////////////////////////////////////////////////

void printList(LinkedList *ll)
{

	ListNode *cur;
	if (ll == NULL)
		return;
	cur = ll->head;

	if (cur == NULL)
		printf("Empty");
	while (cur != NULL)
	{
		printf("%d ", cur->item);
		cur = cur->next;
	}
	printf("\n");
}

void removeAllItems(LinkedList *ll)
{
	ListNode *cur = ll->head;
	ListNode *tmp;

	while (cur != NULL)
	{
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	ll->head = NULL;
	ll->size = 0;
}

ListNode *findNode(LinkedList *ll, int index)
{

	ListNode *temp;

	if (ll == NULL || index < 0 || index >= ll->size)
		return NULL;

	temp = ll->head;

	if (temp == NULL || index < 0)
		return NULL;

	while (index > 0)
	{
		temp = temp->next;
		if (temp == NULL)
			return NULL;
		index--;
	}

	return temp;
}

int insertNode(LinkedList *ll, int index, int value)
{

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	// If empty list or inserting first node, need to update head pointer
	if (ll->head == NULL || index == 0)
	{
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}

	// Find the nodes before and at the target position
	// Create a new node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL)
	{
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}

int removeNode(LinkedList *ll, int index)
{

	ListNode *pre, *cur;

	// Highest index we can remove is size-1
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	if (index == 0)
	{
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// Free the target node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL)
	{

		if (pre->next == NULL)
			return -1;

		cur = pre->next;
		pre->next = cur->next;
		free(cur);
		ll->size--;
		return 0;
	}

	return -1;
}
