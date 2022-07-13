typedef void *Pointer; //Void pointer to store any kind of data needed
typedef void(*DestroyFunc)(Pointer); //Function to propertly free the momory that our data are using
typedef int(*CompareFunc)(Pointer a,Pointer b);//Returns 0 if a=b,<0 if a<b,>0 if a>b 
typedef void(*VisitFunc)(Pointer);//Function to use when traversing the tree

typedef struct priority_queue* PQ; //Pointer to the priority queue

PQ PQ_Initialize(CompareFunc compare, DestroyFunc destroy);//Initialize the priority queue
int PQ_IsEmpty (PQ);//Returns 1 if the queue is empty, 0 otherwise
int PQ_Size(PQ);//Returns the size of the queue
void PQ_Insert(PQ pq, Pointer data);//Inserts a new node into the queue
Pointer PQ_Pop(PQ pq);//Removes the highest priority node from the queue
void PQ_Destroy(PQ pq);//Frees all the memory allocated for the queue