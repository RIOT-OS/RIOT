#ifndef LATENCY_H
#define LATENCY_H

#ifdef CPU_LPC2387

/* Using P0.1 as second debug PIN */
#define DINO_PIN     (BIT2)

#define DINO_PIN_INIT        (FIO2DIR |= DINO_PIN)

#define DINO_PIN_OFF         (FIO2CLR = DINO_PIN)
#define DINO_PIN_ON          (FIO2SET = DINO_PIN)
#define DINO_PIN_TOGGLE      (FIO2PIN ^= DINO_PIN)
#endif

#ifdef CPU_MSP430X16X
/* Using P4.3 as debug PIN */
#define DINO_PIN            (BIT3)
#define DINO_PIN_INIT      (P4DIR = 0xFF)

#define DINO_PIN_OFF       (P4OUT |= DINO_PIN)
#define DINO_PIN_ON        (P4OUT &= ~DINO_PIN)
#define DINO_PIN_TOGGLE    (P4OUT ^= DINO_PIN)

#endif

#endif /* LATENCY_H */
