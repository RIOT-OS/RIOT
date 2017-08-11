/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Attilio Dona'
 *
 */

#include <hw_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "thread.h"
#include "xtimer.h"
#include "lifo.h"

#include <osi.h>

#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#include "device.h"
#include "nwp_conf.h"

/* Priority 5, or 160 as only the top three bits are implemented. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 1 << 5 )

#ifndef MALLOC_SL_OBJECTS

#define MAX_SYNC_OBJS 64

#define MUTEX_LOCK(id) mutex_lock(&osi_mutexes[(int)id])
#define MUTEX_TRYLOCK(id) mutex_trylock(&osi_mutexes[(int)id])
#define MUTEX_UNLOCK(id) mutex_unlock(&osi_mutexes[(int)id])

static int synchronizer[MAX_SYNC_OBJS + 1];

static mutex_t osi_mutexes[MAX_SYNC_OBJS];

static int uxCriticalNesting = 0;


__attribute__ ((weak)) void SimpleLinkWlanEventHandler(SlWlanEvent_t* pSlWlanEvent) {};

__attribute__ ((weak)) void SimpleLinkSockEventHandler(SlSockEvent_t* pSlSockEvent) {};

__attribute__ ((weak)) void SimpleLinkNetAppEventHandler(SlNetAppEvent_t* pSlNetApp) {};

__attribute__ ((weak)) void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
        SlHttpServerResponse_t *pSlHttpServerResponse) {};

__attribute__ ((weak)) void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {};


void init_sync_pool(void) {
    lifo_init(synchronizer, MAX_SYNC_OBJS);

    for(int i=0; i<MAX_SYNC_OBJS; i++) {
        lifo_insert(synchronizer, i);
        mutex_init(&osi_mutexes[i]);
    }
}
#else

#define MUTEX_LOCK(id) mutex_lock(id)
#define MUTEX_TRYLOCK(id) mutex_trylock(id)
#define MUTEX_UNLOCK(id) mutex_unlock(id)

#endif


// network processor status and config handle
nwp_t nwp = {
        .role = ROLE_INVALID
};


#define OSI_MSG_TYPE 0xBEEF

/*
 * global objects
 */

#define THREAD_STACKSIZE_SLSPAWN 2048

#define QUEUE_SIZE_SLSPAWN (8U)

typedef struct osi_task_list {
    char* stack;
    uint8_t busy;
} osi_task_list_t;

static kernel_pid_t sl_spawn_id;

static char *simplelink_stack;


//Local function definition
static void vSimpleLinkSpawnTask(void *pvParameters);

int init_queue(kernel_pid_t tid, msg_t *array, int num) {
    /* check if num is a power of two by comparing to its complement */
    if (num && (num & (num - 1)) == 0) {
        thread_t *me = (thread_t*) thread_get(tid);
        me->msg_array = array;
        cib_init(&(me->msg_queue), num);
        return 0;
    }

    return -1;
}

/*!
 \brief 	This function registers an interrupt in NVIC table

 The sync object is used for synchronization between different thread or ISR and
 a thread.

 \param	iIntrNum	-	Interrupt number to register
 \param	pEntry	    -	Pointer to the interrupt handler
 \param	ucPriority	-	priority of the interrupt

 \return upon successful creation the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_InterruptRegister(int iIntrNum, P_OSI_INTR_ENTRY pEntry,
        unsigned char ucPriority) {
    MAP_IntRegister(iIntrNum, (void (*)(void)) pEntry);
    MAP_IntPrioritySet(iIntrNum, ucPriority);
    MAP_IntEnable(iIntrNum);
    return OSI_OK;
}

/*!
 \brief 	This function De registers an interrupt in NVIC table


 \param	iIntrNum	-	Interrupt number to De register

 \return 	none
 \note
 \warning
 */
void osi_InterruptDeRegister(int iIntrNum) {
    MAP_IntDisable(iIntrNum);
    MAP_IntUnregister(iIntrNum);
}

/*!
 \brief 	This function creates a sync object

 The sync object is used for synchronization between different thread or ISR and
 a thread.

 \param	pSyncObj	-	pointer to the sync object control block

 \return upon successful creation the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_SyncObjCreate(OsiSyncObj_t* pSyncObj) {
    //Check for NULL
    if (NULL == pSyncObj) {
        return OSI_INVALID_PARAMS;
    }

#ifdef MALLOC_SL_OBJECTS
    mutex_t *mutex = malloc(sizeof(mutex_t));
    mutex_init(mutex);
    *pSyncObj = mutex;
#else
    *pSyncObj = (OsiSyncObj_t)lifo_get(synchronizer);
#endif
    return OSI_OK;
}

/*!
 \brief 	This function deletes a sync object

 \param	pSyncObj	-	pointer to the sync object control block

 \return upon successful deletion the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_SyncObjDelete(OsiSyncObj_t* pSyncObj) {
    //Check for NULL
    if (NULL == pSyncObj) {
        return OSI_INVALID_PARAMS;
    }

#ifdef MALLOC_SL_OBJECTS
    free(*pSyncObj);
#else
    mutex_init(&osi_mutexes[(int)*pSyncObj]);
    lifo_insert(synchronizer, (int)*pSyncObj);
#endif
    return OSI_OK;
}

/*!
 \brief 		This function generates a sync signal for the object.

 All suspended threads waiting on this sync object are resumed

 \param		pSyncObj	-	pointer to the sync object control block

 \return 	upon successful signaling the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note		the function could be called from ISR context
 \warning
 */
OsiReturnVal_e osi_SyncObjSignal(OsiSyncObj_t* pSyncObj) {
    //Check for NULL
    if (NULL == pSyncObj) {
        return OSI_INVALID_PARAMS;
    }

    MUTEX_UNLOCK(*pSyncObj);

    return OSI_OK;
}
/*!
 \brief 		This function generates a sync signal for the object
 from ISR context.

 All suspended threads waiting on this sync object are resumed

 \param		pSyncObj	-	pointer to the sync object control block

 \return 	upon successful signalling the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note		the function is called from ISR context
 \warning
 */
OsiReturnVal_e osi_SyncObjSignalFromISR(OsiSyncObj_t* pSyncObj) {
    //Check for NULL
    if (NULL == pSyncObj) {
        return OSI_INVALID_PARAMS;
    }

    MUTEX_UNLOCK(*pSyncObj);

    if (sched_context_switch_request) {
        thread_yield();
    }

    return OSI_OK;
}

/*!
 \brief 	This function waits for a sync signal of the specific sync object

 \param	pSyncObj	-	pointer to the sync object control block
 \param	Timeout		-	numeric value specifies the maximum number of mSec to
 stay suspended while waiting for the sync signal
 Currently, the simple link driver uses only two values:
 - OSI_WAIT_FOREVER
 - OSI_NO_WAIT

 \return upon successful reception of the signal within the timeout window return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_SyncObjWait(OsiSyncObj_t* pSyncObj, OsiTime_t Timeout) {
    //Check for NULL
    if (NULL == pSyncObj) {
        return OSI_INVALID_PARAMS;
    }

    if (Timeout == OSI_NO_WAIT) {
        if (!MUTEX_TRYLOCK(*pSyncObj)) {
            return OSI_OPERATION_FAILED;
        }
    } else {
        MUTEX_LOCK(*pSyncObj);
    }

    return OSI_OK;

}

/*!
 \brief 	This function clears a sync object

 \param	pSyncObj	-	pointer to the sync object control block

 \return upon successful clearing the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_SyncObjClear(OsiSyncObj_t* pSyncObj) {
    //Check for NULL
    if (NULL == pSyncObj) {
        return OSI_INVALID_PARAMS;
    }

    if (OSI_OK == osi_SyncObjWait(pSyncObj, 0)) {
        return OSI_OK;
    } else {
        return OSI_OPERATION_FAILED;
    }
}

/*!
 \brief 	This function creates a locking object.

 The locking object is used for protecting a shared resources between different
 threads.

 \param	pLockObj	-	pointer to the locking object control block

 \return upon successful creation the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_LockObjCreate(OsiLockObj_t* pLockObj) {
    //Check for NULL
    if (NULL == pLockObj) {
        return OSI_INVALID_PARAMS;
    }

#ifdef MALLOC_SL_OBJECTS
    mutex_t *mutex = malloc(sizeof(mutex_t));
    mutex_init(mutex);
    *pLockObj = mutex;
#else
    *pLockObj = (OsiSyncObj_t)lifo_get(synchronizer);
#endif
    return OSI_OK;

}

#if 0
/*!
 \brief 	This function creates a Task.

 Creates a new Task and add it to the last of tasks that are ready to run

 \param	pEntry	-	pointer to the Task Function
 \param	pcName	-	Task Name String
 \param	usStackDepth	-	Stack Size in bytes
 \param	pvParameters	-	pointer to structure to be passed to the Task Function
 \param	uxPriority	-	Task Priority

 \return upon successful creation the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_TaskCreate(P_OSI_TASK_ENTRY pEntry,
        const signed char * const pcName, unsigned short usStackDepth,
        void *pvParameters, unsigned long uxPriority,
        OsiTaskHandle* pTaskHandle) {
    char *stack;
    msg_t *msg_queue;

    kernel_pid_t tid;
    tcb_t *cb;

    stack = malloc(usStackDepth);

    tid = thread_create(stack, usStackDepth, uxPriority, THREAD_CREATE_STACKTEST,
            (thread_task_func_t) pEntry, pvParameters, (const char*) pcName);
    if (tid) {
        msg_queue = malloc(sizeof(msg_t) * QUEUE_SIZE_SLSPAWN);
        init_queue(tid, msg_queue, QUEUE_SIZE_SLSPAWN);
        if (pTaskHandle) {
            cb = (tcb_t *) (stack + usStackDepth - sizeof(tcb_t));
            cb->stack_start = stack;
            *pTaskHandle = cb;
        }
        return OSI_OK;
    }

    return OSI_OPERATION_FAILED;
}

/*!
 \brief 	This function Deletes a Task.

 Deletes a  Task and remove it from list of running task

 \param	pTaskHandle	-	Task Handle

 \note
 \warning
 */
void osi_TaskDelete(OsiTaskHandle* pTaskHandle) {

    tcb_t *tcb = *pTaskHandle;

    if (pTaskHandle == NULL) {
        return;
    }

    free(tcb->msg_array);
    free(tcb->stack_start);

}
#endif

/*!
 \brief 	This function deletes a locking object.

 \param	pLockObj	-	pointer to the locking object control block

 \return upon successful deletion the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_LockObjDelete(OsiLockObj_t* pLockObj) {
    //vSemaphoreDelete((SemaphoreHandle_t)*pLockObj );
#ifdef MALLOC_SL_OBJECTS
    free(*pLockObj);
#else
    mutex_init(&osi_mutexes[(int)*pLockObj]);
    lifo_insert(synchronizer, (int)*pLockObj);
#endif
    return OSI_OK;
}

/*!
 \brief 	This function locks a locking object.

 All other threads that call this function before this thread calls
 the osi_LockObjUnlock would be suspended

 \param	pLockObj	-	pointer to the locking object control block
 \param	Timeout		-	numeric value specifies the maximum number of mSec to
 stay suspended while waiting for the locking object
 Currently, the simple link driver uses only two values:
 - OSI_WAIT_FOREVER
 - OSI_NO_WAIT


 \return upon successful reception of the locking object the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_LockObjLock(OsiLockObj_t* pLockObj, OsiTime_t Timeout) {
    //Check for NULL
    if (NULL == pLockObj) {
        return OSI_INVALID_PARAMS;
    }

    if (Timeout == OSI_NO_WAIT) {
        if (!MUTEX_TRYLOCK(*pLockObj)) {
            return OSI_OPERATION_FAILED;
        }
    } else {
        MUTEX_LOCK(*pLockObj);
    }

    return OSI_OK;

}

/*!
 \brief 	This function unlock a locking object.

 \param	pLockObj	-	pointer to the locking object control block

 \return upon successful unlocking the function should return 0
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_LockObjUnlock(OsiLockObj_t* pLockObj) {
    //Check for NULL
    if (NULL == pLockObj) {
        return OSI_INVALID_PARAMS;
    }

    MUTEX_UNLOCK(*pLockObj);

    return OSI_OK;

}

/*!
 \brief 	This function call the pEntry callback from a different context

 \param	pEntry		-	pointer to the entry callback function

 \param	pValue		- 	pointer to any type of memory structure that would be
 passed to pEntry callback from the execution thread.

 \param	flags		- 	execution flags - reserved for future usage

 \return upon successful registration of the spawn the function should return 0
 (the function is not blocked till the end of the execution of the function
 and could be returned before the execution is actually completed)
 Otherwise, a negative value indicating the error code shall be returned
 \note
 \warning
 */
OsiReturnVal_e osi_Spawn(P_OSI_SPAWN_ENTRY pEntry, void* pValue,
        unsigned long flags) {
    tSimpleLinkSpawnMsg* msg_ptr;
    msg_t riot_msg;

    msg_ptr = malloc(sizeof(tSimpleLinkSpawnMsg));

    msg_ptr->pEntry = pEntry;
    msg_ptr->pValue = pValue;

    riot_msg.type = OSI_MSG_TYPE;
    riot_msg.content.ptr = (char*) msg_ptr;

    msg_send(&riot_msg, sl_spawn_id);

    if (sched_context_switch_request) {
        thread_yield();
    }
    return OSI_OK;
}

/*!
 \brief 	This is the simplelink spawn task to call SL callback from a different context

 \param	pvParameters		-	pointer to the task parameter

 \return void
 \note
 \warning
 */
void vSimpleLinkSpawnTask(void *pvParameters) {
    msg_t msg, msg_queue[QUEUE_SIZE_SLSPAWN];
    tSimpleLinkSpawnMsg* msg_ptr;

    //simplelink_started = 1;

    /* setup the message queue */
    msg_init_queue(msg_queue, QUEUE_SIZE_SLSPAWN);

    for (;;) {
        msg_receive(&msg);
        msg_ptr = (tSimpleLinkSpawnMsg*) msg.content.ptr;

        msg_ptr->pEntry(msg_ptr->pValue);

        free(msg_ptr);
    }
}

/*!
 \brief 	This is the API to create SL spawn task and create the SL queue

 \param	uxPriority		-	task priority

 \return void
 \note
 \warning
 */
OsiReturnVal_e VStartSimpleLinkSpawnTask(unsigned long uxPriority) {

#ifndef MALLOC_SL_OBJECTS
    init_sync_pool();
#endif

    simplelink_stack = malloc(THREAD_STACKSIZE_SLSPAWN);

    sl_spawn_id = thread_create(simplelink_stack, THREAD_STACKSIZE_SLSPAWN, uxPriority,
    		THREAD_CREATE_STACKTEST, (thread_task_func_t) vSimpleLinkSpawnTask, NULL,
            (const char*) "SLSPAWN");

    DEBUG("spawn tid: %d\n", sl_spawn_id);

    return OSI_OK;
}


/*!
 \brief 	This is the API to delete SL spawn task and delete the SL queue

 \param	none

 \return void
 \note
 \warning
 */
void VDeleteSimpleLinkSpawnTask( void )
{
    free(simplelink_stack);
}


/*!
 \brief 	This function is used to create the MsgQ

 \param	pMsgQ	-	pointer to the message queue
 \param	pMsgQName	-	msg queue name
 \param	MsgSize	-	size of message on the queue
 \param	MaxMsgs	-	max. number of msgs that the queue can hold

 \return - OsiReturnVal_e
 \note
 \warning
 */
OsiReturnVal_e osi_MsgQCreate(OsiMsgQ_t* pMsgQ, char* pMsgQName,
        unsigned long MsgSize, unsigned long MaxMsgs) {
    *pMsgQ = malloc(sizeof(msg_t));

    return OSI_OK;
}

/*!
 \brief 	This function is used to delete the MsgQ

 \param	pMsgQ	-	pointer to the message queue

 \return - OsiReturnVal_e
 \note
 \warning
 */
OsiReturnVal_e osi_MsgQDelete(OsiMsgQ_t* pMsgQ) {
    //Check for NULL
    if (NULL == pMsgQ) {
        return OSI_INVALID_PARAMS;
    }

    free(*pMsgQ);

    return OSI_OK;
}

/*!
 \brief 	This function is used to write data to the MsgQ

 \param	pMsgQ	-	pointer to the message queue
 \param	pMsg	-	pointer to the Msg strut to read into
 \param	Timeout	-	timeout to wait for the Msg to be available

 \return - OsiReturnVal_e
 \note
 \warning
 */
OsiReturnVal_e osi_MsgQWrite(OsiMsgQ_t* pMsgQ, void* pMsg, OsiTime_t Timeout) {
    //Check for NULL
    if (NULL == pMsgQ) {
        return OSI_INVALID_PARAMS;
    }

    msg_send(pMsg, *(kernel_pid_t*) pMsgQ);
    return OSI_OK;
}

/*!
 \brief 	This function is used to read data from the MsgQ

 \param	pMsgQ	-	pointer to the message queue
 \param	pMsg	-	pointer to the Msg strut to read into
 \param	Timeout	-	timeout to wait for the Msg to be available

 \return - OsiReturnVal_e
 \note
 \warning
 */
OsiReturnVal_e osi_MsgQRead(OsiMsgQ_t* pMsgQ, void* pMsg, OsiTime_t Timeout) {
    //Check for NULL
    if (NULL == pMsgQ) {
        return OSI_INVALID_PARAMS;
    }

    msg_receive((msg_t *) pMsg);

    return OSI_OK;
}

/*!
 \brief        This function used to suspend the task for the specified number of milli secs
 \param        MilliSecs       -       Time in millisecs to suspend the task
 \return - void
 \note
 \warning
*/
void osi_Sleep(unsigned int MilliSecs) {
    xtimer_usleep(MilliSecs * 80000);
}

__attribute__(( naked )) uint32_t ulPortSetInterruptMask( void )
{
	__asm volatile														\
	(																	\
		"	mrs r0, basepri											\n" \
		"	mov r1, %0												\n"	\
		"	msr basepri, r1											\n" \
		"	bx lr													\n" \
		:: "i" ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) : "r0", "r1"	\
	);

	/* This return will not be reached but is necessary to prevent compiler
	warnings. */
	return 0;
}
/*-----------------------------------------------------------*/

__attribute__(( naked )) void vPortClearInterruptMask( uint32_t ulNewMaskValue )
{
	__asm volatile													\
	(																\
		"	msr basepri, r0										\n"	\
		"	bx lr												\n" \
		:::"r0"														\
	);

	/* Just to avoid compiler warnings. */
	( void ) ulNewMaskValue;
}

/*!
	\brief 	This function use to entering into critical section
	\param	void
	\return - void
	\note
	\warning
*/
unsigned long osi_EnterCritical(void)
{
	// portDISABLE_INTERRUPTS();
	ulPortSetInterruptMask();
	uxCriticalNesting++;
	__asm volatile( "dsb" );
	__asm volatile( "isb" );

    return 0;
}

/*!
	\brief 	This function use to exit critical section
	\param	void
	\return - void
	\note
	\warning
*/
void osi_ExitCritical(unsigned long ulKey)
{
	//configASSERT(uxCriticalNesting);
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		//portENABLE_INTERRUPTS();
		vPortClearInterruptMask(0);
	}

}


void cc3200_reset(void) {
    if (nwp.status & (1<<STATUS_BIT_CONNECTION)) {
        sl_Stop(SL_STOP_TIMEOUT);
        MAP_PRCMHibernateIntervalSet(330);
        MAP_PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
        MAP_PRCMHibernateEnter();
    } else {
        MAP_PRCMMCUReset(1);
    }
}

