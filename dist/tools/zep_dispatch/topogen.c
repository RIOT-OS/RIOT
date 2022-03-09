/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

struct node {
    char name[8];
    int x;
    int y;
    unsigned r;
    bool rx_from_root;
    bool tx_to_root;
};

struct world {
    unsigned w;
    unsigned h;
    unsigned num_nodes;
    struct node *nodes;
};

static unsigned random_range(unsigned lower, unsigned upper)
{
    unsigned range = upper - lower + 1;
    return lower + rand() % range;
}

static struct node *node_generate(struct node *n, const struct world *w,
                                  unsigned range)
{
    n->x = random_range(0, w->w);
    n->y = random_range(0, w->h);
    n->r = range;
    return n;
}

static double node_distance(const struct node *a, const struct node *b)
{
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

static double node_distance_weight(const struct node *a, const struct node *b)
{
    double w = 1 - node_distance(a, b) / a->r;

    if (w < 0) {
        return 0;
    }
    return w;
}

static void node_name(struct node *n, unsigned idx)
{
    char *s = n->name;
    const char *end = s + sizeof(n->name) - 1;

    do {
        uint8_t rem = idx % 26;
        *s++ = 'A' + rem;
        idx -= rem;
    } while (idx && s != end);
    *s = 0;
}

static void world_gen(struct world *w, unsigned num_nodes,
                      unsigned width, unsigned height,
                      unsigned range, unsigned var)
{
    w->w = width;
    w->h = height;
    w->num_nodes = num_nodes;
    w->nodes = calloc(num_nodes, sizeof(*w->nodes));

    for (unsigned i = 0; i < num_nodes; ++i) {
        node_generate(&w->nodes[i], w, random_range(range - var, range + var));
        node_name(&w->nodes[i], i);
    }
}

static unsigned _color(const struct node *n, unsigned base)
{
    if (n->rx_from_root && n->tx_to_root) {
        return base;
    } else if (n->rx_from_root) {
        return base / 2;
    } else if (n->tx_to_root) {
        return base / 3;
    }

    return 0;
}

static void _print_nodes(struct node *nodes, unsigned num, unsigned color)
{
    for (unsigned i = 0; i < num; ++i) {
        printf("# %s\t%d\t%d\t%u\t0x%x\n", nodes[i].name,
                                           nodes[i].x, nodes[i].y,
                                           nodes[i].r, _color(&nodes[i], color));
    }
}

/* To visualize the network we color the nodes based on whether they have
 * a (bi-directional) connection to the root node via some other node(s).
 */
static void _calc_connections(struct node *nodes, unsigned num)
{
    bool changes = true;
    nodes->rx_from_root = true;
    nodes->tx_to_root = true;

    /* super basic algorithm - just loop unti there are no more changes
     * in the node's connection states. */
    while (changes) {
        changes = false;

        for (unsigned i = 0; i < num; ++i) {
            const struct node *n = &nodes[i];

            for (unsigned j = 0; j < num; ++j) {
                struct node *m = &nodes[j];

                /* node is already fully connected */
                if (m->rx_from_root && m->tx_to_root) {
                    continue;
                }

                /* m can receive from n and n can receive from root */
                if (node_distance_weight(n, m) > 0) {
                    if (!m->rx_from_root && n->rx_from_root) {
                        m->rx_from_root = true;
                        changes = true;
                    }
                }

                /* m can send to n and n can send to root */
                if (node_distance_weight(m, n) > 0) {
                    if (!m->tx_to_root && n->tx_to_root) {
                        m->tx_to_root = true;
                        changes = true;
                    }
                }
            }
        }
    }
}

static void _print_distance(struct node *nodes, unsigned num, bool recursive)
{
    struct node *start = nodes;

    for (unsigned i = 1; i < num; ++i) {
        struct node *n = &nodes[i];

        double to_node   = node_distance_weight(start, n);
        double from_node = node_distance_weight(n, start);

        if (to_node > 0 || from_node > 0) {
            printf("%s\t%s\t%.2f\t%.2f\n", start->name, n->name, to_node, from_node);
        }

        if (recursive) {
            _print_distance(n, num - i, false);
        }
    }
}

static void _print_help(const char *name)
{
    puts("Generate a number of nodes that are randomly placed in a rectangular area");
    printf("usage: %s [-s <seed>]"
                    " [-w <width>]"
                    " [-h <height>]"
                    " [-r <range>]"
                    " [-v <variance of range>]"
                    " [-n <nodes>]"
                    "\n", name);
}

int main(int argc, char** argv)
{
    const char *progname = argv[0];

    unsigned width  = 100;
    unsigned height = 100;
    unsigned seed   = time(NULL);
    unsigned range  = 25;
    unsigned var    = 0;
    unsigned num    = 10;
    char c;

    while ((c = getopt(argc, argv, "s:w:h:r:v:n:")) != -1) {
        switch (c) {
        case 's':
            seed = atoi(optarg);
            break;
        case 'w':
            width = atoi(optarg);
            break;
        case 'h':
            height = atoi(optarg);
            break;
        case 'r':
            range = atoi(optarg);
            break;
        case 'v':
            var = atoi(optarg);
            break;
        case 'n':
            num = atoi(optarg);
            break;
        default:
            _print_help(progname);
            exit(1);
        }
    }

    srand(seed);

    struct world w;
    world_gen(&w, num, width, height, range, var);

    printf("# seed = %u\n", seed);
    puts("# Connections");
    _print_distance(w.nodes, w.num_nodes, true);
    puts("");
    puts("");
    puts("# Node\tX\tY\trange\tcolor");
    _calc_connections(w.nodes, w.num_nodes);
    _print_nodes(w.nodes, w.num_nodes, rand());

    return 0;
}
