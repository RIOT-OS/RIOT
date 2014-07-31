/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "testing.h"
#include "flowcontrol.h"

struct timeval timers[256];
struct test_stats *stats = NULL;
int run_counter;

FILE *stats_file = NULL;

void init_file(const char *skeleton_file_name,
               int runs_per_test, float interval)
{
    FILE *skeleton_file = NULL;
    char line[1024];

    skeleton_file = fopen(skeleton_file_name, "r");

    while (fgets(line, 1024, skeleton_file) != NULL) {
        if (strncmp(line, "# sending window size=%d\n", 1024) == 0) {
            fprintf(stats_file, line, BORDER_SWS);
        }
        else if (strncmp(line, "# count=%ld (-c)\n", 1024) == 0) {
            fprintf(stats_file, line, runs_per_test);
        }
        else if (strncmp(line, "# interval=%f (-i)\n", 1024) == 0) {
            fprintf(stats_file, line, interval);
        }
        else {
            fprintf(stats_file, "%s", line);
        }
    }

    fclose(skeleton_file);
}

int testing_init(const char *stats_file_name,
                 const char *skeleton_file_name,
                 int runs_per_test, float interval)
{
    if (stats_file != NULL) {
        return -1;
    }

    stats = (struct test_stats *)calloc(runs_per_test, sizeof(struct test_stats));
    run_counter = 0;

    stats_file = fopen(stats_file_name, "w");

    if (stats_file == NULL) {
        return -1;
    }

    init_file(skeleton_file_name, runs_per_test, interval);
    return 0;
}

int testing_destroy()
{
    int res, i;

    for (i = 0; i < run_counter; i++) {
        fprintf(stats_file, "%7d\t%3d\t%7ld\n",
                i, stats[i].seq_num, stats[i].time_diff
               );
    }

    free(stats);

    stats = NULL;

    res = fclose(stats_file);

    stats_file = NULL;

    return res;
}

void testing_start(uint8_t seq_num)
{
    if (stats_file == NULL) {
        return;
    }

    gettimeofday(&(timers[seq_num]), NULL);
}

void testing_stop(uint8_t seq_num)
{
    if (stats_file == NULL) {
        return;
    }

    struct timeval start = timers[seq_num], end;

    gettimeofday(&end, NULL);

    stats[run_counter].seq_num = seq_num;

    stats[run_counter].time_diff = (end.tv_sec - start.tv_sec) * 1000;

    stats[run_counter].time_diff += (end.tv_usec - start.tv_usec) / 1000;

    run_counter++;
}

void generate_filename(
    char *filename,
    const char *results_dir_name,
    int runs_per_test,
    float interval)
{
    time_t today;
    struct tm *tmp = NULL;
    char timestr[16];
    FILE *test = NULL;
    int count = 1;

    today = time(NULL);
    tmp = localtime(&today);

    if (tmp == NULL) {
        perror("localtime");
        return;
    }

    strftime(timestr, 16, "%Y.%m.%d", tmp);

    do {
        sprintf(filename,
                "%s/%s-Results-Ping6_%d_%d_%f-%d.txt",
                results_dir_name,
                timestr,
                BORDER_SWS,
                runs_per_test,
                interval,
                count++
               );
    }
    while ((test = fopen(filename, "r")) != NULL);

    if (test != NULL) {
        fclose(test);
    }
}

void start_test(const char *ping_addr,
                const char *results_dir_name, const char *skeleton_file,
                int runs_per_test, float interval)
{
    printf("%d, %f\n", runs_per_test, interval);
    char command[50];
    char filename[50];
    generate_filename(filename, results_dir_name, runs_per_test, interval);

    sprintf(command, "ping6 -v -f -c %d -i %f -W 1 abcd::d",
            runs_per_test,
            interval
           );

    testing_init(filename, skeleton_file, runs_per_test, interval);
    puts(command);
    system(command);
    testing_destroy();
}
