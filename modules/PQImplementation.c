/* This is the file PQImplementation.c */

#include <stdio.h>
#include <stdlib.h>
#include "PQInterface.h"

typedef struct PQNodeTag {
   PQItem   NodeItem;
   struct PQNodeTag *Link;
} PQListNode;


typedef struct PriorityQueue{
   int Count;
   PQListNode *ItemList;
   int (*CompareFunc)(PQItem a, PQItem b);
   void (*DestroyFunc)(PQItem a);
} PriorityQueue;


/* Now we give all the details of the functions */
/* declared in the interface file together with */
/* local private functions.                     */

PQ Initialize(int (*compare)(PQItem,PQItem), void (*destory)(PQItem))
{
  PQ pq = malloc(sizeof(PriorityQueue));
  pq->Count=0;
  pq->ItemList=NULL;
  pq->CompareFunc=compare;
  pq->DestroyFunc=destory;
  return pq;
}

int Empty(PQ pq)
{
  return(pq->Count==0);   
}

PQListNode *SortedInsert(PQItem Item, PQListNode *P,int (*compare)(PQItem,PQItem))
{
  PQListNode *N;

  if ((P==NULL)||compare(Item ,P->NodeItem) > 0)
  {
    N=malloc(sizeof(PQListNode));
    N->NodeItem=Item;
    N->Link=P;
    return(N);
  } 
  else 
  {
    P->Link=SortedInsert(Item, P->Link,compare);
    return(P);
  }
}

void Insert(PQItem Item, PQ pq)
{
  pq->Count++;
  pq->ItemList=SortedInsert(Item, pq->ItemList,pq->CompareFunc);
}

PQItem Remove(PQ pq)
{
  PQItem temp;
  PQListNode *free_temp;
  temp=pq->ItemList->NodeItem;
  free_temp = pq->ItemList;
  pq->ItemList=pq->ItemList->Link;
  pq->Count--;
  free(free_temp);
  return(temp);
}

void FreeQueue(PQ pq)
{
  PQListNode* temp;
  while(pq->ItemList != NULL)
  {
    temp = pq->ItemList;
    pq->ItemList = pq->ItemList->Link;
    pq->DestroyFunc(temp->NodeItem);
    free(temp);
  }
  free(pq);
}