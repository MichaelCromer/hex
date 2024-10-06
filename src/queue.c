#include <stdlib.h>

#include "queue.h"


struct Queue {
    int score;
    struct Hex *hex;
    struct Queue *next;
};


struct Queue *queue_create(struct Hex *hex, int score)
{
    struct Queue *queue = malloc(sizeof(struct Queue));
    queue->score = score;
    queue->hex = hex;
    queue->next = NULL;

    return queue;
}


void queue_destroy(struct Queue *queue)
{
    struct Queue *tmp;
    while (queue != NULL) {
        tmp = queue;
        queue = queue->next;
        free(tmp);
    }
}


struct Hex *queue_hex(struct Queue *queue)
{
    return queue->hex;
}


int queue_score(struct Queue *q)
{
    return q->score;
}


struct Queue *queue_next(struct Queue *queue)
{
    return queue->next;
}


struct Queue *queue_find(struct Queue *head, struct Hex *hex)
{
    if (hex == NULL) {
        return NULL;
    }

    struct Queue *curr = head;
    while (curr != NULL) {
        if (curr->hex == hex) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


void queue_add(struct Queue **head, struct Hex *new_hex, int new_score)
{
    /* null pointer handling */
    if ((head == NULL) || (new_hex == NULL)) {
        return;
    }

    /* create new node for new entry */
    struct Queue *new_node = queue_create(new_hex, new_score);

    /* special case of adding to start */
    if ((*head == NULL) || (*head)->score > new_node->score) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    /* main case: loop through to find correct position */
    struct Queue *curr = *head;
    while ((curr->next != NULL) && (curr->next->score < new_node->score)) {
        curr = curr->next;
    }
    new_node->next = curr->next;
    curr->next = new_node;

    return;
}


void queue_remove(struct Queue **head, struct Hex *hex)
{
    /* null pointer handling */
    if ((head == NULL) || (*head == NULL) || (hex == NULL)) {
        return;
    }

    struct Queue *prev = NULL;
    struct Queue *curr = *head;

    /* loop through until we find the matching hex or run off the end */
    while ((curr != NULL) && (curr->hex != hex)) {
        prev = curr;
        curr = curr->next;
    }

    /* if we didn't run off the end, destroy the node and patch the list */
    if (curr != NULL) {
        if (prev == NULL) {
            *head = curr->next;
        } else {
            prev->next = curr->next;
        }
        curr->next = NULL;
        queue_destroy(curr);
    }

    return;
}
