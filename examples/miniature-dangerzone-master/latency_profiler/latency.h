#ifndef LATENCY_H
#define LATENCY_H

#ifdef CPU_LPC2387

/* Using P0.1 as second debug PIN */
#define TRIGGER_PIN     (BIT1)

#define TRIGGER_PIN_INIT        (FIO0DIR |= TRIGGER_PIN)

#define TRIGGER_PIN_OFF         (FIO0CLR = TRIGGER_PIN)
#define TRIGGER_PIN_ON          (FIO0SET = TRIGGER_PIN)
#define TRIGGER_PIN_TOGGLE      (FIO0PIN ^= TRIGGER_PIN)
#endif

#ifdef CPU_MSP430X16X
/* Using P4.3 as second debug PIN */
#define TRIGGER_PIN          (BIT3)
#define TRIGGER_PIN_INIT      (P4DIR = 0xFF)

#define TRIGGER_PIN_ON       (P4OUT |= TRIGGER_PIN)
#define TRIGGER_PIN_OFF       (P4OUT &= ~TRIGGER_PIN)
#define TRIGGER_PIN_TOGGLE    (P4OUT ^= TRIGGER_PIN)

#endif

#ifdef CPU_NATIVE
#define TRIGGER_PIN
#define TRIGGER_PIN_INIT

#define TRIGGER_PIN_OFF
#define TRIGGER_PIN_ON
#define TRIGGER_PIN_TOGGLE
#endif

#endif /* LATENCY_H */
