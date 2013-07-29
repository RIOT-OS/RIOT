
extern void init_settings(void);
extern const char *file_setting(void);
extern const char *port_setting(void);
extern const char *baud_setting(void);
extern const char *crystal_setting(void);
extern void new_file_setting(const char *new_file);
extern void new_port_setting(const char *new_port);
extern void new_baud_setting(const char *new_baud);
extern void new_crystal_setting(const char *new_xtal);

extern char *baud_list[];
