#include"Item.h"

typedef struct PriorityQueue* PQ;
PQ Initialize(int (*compare)(PQItem,PQItem), void (*destory)(PQItem));
int Empty (PQ);
void Insert (PQItem, PQ);
PQItem Remove (PQ);
void FreeQueue(PQ pq);
