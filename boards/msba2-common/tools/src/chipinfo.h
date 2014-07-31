extern char *lpc_return_strings[];

struct sector_info_struct { // an array of
    int address;        // where each sector is located
    int size;       // and how big it is
};

struct chip_info_struct {
    char *part_number;  // human readable part number
    char *id_string;    // id string sent by "J" command
    unsigned int ram_addr;  // where to download into RAM
    int chunk_size;     // download to ram chunk size
    int num_sector;     // number of flash sectors
    struct sector_info_struct *layout;  // layout of sectors
    const unsigned int *bootprog;   // code that boots into user program (NULL = DTR/RTS only)
};

extern struct chip_info_struct chip_info[];
