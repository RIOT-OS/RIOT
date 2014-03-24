#ifndef	__COUNTER_H__
#define	__COUNTER_H__

typedef struct __Counter	Counter;
typedef struct __Counter*	CounterRef;

struct __Counter {
	int value;
};

CounterRef Counter_alloc(void);
void Counter_dealloc(CounterRef);
CounterRef Counter_init(CounterRef);
CounterRef Counter_counter(void);
int Counter_value(CounterRef);
void Counter_setValue(CounterRef,int);
int Counter_inc(CounterRef);
int Counter_dec(CounterRef);
void Counter_clr(CounterRef);

#endif/*__COUNTER_H__*/
