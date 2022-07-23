#include <stdio.h>
#include <stdlib.h>
#include "RB.h"

typedef struct _node
{
    Pointer data; // Pointer tothe stored data
    NodePtr parent; // Pointer to the parent node
    NodePtr left;   // Pointer to the left child
    NodePtr right;  // Pointer to the right child
    int color;    // RED = 1 and BLACK = 0
} Node;

Node z = {NULL, NULL, NULL, NULL, 0};
NodePtr NullNode = &z;

typedef struct redblack
{
    NodePtr root;
    int count;
    DestroyFunc Destroy;
    CompareFunc Compare;
} rb;

void insertFix(RB tree,NodePtr node);
void deleteFix(RB tree, NodePtr DeppestAffected);

RB RB_Initialize(DestroyFunc Destroy,CompareFunc Compare)
{
    RB tree = malloc(sizeof(*tree));
    tree->count=0;

    tree->root = NullNode;
    tree->Destroy = Destroy;
    tree->Compare = Compare;

    return tree;
}

void FreeR(NodePtr root, void (*DestroyFunc)(Pointer))
{
    if(root == NullNode)
        return;

    if(root->left != NullNode)
        FreeR(root->left,DestroyFunc);

    if(root->right != NullNode)
        FreeR(root->right,DestroyFunc);

    //base case
    if(DestroyFunc != NULL)
        DestroyFunc(root->data);
    free(root);
    return;
}

void RB_Destroy(RB tree)
{
    FreeR(tree->root,tree->Destroy);
    free(tree);
}

NodePtr next(NodePtr node)
{
    while (node->left != NullNode)
        node = node->left;

    return node;
}

void rbTransplant(RB tree, NodePtr original, NodePtr replacement)
{
    if (original->parent == NULL)
    {
        tree->root = replacement;
    }
    else if (original == original->parent->left)
    {
        original->parent->left = replacement;
    }
    else
    {
        original->parent->right = replacement;
    }
    replacement->parent = original->parent;
}

void leftRotate(RB tree, NodePtr x)
{
    NodePtr y = x->right;
    x->right = y->left;
    if (y->left != NullNode)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL)
    {
        tree->root = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rightRotate(RB tree, NodePtr x)
{
    NodePtr y = x->left;
    x->left = y->right;
    if (y->right != NullNode)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL)
    {
        tree->root = y;
    }
    else if (x == x->parent->right)
    {
        x->parent->right = y;
    }
    else
    {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

// Deletion//
int RB_RemoveKey(RB tree, Pointer key)
{
    NodePtr node = tree->root;

    NodePtr NodeToDelete = NullNode;
    NodePtr DeppestAffected, ReplaceNode;
    // Searching for the asked key
    while (node != NullNode)
    {
        if (tree->Compare(node->data, key) == 0)
        {
            NodeToDelete = node;
            break;
        }

        if (tree->Compare(node->data,key) < 0)
        {
            node = node->right;
        }
        else
        {
            node = node->left;
        }
    }


    if (NodeToDelete == NullNode) // Case the key is not found
    {
        return -1;
    }

    ReplaceNode = NodeToDelete;
    int rn_original_color = ReplaceNode->color;
    // When the node has only got 1 child we can just replace
    // it whith its child.Also if the node hasnt got any childs
    if (NodeToDelete->left == NullNode)
    {
        DeppestAffected = NodeToDelete->right;
        rbTransplant(tree, NodeToDelete, NodeToDelete->right);
    }
    else if (NodeToDelete->right == NullNode)
    {
        DeppestAffected = NodeToDelete->left;
        rbTransplant(tree, NodeToDelete, NodeToDelete->left);
    }
    // Case where the node has 2 childs
    else
    {
        ReplaceNode = next(NodeToDelete->right);
        // find the next,inorder,node to replace the deleted
        rn_original_color = ReplaceNode->color;
        DeppestAffected = ReplaceNode->right;
        if (ReplaceNode->parent == NodeToDelete)
        {
            DeppestAffected->parent = ReplaceNode;
        }
        else
        {
            rbTransplant(tree, ReplaceNode, ReplaceNode->right);
            ReplaceNode->right = NodeToDelete->right;
            ReplaceNode->right->parent = ReplaceNode;
        }

        rbTransplant(tree, NodeToDelete, ReplaceNode);
        ReplaceNode->left = NodeToDelete->left;
        ReplaceNode->left->parent = ReplaceNode;
        ReplaceNode->color = NodeToDelete->color;
    }

    if(tree->Destroy != NULL)
        tree->Destroy(NodeToDelete->data);
        
    free(NodeToDelete);

    if (rn_original_color == 0)
    {
        deleteFix(tree, DeppestAffected);
    }
    tree->count--;
    return 0;
}

// For balancing the tree after deletion
void deleteFix(RB tree, NodePtr DeppestAffected)
{
    NodePtr sibling;
    //if(DeppestAffected == NullNode) return;

    while (DeppestAffected != tree->root && DeppestAffected->color == 0)
    {
        if (DeppestAffected == DeppestAffected->parent->left)
        {
            sibling = DeppestAffected->parent->right;
            if (sibling->color == 1)
            {
                sibling->color = 0;
                DeppestAffected->parent->color = 1;
                leftRotate(tree, DeppestAffected->parent);
                sibling = DeppestAffected->parent->right;
            }

            if (sibling->left->color == 0 && sibling->right->color == 0)
            {
                sibling->color = 1;
                DeppestAffected = DeppestAffected->parent;
            }
            else
            {
                if (sibling->right->color == 0)
                {
                    sibling->left->color = 0;
                    sibling->color = 1;
                    rightRotate(tree, sibling);
                    sibling = DeppestAffected->parent->right;
                }

                sibling->color = DeppestAffected->parent->color;
                DeppestAffected->parent->color = 0;
                sibling->right->color = 0;
                leftRotate(tree, DeppestAffected->parent);
                DeppestAffected = tree->root;
            }
        }
        else
        {
            sibling = DeppestAffected->parent->left;
            if (sibling->color == 1)
            {
                sibling->color = 0;
                DeppestAffected->parent->color = 1;
                rightRotate(tree, DeppestAffected->parent);
                sibling = DeppestAffected->parent->left;
            }

            if (sibling->left->color == 0 && sibling->right->color == 0)
            {
                sibling->color = 1;
                DeppestAffected = DeppestAffected->parent;
            }
            else
            {
                if (sibling->left->color == 0)
                {
                    sibling->right->color = 0;
                    sibling->color = 1;
                    leftRotate(tree, sibling);
                    sibling = DeppestAffected->parent->left;
                }

                sibling->color = DeppestAffected->parent->color;
                DeppestAffected->parent->color = 0;
                sibling->left->color = 0;
                rightRotate(tree, DeppestAffected->parent);
                DeppestAffected = tree->root;
            }
        }
    }
    DeppestAffected->color = 0;
}

// Deleteion//

// Inseetion//

int RB_InsertKey(RB tree, Pointer key)
{
    NodePtr node = malloc(sizeof(*node));
    node->parent = NULL;
    node->data = key;
    node->left = NullNode;
    node->right = NullNode;
    node->color = 1;

    NodePtr ansestor = NULL;
    NodePtr x = tree->root;

    while (x != NullNode)
    {
        ansestor = x;
        if(tree->Compare(node->data, x->data)==0)
            return -1;


        if(tree->Compare(node->data, x->data)<0)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    node->parent = ansestor;
    if (ansestor == NULL)
    {
        tree->root = node;
    }
    else if (tree->Compare(node->data, ansestor->data)<0)
    {
        ansestor->left = node;
    }
    else
    {
        ansestor->right = node;
    }

    if (node->parent == NULL)
    {
        node->color = 0;
        tree->count++;
        return 0;
    }

    if (node->parent->parent == NULL)
    {
        tree->count++;
        return 0;
    }

    insertFix(tree,node);
    tree->count++;
    return 0;
}

// For balancing the tree after insertion
void insertFix(RB tree,NodePtr node)
{
    NodePtr uncle;
    while (node->parent->color == 1)
    {
        if (node->parent == node->parent->parent->right) //If parent is the right child of grand parent
        {
            uncle = node->parent->parent->left;
            if (uncle->color == 1) //Case where parent and uncle is RED
            {
                uncle->color = 0;
                node->parent->color = 0;
                node->parent->parent->color = 1;
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->left)
                {   
                    node = node->parent;
                    rightRotate(tree,node);
                }
                node->parent->color = 0;
                node->parent->parent->color = 1;
                leftRotate(tree,node->parent->parent);
            }
        }
        else
        {
            uncle = node->parent->parent->right;

            if (uncle->color == 1)
            {
                uncle->color = 0;
                node->parent->color = 0;
                node->parent->parent->color = 1;
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->right)
                {
                    node = node->parent;
                    leftRotate(tree,node);
                }
                node->parent->color = 0;
                node->parent->parent->color = 1;
                rightRotate(tree,node->parent->parent);
            }
        }
        if (node == tree->root)
        {
            break;
        }
    }
    tree->root->color = 0;
}

// Insertion//

// Search//

NodePtr SearchTreeRec(RB tree,NodePtr node, Pointer key)
{
    if(node == NullNode)
    {
        return NULL;
    }

    if (tree->Compare(key, node->data) == 0)
    {
        return node;
    }

    if (key < node->data)
    {
        return SearchTreeRec(tree, node->left, key);
    }
    return SearchTreeRec(tree, node->right, key);
}

NodePtr RB_Search(RB tree, Pointer key)
{
    return(SearchTreeRec(tree,tree->root,key));
}

// Search//


//InOrder Traversal//

void InOrderHelper(void(*Visit)(Pointer),NodePtr node)
{
    if(node == NullNode)
        return;

    InOrderHelper(Visit,node->left);

    Visit(node->data);

    InOrderHelper(Visit,node->right);
}

void RB_PrintElements(RB tree,void(*Visit)(Pointer))
{
    InOrderHelper(Visit,tree->root);
}

//InOrder Traversal//

Pointer RB_DataFromNode(NodePtr node)
{
    return(node->data);
}

void PrintRed()
{
    printf("\033[1;31m");
}

void resetcol()
{
    printf("\033[0m");
}

void print2DUtil(NodePtr node, int space)
{
    if (node == NullNode) return;  // Base case
 
    // Increase distance between levels
    space += 8;

    print2DUtil(node->right, space);
 
    printf("\n");
    for (int i = 8; i < space; i++)
        printf(" ");
    if(node->color == 1)
        PrintRed();
    printf("%d\n", *((int*)node->data));
    resetcol();

    // Process left child
    print2DUtil(node->left, space);
}
 
void RB_print2D(RB tree)
{
   // Initialize space count as 0
   print2DUtil(tree->root, 0);
   printf("\n");
}



