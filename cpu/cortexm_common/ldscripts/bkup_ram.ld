_backup_ram_start_addr = DEFINED( _backup_ram_start_addr ) ? _backup_ram_start_addr : 0x0 ;
_backup_ram_len = DEFINED( _backup_ram_len ) ? _backup_ram_len : 0x0 ;

MEMORY
{
/* not all Cortex-M platforms use cortexm.ld yet */
    bkup_ram (w!rx) : ORIGIN = _backup_ram_start_addr,        LENGTH = _backup_ram_len
}
