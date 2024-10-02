#include <stdlib.h>
#include <stdbool.h>

#include "grid.h"


struct SearchNode {
    int score;
    struct Hex *hex;
    struct SearchNode *next;
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
 *      SEARCHNODE Functions
 */

struct SearchNode *snode_create(int score, struct Hex *hex)
{
    struct SearchNode *snode = malloc(sizeof(struct SearchNode));
    snode->score = score;
    snode->hex = hex;
    snode->next = NULL;

    return snode;
}

void snode_destroy(struct SearchNode *snode)
{
    struct SearchNode *tmp;
    while (snode != NULL) {
        tmp = snode;
        snode = snode->next;
        free(tmp);
    }
}


struct SearchNode *snode_find(struct SearchNode **head, struct Hex *hex)
{
    struct SearchNode *curr = *head;
    while (curr != NULL) {
        if (curr->hex == hex) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


void snode_add(struct SearchNode **head, struct Hex *new_hex, int new_score)
{
    /* null pointer handling */
    if ((head == NULL) || (new_hex == NULL)) {
        return;
    }

    /* find if hex already present */
    struct SearchNode *existing = snode_find(head, new_hex);
    if (existing) {
        if (existing->score == new_score) {
            return;
        }
        snode_remove(head, new_hex);
    }

    /* create new node for new entry */
    struct SearchNode *new_node = snode_create(new_score, new_hex);

    /* special case of adding to start */
    if ((*head == NULL) || (*head)->score > new_node->score) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    /* main case: loop through to find correct position */
    struct SearchNode *curr = *head;
    while ((curr->next != NULL) && (curr->next->score <= new_node->score)) {
        curr = curr->next;
    }
    new_node->next = curr->next;
    curr->next = new_node;

    return;
}


void snode_remove(struct SearchNode **head, struct Hex *hex)
{
    /* null pointer handling */
    if ((head == NULL) || (*head == NULL) || (hex == NULL)) {
        return;
    }

    struct SearchNode *prev = NULL;
    struct SearchNode *curr = *head;

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
        snode_destroy(curr);
    }

    return;
}



/*
 *      COORDINATE Functions
 */

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

struct Coordinate *coordinate_delta(struct Coordinate *c, enum DIRECTION d)
{
    struct Coordinate *result = malloc(sizeof(struct Coordinate));

    switch (d) {
        case EAST:
            result->x = c->x;
            result->y = c->y + 1;
            result->z = c->z - 1;
            break;
        case NORTHEAST:
            result->x = c->x - 1;
            result->y = c->y + 1;
            result->z = c->z;
            break;
        case NORTHWEST:
            result->x = c->x - 1;
            result->y = c->y;
            result->z = c->z + 1;
            break;
        case WEST:
            result->x = c->x;
            result->y = c->y - 1;
            result->z = c->z + 1;
            break;
        case SOUTHWEST:
            result->x = c->x + 1;
            result->y = c->y - 1;
            result->z = c->z;
            break;
        case SOUTHEAST:
            result->x = c->x + 1;
            result->y = c->y;
            result->z = c->z - 1;
            break;
    }

    return result;
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


void hex_set_coordinates(struct Hex *h, struct Coordinate *c)
{
    return;
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
    struct SearchNode *open = NULL;
    struct SearchNode *closed = NULL;
    struct Hex *end = NULL; /* might not exist */

    /* start with the current hex */
    snode_add(&open, start, coordinate_distance(start->c, target));

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
            if (nbr == NULL || snode_find(&closed, nbr)) {
                continue;
            }

            snode_add(&open, nbr, coordinate_distance(nbr->c, target));
        }

        /* close the current open head */
        snode_add(&closed, curr, 0);
        snode_remove(&open, curr);

    }

    /* we did it, let's clean up and get out of here */
    snode_destroy(open);
    snode_destroy(closed);
    return end; /* NULL if target not found */
}


void hex_create_neighbour(struct Hex *h, enum DIRECTION d)
{
    struct Hex *nbr = hex_create();
    struct *c = coordinate_delta(h, d);

    nbr->c->x = c->x;
    nbr->c->y = c->y;
    nbr->c->z = c->z;

    /* assign the new neighbour */
    h->n[d] = nbr;

    /* find the neighbour's neighbourhood */
    for (int i = WEST; i <= SOUTHEAST; i++) {
        struct Coordinate *c = coordinate_delta(nbr->c, i);
        struct Hex *nbr_nbr = hex_find(nbr, c);
        if (nbr_nbr) {
            (nbr->n)[i] = nbr_nbr;
        }
    }
    
    return;
}
