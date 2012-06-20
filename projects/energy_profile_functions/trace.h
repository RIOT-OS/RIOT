/*
 * trace.h
 *
 *  Created on: 19.06.2012
 *      Author: julsch
 */

#ifndef TRACE_H_
#define TRACE_H_

void __attribute__((no_instrument_function))__cyg_profile_func_enter (void *func,  void *caller);
void __attribute__((no_instrument_function))__cyg_profile_func_exit (void *func, void *caller);


#endif /* TRACE_H_ */
