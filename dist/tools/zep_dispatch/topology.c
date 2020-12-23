/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "kernel_defines.h"
#include "topology.h"
#include "zep_parser.h"

#define NODE_NAME_MAX_LEN   32
#define HW_ADDR_MAX_LEN      8

struct node {
    list_node_t next;
    char name[NODE_NAME_MAX_LEN];
    uint8_t mac[HW_ADDR_MAX_LEN];
    struct sockaddr_in6 addr;
    uint8_t mac_len;
};

struct edge {
    list_node_t next;
    struct node *a;
    struct node *b;
    float weight_a_b;
    float weight_b_a;
};

static char *_fmt_addr(char *out, size_t out_len, const uint8_t *addr, uint8_t addr_len)
{
    char *start = out;

    if (out_len < 3 * addr_len) {
        return NULL;
    }

    while (addr_len--) {
        out += sprintf(out, "%02X", *addr++);
        *(out++) = addr_len ? ':' : '\0';
    }

    return start;
}

static struct node *_find_node_by_name(const list_node_t *nodes, const char *name)
{
    for (list_node_t *node = nodes->next; node; node = node->next) {
        struct node *super = container_of(node, struct node, next);
        if (strncmp(super->name, name, sizeof(super->name)) == 0) {
            return super;
        }
    }

    return NULL;
}

static struct node *_find_or_create_node(list_node_t *nodes, const char *name)
{
    struct node *node = _find_node_by_name(nodes, name);

    if (node == NULL) {
        node = malloc(sizeof(*node));
        strncpy(node->name, name, sizeof(node->name) - 1);
        node->mac_len = 0;
        list_add(nodes, &node->next);
    }

    return node;
}

static bool _parse_line(char *line, list_node_t *nodes, list_node_t *edges)
{
    struct edge *e;

    if (*line == '#') {
        return true;
    }

    char *a     = strtok(line, "\t ");
    char *b     = strtok(NULL, "\n\t ");
    char *e_ab  = strtok(NULL, "\n\t ");
    char *e_ba  = strtok(NULL, "\n\t ");

    if (a == NULL || b == NULL) {
        return false;
    }

    if (e_ab == NULL) {
        e_ab = "1";
    }

    if (e_ba == NULL) {
        e_ba = e_ab;
    }

    e = malloc(sizeof(*e));

    e->a = _find_or_create_node(nodes, a);
    e->b = _find_or_create_node(nodes, b);
    e->weight_a_b = atof(e_ab);
    e->weight_b_a = atof(e_ba);

    list_add(edges, &e->next);

    return true;
}

int topology_print(const char *file, const topology_t *t)
{
    FILE *out;
    char addr_str[3 * HW_ADDR_MAX_LEN];

    if (t->flat) {
        // TODO
        return 0;
    }

    if (strcmp(file, "-") == 0) {
        out = stdout;
    } else {
        out = fopen(file, "w");
    }

    if (out == NULL) {
        return -1;
    }

    fprintf(out, "digraph G {\n");

    for (list_node_t *node = t->nodes.next; node; node = node->next) {
        struct node *super = container_of(node, struct node, next);
        fprintf(out, "\t%s [ label = \"%s\\n[%s]\" ]\n",
                super->name, super->name,
                super->mac_len ? _fmt_addr(addr_str, sizeof(addr_str), super->mac, super->mac_len)
                               : "disconnected");
    }

    fprintf(out, "\n");

    for (list_node_t *edge = t->edges.next; edge; edge = edge->next) {
        struct edge *super = container_of(edge, struct edge, next);
        fprintf(out, "\t%s -> %s [ label = \"%.2f\" ]\n",
              super->a->name, super->b->name, super->weight_a_b);
        fprintf(out, "\t%s -> %s [ label = \"%.2f\" ]\n",
              super->b->name, super->a->name, super->weight_b_a);
    }

    fprintf(out, "}\n");

    if (out != stdout) {
        fclose(out);
    }

    return 0;
}

int topology_parse(const char *file, topology_t *out)
{
    FILE *in;
    memset(out, 0, sizeof(*out));

    if (strcmp(file, "-") == 0) {
        in = stdin;
    } else {
        in = fopen(file, "r");
    }

    if (in == NULL) {
        return -1;
    }

    char *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, in) > 0) {
        _parse_line(line, &out->nodes, &out->edges);
    }

    if (line) {
        free(line);
    }

    return 0;
}

void topology_send(const topology_t *t, int sock,
                   const uint8_t *mac_src, size_t mac_src_len,
                   void *buffer, size_t len)
{
    for (list_node_t *edge = t->edges.next; edge; edge = edge->next) {
        struct edge *super = container_of(edge, struct edge, next);

        if (!super->a->mac_len || !super->b->mac_len) {
            continue;
        }

        if ((mac_src_len == super->a->mac_len) &&
            (memcmp(super->a->mac, mac_src, mac_src_len) == 0)) {
            /* packet loss */
            if (random() > super->weight_a_b * RAND_MAX) {
                return;
            }
            zep_set_lqi(buffer, super->weight_a_b * 0xFF);
            sendto(sock, buffer, len, 0, (struct sockaddr*)&super->b->addr, sizeof(super->b->addr));
        } else if ((mac_src_len == super->a->mac_len) &&
                   (memcmp(super->b->mac, mac_src, mac_src_len) == 0)) {
            /* packet loss */
            if (random() > super->weight_b_a * RAND_MAX) {
                return;
            }
            zep_set_lqi(buffer, super->weight_b_a * 0xFF);
            sendto(sock, buffer, len, 0, (struct sockaddr*)&super->a->addr, sizeof(super->a->addr));
        }
    }
}

bool topology_add(topology_t *t, const uint8_t *mac, uint8_t mac_len,
                  struct sockaddr_in6 *addr)
{
    struct node *empty = NULL;
    char addr_str[3 * HW_ADDR_MAX_LEN];

    if (mac_len > HW_ADDR_MAX_LEN) {
        fprintf(stderr, "discarding frame with %u byte address\n", mac_len);
        return false;
    }

    for (list_node_t *node = t->nodes.next; node; node = node->next) {
        struct node *super = container_of(node, struct node, next);

        /* store free node */
        if (!super->mac_len) {
            empty = super;
            continue;
        }

        if (mac_len != super->mac_len) {
            continue;
        }

        /* abort if node is already in list */
        if (memcmp(super->mac, mac, mac_len) == 0) {
            return true;
        }
    }

    /* topology full - can't add node */
    if (empty == NULL) {
        fprintf(stderr, "can't add %s - topology full\n",
                _fmt_addr(addr_str, sizeof(addr_str), mac, mac_len));
        return false;
    }

    printf("adding node %s\n", _fmt_addr(addr_str, sizeof(addr_str), mac, mac_len));

    /* add new node to empty spot */
    memcpy(empty->mac, mac, sizeof(empty->mac));
    memcpy(&empty->addr, addr, sizeof(empty->addr));
    empty->mac_len = mac_len;

    return true;
}
