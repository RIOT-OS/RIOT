extern uint8_t configuration_registers[((0x2E)+1)]; /* address 0x00 - 0x2E */
extern uint8_t configuration_registers_idx; /* for burst access */
extern uint8_t status_registers[((0x3D-0x30)+1)]; /* address 0x30 - 0x3D */
extern uint8_t rx_fifo[]; /* address 0x3F + RW high (BF: 1011 1111) */
extern uint8_t rx_fifo_idx; /* for burst access */
extern uint8_t tx_fifo[]; /* address 0x3F + RW low (3F: 0011 1111) */
extern uint8_t tx_fifo_idx; /* for burst access */
extern uint8_t patable[8]; /* address 0x3E */
extern uint8_t patable_idx; /* for burst access */


extern int native_cc110x_gd0;
extern int native_cc110x_gd1;
extern int native_cc110x_gd2;
extern int native_cc110x_gd0_enabled;
extern int native_cc110x_gd2_enabled;

extern uint8_t addr;
extern uint8_t _native_cc110x_state;

uint8_t do_txrx(uint8_t c);
void do_sres(void);
uint8_t do_strobe(void);
uint8_t parse_header(uint8_t c);
uint8_t write_burst(uint8_t c);
uint8_t read_burst(uint8_t c);
uint8_t write_single(uint8_t c);
uint8_t read_single(uint8_t c);

#define STATE_NULL      0x00
#define STATE_SEL       0x01
#define STATE_WRITE_B   0x02
#define STATE_READ_B    0x03
#define STATE_READ_S    0x04
#define STATE_WRITE_S   0x05

void _native_cc1100_handle_packet(unsigned char *buf, int size);

#define CC1100_FIFO_LENGTH  64
