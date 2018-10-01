REGEXP_TCB_TOTAL = r'sizeof\(thread_t\):\s+(\d+)'
REGEXP_TCB_PARTS = r'\s+tcb->\w+\s+(\d+)\s+\d+'

LIST__TCB_PARTS_32 = [
    '    tcb->sp                       4       0',
    '    tcb->status                   1       4',
    '    tcb->priority                 1       5',
    '    tcb->pid                      2       6',
    '    tcb->rq_entry                 4       8',
    '    tcb->wait_data                4      12',
    '    tcb->msg_waiters              4      16',
    '    tcb->msg_queue               12      20',
    '    tcb->msg_array                4      32',
]
LIST__TCB_PARTS_16 = [
    '    tcb->sp                       2       0',
    '    tcb->status                   1       2',
    '    tcb->priority                 1       3',
    '    tcb->pid                      2       4',
    '    tcb->rq_entry                 2       6',
    '    tcb->wait_data                2       8',
    '    tcb->msg_waiters              2      10',
    '    tcb->msg_queue                6      12',
    '    tcb->msg_array                2      18',
]
