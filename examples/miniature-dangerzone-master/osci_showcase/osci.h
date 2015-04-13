#ifndef OSCI_H
#define OSCI_H

#ifdef CPU_LPC2387

/* Using P0.18 as second debug PIN */
#define OSCI_PIN     (BIT18)

#define OSCI_PIN_INIT        (FIO0DIR |= OSCI_PIN)

#define OSCI_PIN_OFF         (FIO0CLR = OSCI_PIN)
#define OSCI_PIN_ON          (FIO0SET = OSCI_PIN)
#define OSCI_PIN_TOGGLE      (FIO0PIN ^= OSCI_PIN)
#endif

#ifdef CPU_MSP430X16X
/* Using P4.3 as debug PIN */
#define OSCI_PIN            (BIT3)
#define OSCI_PIN_INIT      (P4DIR = 0xFF)

#define OSCI_PIN_OFF       (P4OUT |= OSCI_PIN)
#define OSCI_PIN_ON        (P4OUT &= ~OSCI_PIN)
#define OSCI_PIN_TOGGLE    (P4OUT ^= OSCI_PIN)

#endif

#endif /* OSCI_H */
