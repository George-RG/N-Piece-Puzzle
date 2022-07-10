#include<stdio.h>
#include <stdlib.h>
#include "ListInterface.h"

typedef struct _node
{
    PQItem value;
    struct _node* Link;
}ListNode;

typedef struct _list
{
    int count;
    ListNode* first;
    int (*CompareFunc)(PQItem a, PQItem b);
   void (*DestroyFunc)(PQItem a);
}List;

ListPtr CreateList(int (*compare)(PQItem,PQItem),void (*destory)(PQItem a))
{
    ListPtr ls = malloc(sizeof(List));
    ls->count=0;
    ls->first=NULL;
    ls->CompareFunc=compare;
    ls->DestroyFunc = destory;
    return ls;
}

int ListEmpty(ListPtr ls)
{
    return(ls->count == 0);
}

void ListInsert(PQItem Item, ListPtr ls)
{
    ls->count++;

    ListNode* temp = ls->first;
    ls->first = malloc(sizeof(ListNode));
    ls->first->value=Item;
    ls->first->Link=temp;
}

PQItem ListRemove_nth(ListPtr ls, int N)
{
    PQItem temp;
    ListNode* cursor=ls->first;
    ListNode* temp_free;
    int i;

    if(N<=1)
    {
        temp = ls->first->value;
        temp_free = ls->first;
        ls->first=ls->first->Link;
        free(temp_free);
        ls->count--;
        return (temp);
    }

    for(i=0;i<N-2;i++)
    {
        cursor = cursor->Link;
        if(cursor->Link==NULL)
            break;
    }

    temp_free = cursor->Link;
    temp = cursor->Link->value;
    cursor->Link = cursor->Link->Link;
    free(temp_free);

    ls->count--;

    return temp;
}

int ListFind(ListPtr ls, PQItem value)
{
    int i;
    ListNode* cursor=ls->first;

    for(i=0;i<ls->count;i++)
    {   
        if(!ls->CompareFunc(cursor->value, value))
            return i+1;
    
        cursor=cursor->Link;
    }
    return -1;
}


void freeList(ListPtr ls)
{
    ListNode* temp;
    while(ls->first != NULL)
    {
        temp = ls->first;
        ls->first = ls->first->Link;
        if(ls->DestroyFunc!=NULL)
            ls->DestroyFunc(temp->value);
        free(temp);
    }
    free(ls);
}

PQItem ListGetNth(ListPtr ls, int N)
{
    ListNode* temp = ls->first;
    if(ls->first == NULL)
        return NULL;

    for(int i=1; i<N; i++)
    {
        if(temp == NULL)
            return NULL;

        temp = temp->Link;
    }
    return temp->value;
}