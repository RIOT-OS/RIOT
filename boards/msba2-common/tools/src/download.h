extern int download_begin(char *file);
extern void soft_reboot_begin(void);
extern void hard_reset_to_bootloader(void);
extern void hard_reset_to_user_code(void);
extern void download_cancel(const char *mesg);
extern void download_rx_term(const unsigned char *buf, int num);
extern void download_rx_port(const unsigned char *buf, int num);
extern void download_timer(void);
