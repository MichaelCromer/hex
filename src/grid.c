#include <stdlib.h>
#include <stdbool.h>

#include "grid.h"


struct Queue {
    int score;
    struct Hex *hex;
    struct Queue *next;
};

struct Coordinate {
    int x;
    int y;
    int z;
};

struct Hex {
    struct Coordinate *c;
    enum TERRAIN t;
    struct Hex *n[6];
};


/*
 *      QUEUE Functions
 */

struct Queue *queue_create(int score, struct Hex *hex)
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


struct Queue *queue_find(struct Queue **head, struct Hex *hex)
{
    struct Queue *curr = *head;
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
    struct Queue *new_node = queue_create(new_score, new_hex);

    /* special case of adding to start */
    if ((*head == NULL) || (*head)->score > new_node->score) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    /* main case: loop through to find correct position */
    struct Queue *curr = *head;
    while ((curr->next != NULL) && (curr->next->score <= new_node->score)) {
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
        queue_destroy(curr);
    }

    return;
}



/*
 *      COORDINATE Functions
 */

struct Coordinate d_ee = {  0,  1, -1 };
struct Coordinate d_ne = { -1,  1,  0 };
struct Coordinate d_nw = { -1,  0,  1 };
struct Coordinate d_ww = {  0, -1,  1 };
struct Coordinate d_sw = {  1, -1,  0 };
struct Coordinate d_se = {  1,  0, -1 };


enum DIRECTION direction_opposite(enum DIRECTION d)
{
    enum DIRECTION opp;
    switch (d) {
        case EAST:
            opp = WEST;
            break;
        case NORTHEAST:
            opp = SOUTHWEST;
            break;
        case NORTHWEST:
            opp = SOUTHEAST;
            break;
        case WEST:
            opp = EAST;
            break;
        case SOUTHWEST:
            opp = NORTHEAST;
            break;
        case SOUTHEAST:
            opp = NORTHWEST;
            break;
    }
    return opp;
}

bool coordinate_equals(struct Coordinate *c0, struct Coordinate *c1)
{
    return ((c0->x == c1->x) && (c0->y == c1->y) && (c0->z == c1->z));
}

int coordinate_distance(struct Coordinate *c0, struct Coordinate *c1)
{
    int dx = abs(c0->x - c1->x);
    int dy = abs(c0->y - c1->y);
    int dz = abs(c0->z - c1->z);

    int dc = 0;
    dc = (dx > dy) ? dx : dy;
    dc = (dz > dc) ? dz : dc;

    return dc;
}

void coordinate_add(struct Coordinate *c1, struct Coordinate *c2, struct Coordinate *a)
{
    a->x = c1->x + c2->x;
    a->y = c1->y + c2->y;
    a->z = c1->z + c2->z;

    return;
}

void coordinate_delta(struct Coordinate *c, enum DIRECTION d, struct Coordinate *a)
{
    switch (d) {
        case EAST:
            coordinate_add(c, &d_ee, a);
            break;
        case NORTHEAST:
            coordinate_add(c, &d_ne, a);
            break;
        case NORTHWEST:
            coordinate_add(c, &d_nw, a);
            break;
        case WEST:
            coordinate_add(c, &d_ww, a);
            break;
        case SOUTHWEST:
            coordinate_add(c, &d_sw, a);
            break;
        case SOUTHEAST:
            coordinate_add(c, &d_se, a);
            break;
    }
    return;
}

/*
 *      HEX Functions
 */

struct Hex *hex_create(void)
{
    struct Hex *h = malloc(sizeof(struct Hex));
    if (h == NULL) {
        return NULL;
    }

    h->c = malloc(sizeof(struct Coordinate));
    if (h->c == NULL) {
        free(h);
        return NULL;
    }

    (h->c)->x = 0;
    (h->c)->y = 0;
    (h->c)->z = 0;
    h->t = NONE;
    for (int i=0; i<6; i++) {
        h->n[i] = NULL;
    }

    return h;
}


void hex_destroy(struct Hex *h)
{
    free(h->c);
    free(h);
    return;
}


void hex_set_terrain(struct Hex *h, enum TERRAIN t)
{
    h->t = t;
    return;
}


enum TERRAIN hex_get_terrain(struct Hex *h)
{
    return h->t;
}


int hex_x(struct Hex *h)
{
    return h->c->x;
}


int hex_y(struct Hex *h)
{
    return h->c->y;
}


int hex_z(struct Hex *h)
{
    return h->c->z;
}


struct Hex *hex_find(struct Hex *start, struct Coordinate *target)
{
    struct Queue *open = NULL;
    struct Queue *closed = NULL;
    struct Hex *end = NULL; /* might not exist */

    /* start with the current hex */
    queue_add(&open, start, coordinate_distance(start->c, target));

    while (open) {
        struct Hex *curr = open->hex;

        /* we found it, end here */
        if (coordinate_equals(curr->c, target)) {
            end = curr;
            break;
        }

        /* open each unclosed non-null neighbour */
        for (int i=0; i<6; i++) {
            struct Hex *nbr = (curr->n)[i];

            /* skip if null or already closed */
            if (nbr == NULL || queue_find(&closed, nbr)) {
                continue;
            }

            queue_add(&open, nbr, coordinate_distance(nbr->c, target));
        }

        /* close the current open head */
        queue_add(&closed, curr, 0);
        queue_remove(&open, curr);

    }

    /* we did it, let's clean up and get out of here */
    queue_destroy(open);
    queue_destroy(closed);
    return end; /* NULL if target not found */
}


void hex_create_neighbour(struct Hex *h, enum DIRECTION d)
{
    /* create the hex with the right coords */
    struct Hex *nbr = hex_create();
    coordinate_delta(h->c, d, nbr->c);

    /* assign the new neighbour */
    h->n[d] = nbr;
    nbr->n[direction_opposite(d)] = h;

    /* find the neighbour's neighbourhood */
    struct Coordinate dc = {0, 0, 0};
    for (int i = WEST; i <= SOUTHEAST; i++) {
        coordinate_delta(nbr->c, i, &dc);
        struct Hex *nbr_nbr = hex_find(nbr, &dc);
        if (nbr_nbr) {
            (nbr->n)[i] = nbr_nbr;
        }
    }
    
    return;
}
