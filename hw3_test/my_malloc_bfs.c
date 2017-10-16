#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_BLOCK_SIZE_POW 32
#define BLOCK_AVAILABLE 0
#define BLOCK_ALLOCATED 1
#define BLOCK_SPLIT 2

/* Tree node struct */
typedef struct node
{
    struct node *parent;
    struct node *left;
    struct node *right;
    void *physical_addr;
    unsigned int block_size_pow;
    unsigned int state;
}node;

node *root = NULL;

/* Queue Functions and struct */
typedef struct q_node
{
    struct q_node *next;
    struct node *data;
}q_node;

typedef struct queue
{
    q_node *head;
    q_node *tail;
}queue;

void queue_append(queue *Q, node *n)
{
    q_node *q = NULL;
	if (n != NULL) {
		printf("Append to queue\n");
		q = (q_node *) malloc(sizeof(q_node));
		q -> next = NULL;
		q -> data = n;
		if (Q->head == NULL) {
		    Q->head = q;
		    Q->tail = q;
		}
		else {
		    Q->tail -> next = q;
		    Q->tail = q;
		}
	}
}

void queue_pop(queue *Q)
{
    q_node *q = Q->head -> next;
    free(Q->head);
    Q->head = q;
    if (NULL == Q->head) {
        Q->tail = NULL;
    }
}

void clear_queue(queue *Q)
{
	q_node *q = Q->head;
	while (q) {
		Q->head = q->next;
		free(q);
		q = Q->head;
	}
	Q->head = NULL;
	Q->tail = NULL;
}

/* Tree Functions */
node *initialize_node(node *parent, unsigned int block_size_pow)
{
    node *n = NULL;
    n = (node *) malloc(sizeof(node));
    n->parent = parent;
    n->left = NULL;
    n->right = NULL;
    n->physical_addr = NULL;
    n->block_size_pow = block_size_pow;
    n->state = BLOCK_AVAILABLE;
    return n;
}

node *split(node *parent, unsigned int req_block_size_pow)
{
	while ((req_block_size_pow < parent->block_size_pow)
			&& (parent->block_size_pow > 3)) {
		parent->left = initialize_node(parent, parent->block_size_pow - 1);
		parent->right = initialize_node(parent, parent->block_size_pow - 1);
		parent->state = BLOCK_SPLIT;
		parent = parent->left;
	}
	return parent;
}

void coalesce(node *parent)
{
    if ((BLOCK_AVAILABLE == parent -> left -> state) 
	 		&& (BLOCK_AVAILABLE == parent -> right -> state)) {
        free(parent -> left);
        free(parent -> right);
        parent -> state = BLOCK_AVAILABLE;
    }
    else {
        printf("Child is not free!\n");
        printf("Left child: %d\n", parent -> left -> state);
        printf("Right child: %d\n", parent -> right -> state);
    }
}

node *find_block(unsigned int req_block_size_pow)
{
    node *n = NULL;
    node *allocated_node = NULL;
    queue Q;
    Q.head = NULL;
    Q.tail = NULL;

    queue_append(&Q, root);
	
    while (Q.head != NULL) {
		printf("Find a new block for allocation\n");
		n = Q.head -> data;
		if ((req_block_size_pow == n-> block_size_pow)
				&& (BLOCK_AVAILABLE == n-> state)) {
			printf("new Node allocated");
			n -> state = BLOCK_ALLOCATED;
			allocated_node = n;
			break;
		}
		else if ((req_block_size_pow == n-> block_size_pow)
				&& (BLOCK_AVAILABLE != n-> state)) {
			printf("Leaf node unavailable\n");
            queue_pop(&Q);
        }
        else if ((req_block_size_pow < n-> block_size_pow)
				&& (BLOCK_SPLIT == n-> state)) {
			printf("Node has children\n");
            queue_append(&Q, n -> left);
            queue_append(&Q, n -> right);
            queue_pop(&Q);
        }
        else if ((req_block_size_pow < n-> block_size_pow)
				&& (BLOCK_ALLOCATED == n-> state)) {
			printf("Node is already allocated\n");
            queue_pop(&Q);
        }
        else if ((req_block_size_pow < n-> block_size_pow)
				&& BLOCK_AVAILABLE == n-> state) {
			printf("Split larger nodes\n");
			allocated_node = split(n, req_block_size_pow);
			break;
        }
    }
	clear_queue(&Q);
    return allocated_node;
}

void *my_malloc(size_t size)
{
    unsigned int req_block_size_pow = 3;
    size_t block_size = 8;
    node *allocated_node = NULL;
    void *addr = NULL;
    while (size > block_size) {
        block_size *= 2;
        req_block_size_pow++;
    }
    
    allocated_node = find_block(req_block_size_pow);
    if (NULL == allocated_node) {
        printf("Allocation failed!\n");
    }
    else {
    	addr = malloc(size);
    	allocated_node -> physical_addr = addr;
        printf("Allocated memory!\n");
    }
    return addr;
}

void free_block(void *addr)
{
	node *n = NULL;
	node *p = NULL;
	queue Q;
	Q.head = NULL;
	Q.tail = NULL;

	queue_append(&Q, root);

	while(Q.head != NULL) {
		n = Q.head -> data;
		if(addr == n-> physical_addr) {
			n -> physical_addr = NULL;
			n -> state = BLOCK_AVAILABLE;
			p = p -> parent;
			while(p) {
				if((BLOCK_AVAILABLE == p -> left -> state)
						&& (BLOCK_AVAILABLE == p -> right -> state)) {
					coalesce(p);
					p = p -> parent;
				}
				else {
					break;
				}
			}
		}
		else  {
			queue_append(&Q, n -> left);
			queue_append(&Q, n -> right);
			queue_pop(&Q);
		}
	}
	clear_queue(&Q);
}

void my_free(void *addr)
{
	free_block(addr);
	free(addr);
}

/* Print nodes at a given level */
void printGivenLevel(struct node* root, int level)
{
	if (root == NULL)
		return;
	if (level == 1)
		printf("%u\n", root->block_size_pow);
	else if (level > 1)
	{
		printGivenLevel(root->left, level-1);
		printGivenLevel(root->right, level-1);
	}
}

/* Compute the "height" of a tree -- the number of
 * nodes along the longest path from the root node
 * down to the farthest leaf node.*/
int height(struct node* node)
{
	if (node==NULL)
		return 0;
	else
	{
		/* compute the height of each subtree */
		int lheight = height(node->left);
		int rheight = height(node->right);

		/* use the larger one */
		if (lheight > rheight) {
			return(lheight+1);
		}
		else {
			return(rheight+1);
		}
	}
}

/* Function to print level order traversal a tree*/
void printLevelOrder(struct node* root)
{
	int h = height(root);
	int i = 0;
	for (i = 1; i <= h; i++)
		printGivenLevel(root, i);
}

int main()
{
    root = initialize_node(NULL, MAX_BLOCK_SIZE_POW);
	if (root == NULL)	
		printf("Unable to initialise root\n");
    my_malloc(3);
    my_malloc(3);
    my_malloc(3);
	printLevelOrder(root);
    return 0;
}
