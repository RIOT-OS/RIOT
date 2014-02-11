extern int read_intel_hex(const char *filename);
extern int bytes_within_range(int begin, int end);
extern void get_ihex_data(int addr, int len, unsigned char *bytes);
extern void put_ihex_data(int addr, int len, const unsigned char *bytes);
