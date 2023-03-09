#include <stdio.h>
#include "periph_cpu.h"
#include "board.h"
#include "shell.h"
#include "irq_arch.h"

typedef void IsrFn(void);
extern IsrFn isrStub;
extern IsrFn isrColor;

// volatile unsigned short *keypad = (unsigned short *)REG_KEYPAD;
void (**isr)(void) = (void *)0x03FFFFFC;
// volatile unsigned int *isr = (unsigned int *)0x03007FFC;

void test(void)
{
    int retval;
    __asm__ volatile(" mrs  %0, cpsr"
                     : "=r"(retval)
                     : /* no inputs */
                     : "memory");
    if ((retval & (0x11 | 0x12)) == 18)
    {
        printf("In interrupt!\n");
    }
    else
    {
        printf("Not ruppted\n");
    }
    isrStub();
}

int main(void)
{
    printf("Hello world! Running on:\nARM7TDMI\n");
    printf("GameBoy Advance!\n");
    printf("\t\01Hello \02Hack\03'n'\01ack!\n");

    // Disable global interrupts
    // GBA_IME = 0;
    irq_disable();
    GBA_IME = 1;

    // clear interrupt enable register
    GBA_IE = 0;

    // ack all possible pending irq
    GBA_IRF = 0xFFFF;

    // set an isr
    (*(IsrFn * (*))(0x03FFFFFC)) = isrColor;

    // Enable global interrupt
    // GBA_IME = 1;
    irq_enable();

    // enable irq on vblank
    GBA_IE = 1;

    // make hardware throw irq on vblank
    GBA_DISPSTAT = 0x8;

    // Clear the force-blank bit
    GBA_DISPCNT = 0;

    volatile unsigned short read = 0x3FF;
    printf("Press Start for a \02Shell...\n\03..without Stdin :)\n");
    while (read != GBA_KEY_Start)
    {
        read = GBA_KEYPAD ^ 0x3FF;
        if (read)
        {
            char *pressed = "Nan";
            switch (read)
            {
            case GBA_KEY_A:
                pressed = "A";
                test();
                break;
            case GBA_KEY_B:
                pressed = "B";
                break;
            case GBA_KEY_Select:
                pressed = "Select";
                break;
            case GBA_KEY_Start:
                pressed = "Start";
                break;
            case GBA_KEY_Right:
                pressed = "Right";
                break;
            case GBA_KEY_Left:
                pressed = "Left";
                break;
            case GBA_KEY_Up:
                pressed = "Up";
                // GBA_IME = 1;
                irq_enable();
                break;
            case GBA_KEY_Down:
                pressed = "Down";
                irq_disable();
                // GBA_IME = 0;
                break;
            case GBA_KEY_R:
                pressed = "R";
                break;
            case GBA_KEY_L:
                pressed = "L";
                break;
            default:
                break;
            }
            printf("Pressed: %s\n", pressed);
        }
    }
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
