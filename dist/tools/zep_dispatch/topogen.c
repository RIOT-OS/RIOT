/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#ifndef CONFIG_USE_NUMERIC_NAMES
#define CONFIG_USE_NUMERIC_NAMES 1
#endif

#define DEFAULT_RANGE 25
#define DELIM "\t ;,"

struct node {
    char name[16];
    char extra[24];
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
    bool grid;
};

static unsigned random_range(unsigned lower, unsigned upper)
{
    unsigned range = upper - lower + 1;
    return lower + rand() % range;
}

static struct node *node_generate(struct node *node, const struct world *w,
                                  unsigned range, unsigned idx)
{
    if (w->grid) {
        float width  = w->w;
        float height = w->h;
        float num    = w->num_nodes;

        /* https://math.stackexchange.com/a/1039514 */
        float n_x = sqrtf(num * width/height
                          + powf(width - height, 2)/(4 * height*height))
                  - (width - height)/2;
        float n_y = num / n_x;

        unsigned step_x = width / n_x;
        unsigned step_y = height / n_y;

        node->x = (idx * step_x + step_x / 2) % w->w;
        node->y = ((idx * step_x + step_x / 2) / w->w) * step_y + step_y / 2;
    } else {
        node->x = random_range(0, w->w);
        node->y = random_range(0, w->h);
    }
    node->r = range;
    return node;
}

static double node_distance(const struct node *a, const struct node *b)
{
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

static double node_distance_weight(const struct node *a, const struct node *b)
{
    double w = 1 - pow(node_distance(a, b), 2) / pow(a->r, 2);

    if (w < 0) {
        return 0;
    }
    return w;
}

static void node_name(struct node *n, unsigned idx)
{
    if (CONFIG_USE_NUMERIC_NAMES) {
        snprintf(n->name, sizeof(n->name), "n%03u", (uint16_t)idx + 1);
        return;
    }

    char *s = n->name;
    const char *end = s + sizeof(n->name) - 1;

    do {
        uint8_t rem = idx % 26;
        idx /= 26;
        *s++ = 'A' + rem;
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
        node_generate(&w->nodes[i], w, random_range(range - var, range + var), i);
        node_name(&w->nodes[i], i);
    }

    if (!w->grid) {
        /* place first node at origin */
        w->nodes[0].x = 0;
        w->nodes[0].y = 0;
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

static void _print_distance(struct node *nodes, unsigned num, bool recursive, bool binary)
{
    struct node *start = nodes;

    if (recursive) {
        for (unsigned i = 0; i < num; ++i) {
            printf("%s", nodes[i].name);
            if (*nodes[i].extra) {
                printf(" := %s", nodes[i].extra);
            }
            puts("");
        }
    }

    for (unsigned i = 1; i < num; ++i) {
        struct node *n = &nodes[i];

        double to_node   = node_distance_weight(start, n);
        double from_node = node_distance_weight(n, start);

        if (binary) {
            if (to_node >= 0.5) {
                printf("%s\t%s\n", start->name, n->name);
            }
            if (from_node >= 0.5) {
                printf("%s\t%s\n", n->name, start->name);
            }
        } else if (to_node > 0 || from_node > 0) {
            printf("%s\t%s\t%.2f\t%.2f\n", start->name, n->name, to_node, from_node);
        }

        if (recursive) {
            _print_distance(n, num - i, false, binary);
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
                    " [-f <file>]"
                    " [-b][-g]"
                    "\n", name);

    puts("\nOptions:");
    puts("\t-s <seed>\trandom seed used for topology generation");
    puts("\t-w <width>\twidth of the 2D topology");
    puts("\t-h <height>\theight of the 2D topology");
    puts("\t-r <range>\tRadio range of the nodes");
    puts("\t-v <variance>\tmaximal random variance of radio range");
    puts("\t-n <nodes>\tnumber of nodes in the topology");
    puts("\t-f <file>\tread world from file instead of generating it randomly\n");
    puts("\t-b\t\tbinary links: link quality is rounded to 100% or 0%");
    puts("\t-g\t\tnodes are organized as a grid");
}

static bool _is_empty(const char *line)
{
    while (*line) {
        if (!isspace(*line++)) {
            return false;
        }
    }

    return true;
}

static struct node *_get_node_by_name(struct world *w, const char *name)
{
    for (unsigned i = 0; i < w->num_nodes; ++i) {
        if (!strcmp(w->nodes[i].name, name)) {
            return &w->nodes[i];
        }
    }

    return NULL;
}

static int _from_file(struct world *w, FILE *file)
{
    char *line = NULL;
    size_t len = 0;


    unsigned linenum = 0;
    while (getline(&line, &len, file) >= 0) {
        ++linenum;

        /* skip comments & empty lines */
        if (*line == '#' || _is_empty(line)) {
            continue;
        }

        char *name = strtok(line, DELIM);
        char *xpos = strtok(NULL, DELIM);
        char *ypos = strtok(NULL, DELIM);
        char *range = strtok(NULL, DELIM);

        /* check if node is already stored */
        struct node *n = _get_node_by_name(w, name);
        if (n == NULL) {
            w->nodes = reallocarray(w->nodes, ++w->num_nodes, sizeof(*w->nodes));
            n = &w->nodes[w->num_nodes - 1];
        }

        /* store name */
        strncpy(n->name, name, sizeof(n->name) - 1);
        if (strlen(name) >= sizeof(n->name)) {
            fprintf(stderr, "warning: '%s' truncated to '%s'\n", name, n->name);
        }

        /* node definition with pinned MAC */
        if (!strcmp(xpos, ":=")) {
            strncpy(n->extra, ypos, sizeof(n->extra) - 1);
            continue;
        }

        if (xpos) {
            n->x = atoi(xpos);
        } else {
            fprintf(stderr, "error on line %d: '%s' has no position\n", linenum, name);
            return -1;
        }
        if (ypos) {
            n->y = atoi(ypos);
        }
        if (range) {
            n->r = atoi(range);
        } else {
            n->r = DEFAULT_RANGE;
        }

        if (n->x + n->r > w->w) {
            w->w = n->x + n->r;
        }
        if (n->y + n->r > w->h) {
            w->h = n->y + n->r;
        }
    }

    free(line);

    return 0;
}

int main(int argc, char** argv)
{
    const char *progname = argv[0];
    char *worldmap = NULL;

    unsigned width  = 100;
    unsigned height = 100;
    unsigned seed   = time(NULL);
    unsigned range  = DEFAULT_RANGE;
    unsigned var    = 0;
    unsigned num    = 10;
    bool binary     = false;
    bool grid       = false;
    char c;

    while ((c = getopt(argc, argv, "s:w:h:r:v:n:f:bg")) != -1) {
        switch (c) {
        case 'b':
            binary = true;
            break;
        case 'g':
            grid = true;
            break;
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
        case 'f':
            worldmap = optarg;
            break;
        default:
            _print_help(progname);
            exit(1);
        }
    }

    srand(seed);

    struct world w = {
        .grid = grid,
    };

    if (worldmap) {
        FILE *file;
        if (strcmp(worldmap, "-")) {
            file = fopen(worldmap, "r");
        } else {
            file = stdin;
        }
        if (!file) {
            fprintf(stderr, "can't open %s\n", worldmap);
            return -1;
        }
        int res = _from_file(&w, file);
        fclose(file);
        if (res) {
            return res;
        }
    } else {
        world_gen(&w, num, width, height, range, var);
    }

    printf("# seed = %u\n", seed);
    puts("# Connections");
    _print_distance(w.nodes, w.num_nodes, true, binary);
    puts("");
    puts("");
    puts("# Node\tX\tY\trange\tcolor");
    _calc_connections(w.nodes, w.num_nodes);
    _print_nodes(w.nodes, w.num_nodes, rand());

    return 0;
}
