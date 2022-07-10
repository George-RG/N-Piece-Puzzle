#include"Item.h"

typedef struct _list *ListPtr;
ListPtr CreateList(int (*compare)(PQItem,PQItem) ,void (*destory)(PQItem a));
int ListEmpty(ListPtr ls);
void ListInsert(PQItem Item, ListPtr ls);
PQItem ListRemove_nth(ListPtr ls, int N);
int ListFind(ListPtr ls, PQItem value);
void freeList(ListPtr ls);
PQItem ListGetNth(ListPtr ls, int N);
