#if 0

from itertools import cycle

regs = [ ('eax', [1,0,0,0,0,0,0]),
         ('ecx', [0,1,0,0,0,0,0]),
         ('edx', [0,0,1,0,0,0,0]),
         #('ebx', [0,0,0,1,0,0,0]),
         #('ebp', [0,0,0,0,1,0,0]),
         ('esi', [0,0,0,0,0,1,0]),
         ('edi', [0,0,0,0,0,0,1]), ]
REGS0 = list(regs)

for cur in cycle(range(len(regs))):
    nex = (cur + 1) % len(regs)
    print 'asm volatile ("xor %%%s, %%%s");' % (regs[cur][0], regs[nex][0])
    val = [a^b for a, b in zip(regs[cur][1], regs[nex][1])]
    regs[nex] = (regs[nex][0], val)
    if regs == REGS0:
        break

#endif

asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
asm volatile ("xor %eax, %ecx");
asm volatile ("xor %ecx, %edx");
asm volatile ("xor %edx, %esi");
asm volatile ("xor %esi, %edi");
asm volatile ("xor %edi, %eax");
