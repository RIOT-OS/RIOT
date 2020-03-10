/*
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== QueueP_nortos.c ========
 */

#include <ti/drivers/dpl/HwiP.h>
#include "QueueP.h"

/*
 *  ======== QueueP_init ========
 */
void QueueP_init(QueueP_Obj *obj)
{
    obj->elem.next = obj->elem.prev = &(obj->elem);
}

/*
 *  ======== QueueP_empty ========
 */
bool QueueP_empty(QueueP_Obj *obj)
{
    return (obj->elem.next == &(obj->elem));
}

/*
 *  ======== QueueP_get ========
 */
uintptr_t QueueP_get(QueueP_Obj *obj)
{
    QueueP_Elem *elem;
    uintptr_t key;

    key = HwiP_disable();

    elem = obj->elem.next;

    obj->elem.next = elem->next;
    elem->next->prev = &(obj->elem);

    HwiP_restore(key);

    return ((uintptr_t)elem);

}

/*
 *  ======== QueueP_getTail ========
 */
uintptr_t QueueP_getTail(QueueP_Obj *obj)
{
    QueueP_Elem *elem;
    uintptr_t key;

    key = HwiP_disable();

    elem = obj->elem.prev;

    obj->elem.prev = elem->prev;
    elem->prev->next = &(obj->elem);

    HwiP_restore(key);

    return ((uintptr_t)elem);

}

/*
 *  ======== QueueP_head ========
 */
uintptr_t QueueP_head(QueueP_Obj *obj)
{
    return ((uintptr_t)(obj->elem.next));
}

/*
 *  ======== elemClear ========
 */
void QueueP_elemClear(QueueP_Elem *qelem)
{
    qelem->next = qelem->prev = qelem;
}

/*
 *  ======== insert ========
 */
void QueueP_insert(QueueP_Elem *qelem, QueueP_Elem *elem)
{
    QueueP_put((QueueP_Obj *)qelem, elem);
}

/*
 *  ======== next ========
 */
uintptr_t QueueP_next(QueueP_Elem *qelem)
{
    return ((uintptr_t)qelem->next);
}

/*
 *  ======== QueueP_prev ========
 */
uintptr_t QueueP_prev(QueueP_Elem *qelem)
{
    return ((uintptr_t)qelem->prev);
}

/*
 *  ======== QueueP_put ========
 */
void QueueP_put(QueueP_Obj *obj, QueueP_Elem *elem)
{
    uintptr_t key;

    key = HwiP_disable();

    elem->next = &(obj->elem);
    elem->prev = obj->elem.prev;
    obj->elem.prev->next = elem;
    obj->elem.prev = elem;

    HwiP_restore(key);
}

/*
 *  ======== QueueP_putHead ========
 */
void QueueP_putHead(QueueP_Obj *obj, QueueP_Elem *elem)
{
    uintptr_t key;

    key = HwiP_disable();

    elem->prev = &(obj->elem);
    elem->next = obj->elem.next;
    obj->elem.next->prev = elem;
    obj->elem.next = elem;

    HwiP_restore(key);
}

/*
 *  ======== QueueP_remove ========
 */
void QueueP_remove(QueueP_Elem *qelem)
{
#if defined(__IAR_SYSTEMS_ICC__)
    QueueP_Elem *temp;
    temp = qelem->next;
    qelem->prev->next = temp;
    temp = qelem->prev;
    qelem->next->prev = temp;
#else
    qelem->prev->next = qelem->next;
    qelem->next->prev = qelem->prev;
#endif
}

/*
 *  ======== isQueued ========
 */
bool QueueP_isQueued(QueueP_Elem *qelem)
{
    if ((qelem->prev == qelem) && (qelem->next == qelem)) {
        return (false);
    }
    else {
        return (true);
    }
}
