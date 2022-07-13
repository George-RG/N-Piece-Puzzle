typedef void *Pointer; //Void pointer to store any kind of data needed
typedef void(*DestroyFunc)(Pointer); //Function to propertly free the momory that our data are using
typedef int(*CompareFunc)(Pointer a,Pointer b);//Returns 0 if a=b,<0 if a<b,>0 if a>b

typedef struct PriorityQueue* PQ;

PQ PQ_Initialize(CompareFunc CompareFunc, DestroyFunc destroy);
int PQ_IsEmpty (PQ);
void PQ_Insert (PQ, Pointer data);
Pointer PQ_Pop (PQ);
void PQ_Destroy(PQ pq);
