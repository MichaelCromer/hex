#ifndef QUEUE_H
#define QUEUE_H

#include "grid.h"

struct Queue;

struct Queue *queue_create(struct Hex *hex, int score);
void queue_destroy(struct Queue *queue);
struct Hex *queue_hex(struct Queue *queue);
int queue_score(struct Queue *q);
struct Queue *queue_next(struct Queue *queue);
struct Queue *queue_find(struct Queue *head, struct Hex *hex);
void queue_add(struct Queue **head, struct Hex *new_hex, int new_score);
void queue_remove(struct Queue **head, struct Hex *hex);

#endif
