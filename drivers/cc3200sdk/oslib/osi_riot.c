/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#include <hw_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "thread.h"

#include <osi.h>

#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DYNAMIC_MEM

#define OSI_MSG_TYPE 0xBEEF

/*
 * global objects
 */

#define THREAD_STACKSIZE_SLSPAWN 2048

typedef struct osi_task_list {
	char* stack;
	uint8_t busy;
} osi_task_list_t;

static kernel_pid_t sl_spawn_id;

//Local function definition
static void vSimpleLinkSpawnTask( void *pvParameters );

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
OsiReturnVal_e osi_InterruptRegister(int iIntrNum,P_OSI_INTR_ENTRY pEntry,unsigned char ucPriority)
{
	MAP_IntRegister(iIntrNum,(void(*)(void))pEntry);
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

void osi_InterruptDeRegister(int iIntrNum)
{
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
OsiReturnVal_e osi_SyncObjCreate(OsiSyncObj_t* pSyncObj)
{
    //Check for NULL
    if(NULL == pSyncObj)
    {
        return OSI_INVALID_PARAMS;
    }

#ifdef DYNAMIC_MEM
    mutex_t *mutex = malloc(sizeof(mutex_t));
    mutex_init(mutex);
    *pSyncObj = mutex;
#else
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
OsiReturnVal_e osi_SyncObjDelete(OsiSyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return OSI_INVALID_PARAMS;
	}

#ifdef DYNAMIC_MEM
	free(*pSyncObj);
#else
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
OsiReturnVal_e osi_SyncObjSignal(OsiSyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return OSI_INVALID_PARAMS;
	}

    mutex_unlock(*pSyncObj);

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
OsiReturnVal_e osi_SyncObjSignalFromISR(OsiSyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return OSI_INVALID_PARAMS;
	}

	mutex_unlock(*pSyncObj);

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
OsiReturnVal_e osi_SyncObjWait(OsiSyncObj_t* pSyncObj , OsiTime_t Timeout)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return OSI_INVALID_PARAMS;
	}

    if (Timeout == OSI_NO_WAIT) {
    	if(!mutex_trylock(*pSyncObj)) {
    		return OSI_OPERATION_FAILED;
    	}
    } else {
    	mutex_lock(*pSyncObj);
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
OsiReturnVal_e osi_SyncObjClear(OsiSyncObj_t* pSyncObj)
{
	//Check for NULL
	if(NULL == pSyncObj)
	{
		return OSI_INVALID_PARAMS;
	}

    if (OSI_OK == osi_SyncObjWait(pSyncObj,0) )
    {
        return OSI_OK;
    }
    else
    {
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
OsiReturnVal_e osi_LockObjCreate(OsiLockObj_t* pLockObj)
{
    //Check for NULL
    if(NULL == pLockObj)
    {
            return OSI_INVALID_PARAMS;
    }

#ifdef DYNAMIC_MEM
    mutex_t *mutex = malloc(sizeof(mutex_t));
    mutex_init(mutex);
    *pLockObj = mutex;
#else
#endif
    return OSI_OK;

}

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
OsiReturnVal_e osi_TaskCreate(P_OSI_TASK_ENTRY pEntry,const signed char * const pcName,
                              unsigned short usStackDepth, void *pvParameters,
                              unsigned long uxPriority,OsiTaskHandle* pTaskHandle)
{
	char *stack;
	stack = malloc(usStackDepth);

	if(thread_create(stack, usStackDepth,
			uxPriority, CREATE_STACKTEST, (thread_task_func_t)pEntry, pvParameters, (const char*)pcName))
	{
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
void osi_TaskDelete(OsiTaskHandle* pTaskHandle)
{
	while(1) {};
}



/*!
	\brief 	This function deletes a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
	\note
	\warning
*/
OsiReturnVal_e osi_LockObjDelete(OsiLockObj_t* pLockObj)
{
    //vSemaphoreDelete((SemaphoreHandle_t)*pLockObj );
#ifdef DYNAMIC_MEM
	free(*pLockObj);
#else
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
OsiReturnVal_e osi_LockObjLock(OsiLockObj_t* pLockObj , OsiTime_t Timeout)
{
    //Check for NULL
    if(NULL == pLockObj)
    {
            return OSI_INVALID_PARAMS;
    }

    if (Timeout == OSI_NO_WAIT) {
    	if(!mutex_trylock(*pLockObj)) {
    		return OSI_OPERATION_FAILED;
    	}
    } else {
    	mutex_lock(*pLockObj);
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
OsiReturnVal_e osi_LockObjUnlock(OsiLockObj_t* pLockObj)
{
	//Check for NULL
	if(NULL == pLockObj)
	{
		return OSI_INVALID_PARAMS;
	}

    mutex_unlock(*pLockObj);

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

OsiReturnVal_e osi_Spawn(P_OSI_SPAWN_ENTRY pEntry , void* pValue , unsigned long flags)
{
	tSimpleLinkSpawnMsg* msg_ptr;
	msg_t riot_msg;

	msg_ptr = malloc(sizeof(tSimpleLinkSpawnMsg));

	msg_ptr->pEntry = pEntry;
	msg_ptr->pValue = pValue;

	riot_msg.type = OSI_MSG_TYPE;
	riot_msg.content.ptr = (char* )msg_ptr;

	msg_send(&riot_msg, sl_spawn_id);

	// TODO: verify that a context_switch is not needed
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
void vSimpleLinkSpawnTask(void *pvParameters)
{
	msg_t msg;
	tSimpleLinkSpawnMsg* msg_ptr;

	for(;;) {
		msg_receive(&msg);
		msg_ptr = (tSimpleLinkSpawnMsg*)msg.content.ptr;

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
OsiReturnVal_e VStartSimpleLinkSpawnTask(unsigned long uxPriority)
{
	char *stack;
	stack = malloc(THREAD_STACKSIZE_SLSPAWN);

	sl_spawn_id = thread_create(stack, THREAD_STACKSIZE_SLSPAWN,
			uxPriority, CREATE_STACKTEST, (thread_task_func_t)vSimpleLinkSpawnTask, NULL, (const char*)"SLSPAWN");

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
	while(1) {};
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
OsiReturnVal_e osi_MsgQCreate(OsiMsgQ_t* 		pMsgQ , 
			      char*			pMsgQName,
			      unsigned long 		MsgSize,
			      unsigned long		MaxMsgs)
{
	while(1) {}

	return OSI_OK;
}
/*!
	\brief 	This function is used to delete the MsgQ

	\param	pMsgQ	-	pointer to the message queue

	\return - OsiReturnVal_e
	\note
	\warning
*/
OsiReturnVal_e osi_MsgQDelete(OsiMsgQ_t* pMsgQ)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return OSI_INVALID_PARAMS;
	}

	while(1) {}

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

OsiReturnVal_e osi_MsgQWrite(OsiMsgQ_t* pMsgQ, void* pMsg , OsiTime_t Timeout)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return OSI_INVALID_PARAMS;
	}

    msg_send(pMsg, *(kernel_pid_t*)pMsgQ);
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

OsiReturnVal_e osi_MsgQRead(OsiMsgQ_t* pMsgQ, void* pMsg , OsiTime_t Timeout)
{
	//Check for NULL
	if(NULL == pMsgQ)
	{
		return OSI_INVALID_PARAMS;
	}

	msg_receive((msg_t *)pMsg);

	return OSI_OK;
}

/*!
	\brief 	This function to call the memory de-allocation function of the FREERTOS

	\param	Size	-	size of memory to alloc in bytes

	\return - void *
	\note
	\warning
*/

void * mem_Malloc(unsigned long Size)
{
  
    return ( void * ) malloc( (size_t)Size );
}

/*!
	\brief 	This function to call the memory de-allocation function of the FREERTOS

	\param	pMem		-	pointer to the memory which needs to be freed
	
	\return - void 
	\note
	\warning
*/
void mem_Free(void *pMem)
{
    free( pMem );
}

/*!
	\brief 	This function call the memset function
	\param	pBuf	     -	 pointer to the memory to be fill
        \param  Val          -   Value to be fill
        \param  Size         -   Size of the memory which needs to be fill
	\return - void 
	\note
	\warning
*/

void  mem_set(void *pBuf,int Val,size_t Size)
{
    memset( pBuf,Val,Size);
  
}

/*!
      \brief 	This function call the memcopy function
      \param	pDst	-	pointer to the destination
      \param pSrc     -   pointer to the source
      \param Size     -   Size of the memory which needs to be copy
      
      \return - void 
      \note
      \warning
*/
void  mem_copy(void *pDst, void *pSrc,size_t Size)
{
    memcpy(pDst,pSrc,Size);
}


/*!
	\brief 	This function use to entering into critical section
	\param	void		
	\return - void 
	\note
	\warning
*/


/*!
	\brief 	This function used to suspend the task for the specified number of milli secs
	\param	MilliSecs	-	Time in millisecs to suspend the task
	\return - void
	\note
	\warning
*/
void osi_Sleep(unsigned int MilliSecs)
{

}


/*!
	\brief 	This function used to save the OS context before sleep
	\param	void
	\return - void
	\note
	\warning
*/
void osi_ContextSave()
{

}
/*!
	\brief 	This function used to restore the OS context after sleep
	\param	void
	\return - void
	\note
	\warning
*/
void osi_ContextRestore()
{

}
