#include <stdio.h>
#include <stdlib.h>

#include "avl.h"



struct Tree
{
    struct Node *root;
    size_t size;
    comp_it comp;
    free_it free_data;
    get_it get_data;
};


avl_tree *
create_avl_tree(comp_it comp, free_it free_data, get_it get_data)
{
    avl_tree *tree = malloc(sizeof(*tree));
    if (!tree)
    {
        fprintf(stderr, "not enough memory\n");
        return NULL;
    }

    tree->root = NULL;
    tree->size = 0;
    tree->comp = comp;
    tree->free_data = free_data;
    tree->get_data = get_data;

    return tree;
}

int
max(int a, int b)
{
    return (a > b)? a : b;
}

// recursive fuction that gets the hight of a specific node N within the tree
int height(struct Node *N)
{
    if (N == NULL)
    {
        return 0;
    }
    return 1 + max(height(N->left), height(N->right));
}  

// helper function that allocates for a new node with data and left/right set to NULL
struct Node* newNode(void *data)
{
    struct Node *node = malloc(sizeof(*node));
    if (!node)
    {
        fprintf(stderr, "not enough memory\n");
        return NULL;
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    return node;
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
struct Node *rightRotate(struct Node *y)
{
    struct Node *x = y->left;
    struct Node *T2 = x->right;
 
    // Perform rotation
    x->right = y;
    y->left = T2;
 
    // Update heights
    y->height = height(y);
    x->height = height(x);
 
    // Return new root
    return x;
}
 
// A utility function to left rotate subtree rooted with x
// See the diagram given above.
struct Node *leftRotate(struct Node *x)
{
    struct Node *y = x->right;
    struct Node *T2 = y->left;
 
    // Perform rotation
    y->left = x;
    x->right = T2;
 
    //  Update heights
    x->height = height(x);
    y->height = height(y);
 
    // Return new root
    return y;
}
 
// Get Balance factor of node N
int getBalance(struct Node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}
 
// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
struct Node* insert(struct Node* node, void *data, comp_it comp)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return(newNode(data));
 
    if (comp(data, node->data) < 0)
        node->left  = insert(node->left, data, comp);
    else if (comp(data, node->data) > 0)
        node->right = insert(node->right, data, comp);
    else // Equal keys are not allowed in BST
        return node;
 
    /* 2. Update height of this ancestor node */
    node->height = height(node);
 
    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);
 
    // If this node becomes unbalanced, then
    // there are 4 cases
 
    // Left Left Case
    if (balance > 1 && comp(data, node->left->data) < 0)
        return rightRotate(node);
 
    // Right Right Case
    if (balance < -1 && comp(data, node->right->data) > 0)
        return leftRotate(node);
 
    // Left Right Case
    if (balance > 1 && comp(data, node->left->data) > 0)
    {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }
 
    // Right Left Case
    if (balance < -1 && comp(data, node->right->data) < 0)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
 
    /* return the (unchanged) node pointer */
    return node;
}
 
// A utility function to print preorder traversal
// of the tree.
// The function also prints height of every node
void
preOrder(struct Node *root, get_it get_data)
{
    if(root != NULL)
    {
        int *num = (int *) get_data(root->data);
        printf("%d ", *num);
        preOrder(root->left, get_data);
        preOrder(root->right, get_data);
    }
}


int
insert_node(avl_tree *tree, void *data)
{
    tree->root =  insert(tree->root, data, tree->comp);
    if (!tree->root)
    {
        fprintf(stderr, "insert failed\n");
        return -1;
    }
    return 0;
}

/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
struct Node * minValueNode(struct Node* node)
{
    struct Node* current = node;
 
    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;
 
    return current;
}


// Recursive function to delete a node with given key
// from subtree with given root. It returns root of
// the modified subtree.
struct Node* deleteNode(struct Node* root, void *data, comp_it comp)
{
    // STEP 1: PERFORM STANDARD BST DELETE
 
    if (root == NULL)
        return root;
 
    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if ( comp(data, root->data) < 0)
        root->left = deleteNode(root->left, data, comp);
 
    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if(comp(data, root->data) > 0)
        root->right = deleteNode(root->right, data, comp);
 
    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            struct Node *temp = root->left ? root->left :
                                             root->right;
 
            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else // One child case
             *root = *temp; // Copy the contents of
                            // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            struct Node* temp = minValueNode(root->right);
 
            // Copy the inorder successor's data to this node
            root->data = temp->data;
 
            // Delete the inorder successor
            root->right = deleteNode(root->right, temp->data, comp);
        }
    }
 
    // If the tree had only one node then return
    if (root == NULL)
      return root;
 
    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(height(root->left),
                           height(root->right));
 
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance(root);
 
    // If this node becomes unbalanced, then there are 4 cases
 
    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
 
    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left =  leftRotate(root->left);
        return rightRotate(root);
    }
 
    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
 
    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
 
    return root;
}


int remove_node(avl_tree *tree, void *data)
{
    tree->root =  deleteNode(tree->root, data, tree->comp);
    if (!tree->root)
    {
        fprintf(stderr, "error deleting node\n");
        return -1;
    }
    return 0;
}


bool search(avl_tree *tree, void *data)
{
     /* 1.  Perform the normal BST insertion */
    if (!tree || !data)
    {
        return false;
    }

    struct Node *n = tree->root;
    while (n != NULL)
    {
        if (tree->comp(data, n->data) < 0)
            n = n->left;
        else if (tree->comp(data, n->data) > 0)
            n = n->right;
        else // Equal keys are not allowed in BST
            return true;
    }
    return false;
}


void
delete_tree(struct Node *node, free_it free_data)
{
    if (node == NULL)
    {
        return;
    }
    
    delete_tree(node->left, free_data);
    delete_tree(node->right, free_data);

    free_data(node->data);
    node->data = NULL;
    free(node);
    node = NULL;

    return;
}

//destroy!!!
void
destroy_tree(avl_tree *tree)
{
    if (!tree)
    {
        return;
    }

    delete_tree(tree->root, tree->free_data);

    free(tree);

    tree = NULL;

    return;
}