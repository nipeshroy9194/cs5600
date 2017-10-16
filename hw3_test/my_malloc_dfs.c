#include <stdio.h>
#include <stdlib.h>

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

/* Stack Funtions and structs */

typedef struct s_node
{
	struct s_node *next;
	struct node *data;
}s_node;

typedef struct stack
{
	s_node *top;
}stack;

void stack_push(stack *S, node *n)
{
	s_node *s;
	if(n != NULL) {
		s = (s_node *) malloc(sizeof(s_node));
		s->data = n;
		s->next = S->top;
		S->top = s;		
	}
	else {
		printf("Cannot push empty node!\n");
	}
}

void stack_pop(stack *S)
{
	if(S->top) {
		s_node *s = S->top->next;
		free(S->top);
		S->top = s;
	}
	else {
		printf("Stack is empty!");
	}
}

void clear_stack(stack *S)
{
	while(S->top) {
		stack_pop(S);
	}
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
	if ((BLOCK_AVAILABLE == parent->left->state) 
			&& (BLOCK_AVAILABLE == parent->right->state)) {
		free(parent->left);
		free(parent->right);
		parent->state = BLOCK_AVAILABLE;
	}
	else {
		printf("Child is not free!\n");
		printf("Left child: %d\n", parent->left->state);
		printf("Right child: %d\n", parent->right->state);
	}
}

node *check(node *n, unsigned int req_block_size_pow) 
{
	node *allocated_node = NULL;
	printf("Check function called!\n");
	if ((req_block_size_pow == n-> block_size_pow)
			&& (BLOCK_AVAILABLE == n-> state)) {
		printf("new Node allocated");
		n->state = BLOCK_ALLOCATED;
		allocated_node = n;
	}
	else if ((req_block_size_pow == n-> block_size_pow)
			&& (BLOCK_AVAILABLE != n-> state)) {
		printf("Leaf node unavailable\n");
	}
	else if ((req_block_size_pow < n-> block_size_pow)
			&& (BLOCK_SPLIT == n-> state)) {
		printf("Node has children\n");
	}
	else if ((req_block_size_pow < n-> block_size_pow)
			&& (BLOCK_ALLOCATED == n-> state)) {
		printf("Node is already allocated\n");
	}
	else if ((req_block_size_pow < n-> block_size_pow)
			&& BLOCK_AVAILABLE == n-> state) {
		printf("Split larger nodes\n");
		allocated_node = split(n, req_block_size_pow);
		allocated_node->state = BLOCK_ALLOCATED;
	}
	return allocated_node;
}

node *find_block(unsigned int req_block_size_pow)
{
	node *n = NULL;
	node *allocated_node = NULL;
	stack S;
	S.top = NULL;

	n = root;

	while(1) {
		if(NULL != n) {
			stack_push(&S, n);                                               
			n = n->left;  
		}
		else {
			if (NULL != S.top) {
				n = S.top->data;
				stack_pop(&S);
				allocated_node = check(n, req_block_size_pow);
				if(NULL != allocated_node) {
					break;
				}
				n = n->right;
			}
			else {
				break;
			} 
		}
	}
	clear_stack(&S);
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
		/* commented so that we dont run out of ram can change dis later for
		 * lower allocations
		 */
		//addr = malloc(size);
		addr = malloc(1);
		allocated_node->physical_addr = addr;
		printf("Allocated memory!\n");
	}
	return addr;
}

/* For searching the tree not using BFS nor DFS */

void free_block(void *addr)
{
	node *n = NULL;
	node *p = NULL;
	stack S;
	S.top = NULL;

	stack_push(&S, root);

	while(S.top != NULL) {
		n = S.top->data;
		if(addr == n-> physical_addr) {
			n->physical_addr = NULL;
			n->state = BLOCK_AVAILABLE;
			p = p->parent;
			while(p) {
				if((BLOCK_AVAILABLE == p->left->state)
						&& (BLOCK_AVAILABLE == p->right->state)) {
					coalesce(p);
					p = p->parent;
				}
				else {
					break;
				}
			}
		}
		else  {
			if(n->right != NULL)
				stack_push(&S, n->right);
			if(n->left != NULL)
				stack_push(&S, n->left);
			stack_pop(&S);
		}
	}
	clear_stack(&S);
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
		printf("%u %p\n", root->block_size_pow, root->physical_addr);
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
	for(int i = 0; i < 2; i++) {
		printf("\nALLOCATION %d\n", i+1);
		my_malloc(1073741824);
		printLevelOrder(root);	
	}
	printf("\n\nALLOCATION 5");
	my_malloc(2273741824);
	printLevelOrder(root);
	return 0;
}
