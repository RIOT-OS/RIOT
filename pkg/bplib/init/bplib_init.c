#include "bplib_init.h"

#include "bplib_riot_fwp.h"
#include "bplib_riot_nc.h"

#include <inttypes.h>

// TODO check stack size
#define BPLIB_GENERIC_STACK_SIZE THREAD_STACKSIZE_SMALL
static char generic_worker_stack[BPLIB_GENERIC_STACK_SIZE];

static char mem_pool[BPLIB_MEMPOOL_LEN] __attribute__ ((aligned (8)));

bplib_instance_data_t bplib_instance_data;

static void* generic_worker(void *)
{
    int32_t WorkerID; // THIS ONLY WORKS IF THERE'S ONE WORKER ID!!!
    BPLib_Status_t rv = BPLib_QM_RegisterWorker(&bplib_instance_data.BPLibInst, &WorkerID);
    if (rv != BPLIB_SUCCESS)
    {
        printf("bplib QM_RegisterWorker failed with %"PRId32"\n", rv);
        return NULL;
    }

    while (bplib_instance_data.running)
    {
        BPLib_QM_WorkerRunJob(&bplib_instance_data.BPLibInst, 0, BPLIB_GEN_WORKER_TIMEOUT);
    }
    return NULL;
}

#define DEBUG_NO_CANCEL_INIT 0

// TODO: Documentation. Note: The time functions provided through the NC should be running
// before this is called
int bplib_init(void) {
    BPLib_Status_t BPLibStatus;

    bplib_instance_data.running = 1;

    /* FWP */
    BPLibStatus = bplib_riot_fwp_init();
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        return 1;
    }

    /* EM */
    BPLibStatus = BPLib_EM_Init();
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        return 2;
    }

    /* Time Management */
    BPLibStatus = BPLib_TIME_Init();
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        return 3;
    } 

    /* Node Config */
    BPLibStatus = bplib_riot_nc_init(&bplib_instance_data.ConfigPtrs);
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        return 4;
    }

    /* MEM */
    BPLibStatus = BPLib_MEM_PoolInit(&bplib_instance_data.BPLibInst.pool, mem_pool,
        (size_t)BPLIB_MEMPOOL_LEN);
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        return 5;
    }

    BPLibStatus = BPLib_QM_QueueTableInit(&bplib_instance_data.BPLibInst, BPLIB_QM_MAX_JOBS);
    if (BPLibStatus != BPLIB_SUCCESS)
    {
        return 6;
    }

    /* Start Gen Worker */
    int rc = thread_create(generic_worker_stack, BPLIB_GENERIC_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 1, 0, generic_worker,
        NULL, "bplib-generic");
    if (rc < 0) {
        return 7;
    }

    return 0;
}

void bplib_terminate(void) {
    /* This stops the gen worker after its timeout
     * Might be unsafe since the thread is not joined. But terminate is
     * only called at the end of main() and does not really have any functionality
     * connected to it as of now.
    **/
    bplib_instance_data.running = 0;

    BPLib_QM_QueueTableDestroy(&bplib_instance_data.BPLibInst);
}
