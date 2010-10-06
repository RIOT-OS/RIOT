/* fragmentation defines */
#define FRAG1_HDR_LEN		32 	// in bit
#define FRAGN_HDR_LEN		40 	// in bit
#define FRAG1_DISPATCH		0xC0	// 11000000
#define FRAGN_DISPATCH		0xE0	// 11100000

/* 48-bit MAC to EUI64 pattern */
#define MAC_TO_EUI64_PTRN   0xFFFE

typedef struct eui64{
    uint8_t oui_1;
    uint16_t oui_2;
    uint16_t pattern;
    uint8_t ext_ident_1;
    uint16_t ext_ident_2;
} eui64;  
