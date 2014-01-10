#include <stdio.h>
#include <board.h>
#include <math.h>

int main(void)
{
    double  x = 1234567./1024., z; 
    while (1) { 
            x += 0.1; 
            z = x - floor(x); 
            if (z >= 1) { 
                    putchar('+'); 
            } else { 
                    putchar('-'); 
            } 
    }
}
