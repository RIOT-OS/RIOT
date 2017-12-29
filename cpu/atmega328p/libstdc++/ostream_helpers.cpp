/* 
 * This code is taken from the Arduino Print class. 
 * 
 * printf() doesn't handle floating point types on AVR. 
 * This code helps me hack that in. It's not here permanently. 
 * 
 */

#include <ostream_helpers>
#include <stdio.h>

namespace std {

int arduinoPrintFloat(double number, uint8_t digits, char *buffer, size_t buffer_size) 
{ 
  size_t n = 0;

  if (buffer_size < 4) 
    return 0;

  if (isnan(number)) return snprintf(buffer, buffer_size, "nan");
  if (isinf(number)) return snprintf(buffer, buffer_size, "inf");
  if (number > 4294967040.0) return snprintf(buffer, buffer_size, "ovf");  // constant determined empirically
  if (number <-4294967040.0) return snprintf(buffer, buffer_size, "ovf");  // constant determined empirically

  // Handle negative numbers
  if (number < 0.0) {
    n += snprintf(&buffer[n], buffer_size-n, "-");
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += snprintf(&buffer[n], buffer_size-n, "%lu", int_part);
  if (n >= buffer_size) 
    return n; 

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += snprintf(&buffer[n], buffer_size-n, ".");
    if (n >= buffer_size) 
      return n; 
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
    {
      remainder *= 10.0;
      unsigned int toPrint = (unsigned int)(remainder);
      n += snprintf(&buffer[n], buffer_size-n, "%u", toPrint);
      if (n >= buffer_size) 
	return n; 
      remainder -= toPrint; 
    }
  
  return n;
}

}
