#pragma once

#include "bplib.h"

#ifndef BPLIB_MEMPOOL_LEN
#define BPLIB_MEMPOOL_LEN 20000u
#endif

#ifndef BPLIB_QM_MAX_JOBS
#define BPLIB_QM_MAX_JOBS 32
#endif

#define BPLIB_GEN_WORKER_TIMEOUT 1000u

typedef struct {
    volatile int running;
    BPLib_Instance_t BPLibInst;
    BPLib_NC_ConfigPtrs_t ConfigPtrs;
} bplib_instance_data_t;

extern bplib_instance_data_t bplib_instance_data;

int bplib_init(void);

void bplib_terminate(void);