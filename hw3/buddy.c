#include "buddy.h"
#include <unistd.h>  // for sysconf()
#include <string.h>
#include "utility.h"
#include <assert.h>

//1MB initial memory in bytes
#define INITIAL_MEMORY (4096)

buddy_node_t *root = NULL;
buddy_node_t pool[24];
uint8_t pool_free[24];

void _init_buddy_node_pool()
{
    int i = 0;

    for (i = 0; i < 24; i++) {
        memset(&pool[i], 0, sizeof(pool[i]));
        pool[i].free = 1;
        pool_free[i] = 1;
    }
}
 
//return a node structure from pool of buddy_nodes
buddy_node_t *_get_node()
{
    int i = 0;

    for (i = 0; i < 24; i++) {
        if (1 == pool_free[i]) {
            pool_free[i] = 0;
            return &pool[i];
        }
    }
    return NULL;
}

void _give_up_node(buddy_node_t *node)
{
    int i = 0;

    for (i = 0; i < 24; i++) {
        if (node == &pool[i]) {
            pool_free[i] = 1;
        }
    }
}

void print_pool()
{
    int i = 0;

    for (i = 0; i < 24; i++) {
        printf("Pool Node %d: Free %d\n", i, pool_free[i]);
    }
}

void print_tree(buddy_node_t *root)
{
    if (root != NULL) {
        printf("Tree Node Size: %zu, Free %d, Address %p\n", root->size, root->free, root->start);
        print_tree(root->left);
        print_tree(root->right);
    }
}

void mark_parent_as_allocated(buddy_node_t *root, buddy_node_t *node)
{
    //Inefficient
    if (root != NULL && root != node) {
        if (root->left == node || root->right == node) {
            if (root->left->free == 0 && root->right->free == 0) {
                root->free = 0;
                return;
            }
        }
        mark_parent_as_allocated(root->left, node);
        mark_parent_as_allocated(root->right, node);
    }
}

void *_setup_metadata(buddy_node_t *node, size_t size)
{
    if (node) {
        if (node->size == size) {
            node->free = 0;
            mark_parent_as_allocated(root, node);
            return node->start;
        }
        //split
        node->left = _get_node();
        assert(node->left != NULL);
        node->left->free = 1;
        node->left->size = node->size / 2;
        node->left->start = node->start;

        node->right = _get_node();
        assert(node->right != NULL);
        node->right->free = 1;
        node->right->size = node->size / 2;
        node->right->start = (uint8_t *)node->start + node->left->size;

        return _setup_metadata(node->left, size);
    }


}

buddy_node_t *_check_for_available_block(buddy_node_t *root, size_t size)
{
    //check if a block is of size >= to request
    buddy_node_t *mem = NULL;
    if (root != NULL && root->size >= size) {
        debug("NODE - size %zu, free %d", root->size, root->free);
        //if it has children, see about the children
        if (root->free && root->left == NULL) {
            if (size == root->size) {
                mem = root;
                goto out;
            }
        }
            mem = _check_for_available_block(root->left, size);
            if (NULL != mem)
                goto out;
            mem = _check_for_available_block(root->right, size);
            if (NULL != mem)
                goto out;
            mem = root;
        
    }

out:
    return mem;
}

//obtain memory
void *_request_from_kernel(size_t bytes)
{
    size_t page_size = 0;
    uint32_t pg_count = 0;
    void *mem = NULL;

    page_size = sysconf(_SC_PAGESIZE);
    debug("page size is %zu", page_size);
    pg_count = bytes / page_size;  // all calculations are in bytes here
    if (pg_count < 1) 
        pg_count = 1;

    debug("Requesting %u page(s)", pg_count);
    mem = sbrk(pg_count);
    if (NULL == mem) {
        debug("Failed to increase size of heap");
        goto out;
    }
    debug("New end of heap %p", mem);

out:
    return mem;
}

void *_alloc_memory(size_t size)
{
	buddy_node_t *temp = NULL;
    void *mem = NULL;
	
	if (NULL == root) {
        //if this is first call
        //_request_from_kernel()

        _init_buddy_node_pool();
        root = _get_node();
        debug("%p %u", root, root->free);
        assert(root != NULL);
        root->start = _request_from_kernel(INITIAL_MEMORY);
        root->size = INITIAL_MEMORY;
        assert(root->start != NULL);
	}

    //check satisfaction, request for more if can't satisfy
    temp = _check_for_available_block(root, size);
    if (NULL == temp) {
        debug("TODO request for more memory");
        //TODO request for more memory
        goto null_return;
    }
    debug("Available node: size %zu, free %d", temp->size, temp->free);
    //set_up_metadata
    mem = _setup_metadata(temp, size);
    print_tree(root);

mem_return:
    return mem;
null_return:
    return NULL;
}

void _mark_parent_as_free(buddy_node_t *root, buddy_node_t *node)
{
    //Inefficient
    if (root != NULL) {
        debug("root: %p", root);
        debug("l: %p | r: %p | node: %p", root->left, root->right, node);
        if (root->left == node || root->right == node) {
            root->free = 1;
            if (root->left->free == 1 && root->right->free == 1) {
                //coalescing
                debug("coalescing...");
                _give_up_node(root->left);
                root->left = NULL;
                _give_up_node(root->right);
                root->right = NULL;
                return;
            }
        }
        _mark_parent_as_free(root->left, node);
        _mark_parent_as_free(root->right, node);
    }
}

buddy_node_t *_mark_node_as_free(buddy_node_t *root, void *mem)
{
    buddy_node_t *temp = NULL;
    if (NULL != root) {
        temp = _mark_node_as_free(root->left, mem);
        if (NULL != temp)
            return temp;
        debug("Current node has start address %p", root->start);
        if (root->start == mem) {
            root->free = 1;
            return root;
        }
        temp = _mark_node_as_free(root->right, mem);
        if (NULL != temp)
            return temp;
    }
    return NULL;
}

void _reclaim_memory(void *mem)
{
    buddy_node_t * temp =  NULL;

    //find the block and mark it free
    temp = _mark_node_as_free(root, mem);
    if (NULL == temp) {
        debug("GOT NULL from free");
        goto out;
    }
    //mark the parent free if it is not
    //try coalescing blocks
    _mark_parent_as_free(root, temp);
out:
    debug("after reclaiming memory");
    print_tree(root);
    return;
}

