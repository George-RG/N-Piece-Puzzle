#include <stdio.h>
#include <stdlib.h>

#include "PQ.h"

#define DEBUG 0

// Structs for min heap tree
typedef struct heap_node
{
    Pointer data;

    int min_height;
    int max_height;

    struct heap_node *left;
    struct heap_node *right;
    struct heap_node *parent;

} heap_node;

typedef heap_node *HeapNode;

// Struct for PQ
typedef struct priority_queue
{
    HeapNode root;

    int size;
    int layer;
    int cur_max_index;
    int cur_min_index;

    HeapNode last_inserted;

    CompareFunc compare;
    DestroyFunc destroy;
} priority_queue;


void ShowHeap(HeapNode node, int space);//Prints the heap in a 2D fashion

PQ PQ_Initialize(CompareFunc compare, DestroyFunc destroy)
{
    priority_queue *pq = malloc(sizeof(priority_queue));

    pq->root = NULL;
    pq->size = 0;
    pq->layer = 0;
    pq->cur_max_index = 0;

    pq->compare = compare;
    pq->destroy = destroy;
    return pq;
}

int PQ_IsEmpty(PQ pq)
{
    return pq->size == 0;
}

int PQ_Size(PQ pq)
{
    return pq->size;
}

HeapNode Insert_Helper(PQ pq, int height, HeapNode root, HeapNode node)
{
    if (height > pq->layer)
    {
        return NULL;
    }

    if (root->left == NULL)
    {
        root->left = node;
        root->left->parent = root;

        if (pq->compare(root->left->data, root->data) > 0)
        {
            Pointer temp = root->data;
            root->data = root->left->data;
            root->left->data = temp;
        }

        root->max_height = 1;

        return root;
    }
    else if (root->right == NULL)
    {
        root->right = node;
        root->right->parent = root;

        if (pq->compare(root->right->data, root->data) > 0)
        {
            Pointer temp = root->data;
            root->data = root->right->data;
            root->right->data = temp;
        }

        root->min_height = 1;

        return root;
    }
    else
    {
        if (height + root->left->min_height < pq->layer)
        {
            root->left = Insert_Helper(pq, height + 1, root->left, node);

            // Heapify the tree
            if (pq->compare(root->left->data, root->data) > 0)
            {
                Pointer temp = root->data;
                root->data = root->left->data;
                root->left->data = temp;
            }
        }
        else
        {
            root->right = Insert_Helper(pq, height + 1, root->right, node);

            // Heapify the tree
            if (pq->compare(root->right->data, root->data) > 0)
            {
                Pointer temp = root->data;
                root->data = root->right->data;
                root->right->data = temp;
            }
        }

        // Update min_height
        if (root->left->min_height < root->right->min_height)
            root->min_height = root->left->min_height + 1;
        else
            root->min_height = root->right->min_height + 1;

        // Update max_height
        if (root->right->max_height > root->left->max_height)
            root->max_height = root->right->max_height + 1;
        else
            root->max_height = root->left->max_height + 1;
    }

    return root;
}

void Fix_Layer(PQ pq)
{
    if (DEBUG)
        printf("Fixing layer size:%d, max:%d\n", pq->size, pq->cur_max_index);

    if (pq->size == pq->cur_max_index)
    {
        pq->cur_min_index += (1 << pq->layer);
        pq->layer++;
        pq->cur_max_index += (1 << pq->layer);
    }
    else if (pq->size == pq->cur_min_index)
    {
        pq->cur_max_index -= (1 << pq->layer);
        pq->layer--;

        if (pq->cur_min_index != 0)
            pq->cur_min_index -= (1 << pq->layer);
    }
}

// Heap Insertion
void PQ_Insert(PQ pq, Pointer data)
{
    HeapNode new_node = malloc(sizeof(heap_node));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->min_height = 0;
    new_node->max_height = 0;

    if (pq->root == NULL)
    {
        pq->root = new_node;
        pq->layer = 1;
        pq->cur_max_index = 3;
        pq->cur_min_index = 0;
    }
    else
    {
        pq->root = Insert_Helper(pq, 1, pq->root, new_node);
    }

    pq->size++;
    Fix_Layer(pq);
    pq->last_inserted = new_node;
}

HeapNode FindLast(HeapNode root)
{
    int max = root->max_height;
    HeapNode cur = root;
    while (max != 0)
    {
        if (cur->right == NULL || cur->left == NULL)
            break;

        if (cur->left->max_height > cur->right->max_height)
            cur = cur->left;
        else
            cur = cur->right;

        max = cur->max_height;
    }

    if (cur->right != NULL)
        return cur->right;
    else if (cur->left != NULL)
        return cur->left;

    return cur;
}

void UpdateHeight(HeapNode node)
{
    // Update min_height
    if (node->right == NULL || node->left == NULL)
        node->min_height = 0;
    else if (node->left->min_height < node->right->min_height)
        node->min_height = node->left->min_height + 1;
    else
        node->min_height = node->right->min_height + 1;

    // Update max_height
    if (node->right == NULL && node->left == NULL)
        node->max_height = 0;
    else if (node->right == NULL)
        node->max_height = node->left->max_height + 1;
    else if (node->right->max_height > node->left->max_height)
        node->max_height = node->right->max_height + 1;
    else
        node->max_height = node->left->max_height + 1;

    if (node->parent != NULL)
        UpdateHeight(node->parent);
}

HeapNode FindBiggest(PQ pq, HeapNode node)
{
    HeapNode big_child;

    if (node->right == NULL && node->left == NULL)
        return node;
    else if(node->right == NULL)
        big_child = node->left;
    else if(node->left == NULL)
        big_child = node->right;
    else if(pq->compare(node->left->data, node->right->data) >= 0)
        big_child = node->left;
    else
        big_child = node->right;

    if(pq->compare(node->data, big_child->data) < 0)
        return big_child;
    else
        return node;
}

Pointer PQ_Pop(PQ pq)
{
    if (pq->root == NULL)
        return NULL;

    HeapNode last_cur = pq->last_inserted;
    HeapNode last_parent = last_cur->parent;

    Pointer result = pq->root->data;

    if(last_parent == NULL)
    {
        pq->root = NULL;
        pq->size--;
        Fix_Layer(pq);
        return result;
    }

    pq->root->data = last_cur->data;

    if (last_parent->left == last_cur)
        last_parent->left = NULL;
    else
        last_parent->right = NULL;

    free(last_cur);
    pq->size--;

    if(DEBUG)
        ShowHeap(pq->root, 0);

    UpdateHeight(last_parent);

    if(DEBUG)
        ShowHeap(pq->root, 0);

    pq->last_inserted = FindLast(pq->root);

    Fix_Layer(pq);

    HeapNode cur = pq->root;
    HeapNode swap;
    while(cur->left != NULL && cur->right != NULL)
    {
        swap = FindBiggest(pq,cur);
        if(swap == cur)
            break;
        else
        {
            Pointer temp = cur->data;
            cur->data = swap->data;
            swap->data = temp;
            cur = swap;
        }
    }

    return result;
}

void Free_Helper(PQ pq, HeapNode node)
{
    if (node->left != NULL)
        Free_Helper(pq, node->left);
    if (node->right != NULL)
        Free_Helper(pq, node->right);

    if(pq->destroy != NULL)
        pq->destroy(node->data);
    free(node);
}

void PQ_Destroy(PQ pq)
{
    Free_Helper(pq, pq->root);
    free(pq);
}

void ShowHeap(HeapNode node, int space)
{
    if (node == NULL)
        return; // Base case

    // Increase distance between levels
    space += 8;

    ShowHeap(node->right, space);

    printf("\n");
    for (int i = 8; i < space; i++)
        printf(" ");

    printf("%d(min:%d)(max:%d)\n", *((int *)node->data), node->min_height, node->max_height);

    // Process left child
    ShowHeap(node->left, space);
}