#include <stdlib.h>
#include "counter.h"

CounterRef Counter_alloc(void)
{
	return (CounterRef)malloc(sizeof(Counter));
}

void Counter_dealloc(CounterRef self)
{
	free(self);
}

CounterRef Counter_init(CounterRef self)
{
	self->value = 0;
	return self;
}

CounterRef Counter_counter(void)
{
	return Counter_init(Counter_alloc());
}

int Counter_value(CounterRef self)
{
	return self->value;
}

void Counter_setValue(CounterRef self,int value)
{
	self->value = value;
}

int Counter_inc(CounterRef self)
{
	self->value++;
	return self->value;
}

int Counter_dec(CounterRef self)
{
	self->value--;
	return self->value;
}

void Counter_clr(CounterRef self)
{
	self->value = 0;
}
