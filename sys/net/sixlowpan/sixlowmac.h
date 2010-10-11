/* 6LoWPAN MAC header file */

#define MSBA2_OUI	0x005BA2	// 24bit OUI 
#define MSBA2_R8BIT	0xA2		// random 8bit

typedef struct mac_addr{
    uint16_t oui_1;
    uint8_t oui_2;
    uint8_t ext_ident_1;        // MSBA2_R8BIT
    uint16_t ext_ident_2;       // radio address
} mac_addr;
