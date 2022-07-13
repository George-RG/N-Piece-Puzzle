/* This is the file PQImplementation.c */

#include <stdio.h>
#include <stdlib.h>
#include "PQInterface.h"

typedef struct PQNodeTag {
   Pointer   NodeItem;
   struct PQNodeTag *Link;
} PQListNode;


typedef struct PriorityQueue{
   int Count;
   PQListNode *ItemList;
   CompareFunc CompareFunc;
   DestroyFunc DestroyFunc;
} PriorityQueue;


/* Now we give all the details of the functions */
/* declared in the interface file together with */
/* local private functions.                     */

PQ PQ_Initialize(CompareFunc compare, DestroyFunc destroy)
{
  PQ pq = malloc(sizeof(PriorityQueue));
  pq->Count=0;
  pq->ItemList=NULL;
  pq->CompareFunc=compare;
  pq->DestroyFunc=destroy;
  return pq;
}

int PQ_IsEmpty(PQ pq)
{
  return(pq->Count==0);   
}

PQListNode *SortedInsert(Pointer Item, PQListNode *P,CompareFunc compare)
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

void PQ_Insert(PQ pq, Pointer data)
{
  pq->Count++;
  pq->ItemList=SortedInsert(data, pq->ItemList,pq->CompareFunc);
}

Pointer PQ_Pop(PQ pq)
{
  Pointer temp;
  PQListNode *free_temp;
  temp=pq->ItemList->NodeItem;
  free_temp = pq->ItemList;
  pq->ItemList=pq->ItemList->Link;
  pq->Count--;
  free(free_temp);
  return(temp);
}

void PQ_Destroy(PQ pq)
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