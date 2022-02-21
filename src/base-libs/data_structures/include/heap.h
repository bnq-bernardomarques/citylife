#pragma once

#include "queue.h"
#include "list.h"

typedef struct _active_record {
    // Nodes pointing to this record are active if and only if the flag is true.
    char is_active;

    // The number of nodes pointing to the record. If flag is false and ref-count is 0, then the node can be free'd
    int ref_count;
} active_record;

typedef struct _heap_node {
    // Number of nodes below this one
    int size;

    // is active or passive?
    active_record *activity;

    // Number of active children (if the node is active)
    int rank; // TODO: Should this be a pointer to the rank list?

    // ??? Total loss of an heap is the sum of the loss over all active nodes
    unsigned loss; // TODO: Is this being correctly updated?

    // The item being stored in this heap node
    void *item;

    // The parent of the node
    struct _heap_node *parent;

    // The relative position of the node with respect to its siblings in the parent node.
    list_node *relative_position_to_siblings; // TODO: Update and use this value

    // The node's position in heap->Q
    queue_item *position_in_q;

    // List of the children of this node
    list *children;
} heap_node;

typedef struct _heap {
    // Total number of nodes in the heap
    int size;

    // Root node of the heap
    heap_node *root;

    // Returns -1 if a < b, 1 if a > b and 0 if they're equal
    int (*compare)(const void *a, const void *b);

    active_record *active_record;

    // All nodes except the root are kept in this queue
    queue *Q;

} heap;

heap *heap_new(int (*compare)(const void *a, const void *b));

void heap_destroy(heap *h);

heap *heap_push(heap *h, void *item);

heap *heap_meld(heap *h1, heap *h2);

void *heap_find_min(heap *h);

void *heap_delete_min(heap *h);

void heap_delete(heap *h, void *e);

void heap_decrease_key(heap *h, heap_node *x, void *new_item);
