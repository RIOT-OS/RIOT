#ifndef NDEF_TEXT_PAYLOAD_H
#define NDEF_TEXT_PAYLOAD_H

#include <stdint.h>
#include "net/nfc/ndef/ndef.h"



typedef enum {
    UTF8 = 0,
    UTF16 = 1
} ndef_text_encoding_t;


typedef struct {
	uint8_t *start;

	uint8_t *status_byte;
	uint8_t *language_code;
	uint8_t *text;
} ndef_record_text_payload_desc_t;


int ndef_add_text_record(ndef_t *message, char const *text, uint32_t text_length, 
char const *lang_code, uint8_t lang_code_length, ndef_text_encoding_t encoding);

#endif