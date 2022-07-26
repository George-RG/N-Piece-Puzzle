#pragma once

typedef void *Pointer; //Void pointer to store any kind of data needed
typedef void(*DestroyFunc)(Pointer); //Function to propertly free the momory that our data are using
typedef int(*CompareFunc)(Pointer a,Pointer b);//Returns 0 if a=b,<0 if a<b,>0 if a>b 

typedef struct redblack *RB; // Pointer to the entire tree

typedef struct _node *NodePtr;// Pointer to one of the tree's nodes


RB RB_Initialize(DestroyFunc Destroy,CompareFunc Compare);
int RB_RemoveKey(RB tree, Pointer key); //Returns -1 in case the key is not found
int RB_InsertKey(RB tree, Pointer key); //Returns -1 in case the insertion failed
NodePtr RB_Search(RB tree, Pointer key);
void RB_PrintElements(RB tree,void(*Visit)(Pointer));
Pointer RB_DataFromNode(NodePtr node);
int RB_Size(RB tree);
void RB_print2D(RB tree);
void RB_Destroy(RB tree);