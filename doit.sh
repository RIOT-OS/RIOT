#!/bin/sh

echo "i2c_init_master"
git grep -l 'i2c_init_master' | xargs sed -i 's/i2c_init_master/i2c_depr_init_master/g'
echo "i2c_acquire"
git grep -l 'i2c_acquire' | xargs sed -i 's/i2c_acquire/i2c_depr_acquire/g'
echo "i2c_release"
git grep -l 'i2c_release' | xargs sed -i 's/i2c_release/i2c_depr_release/g'
echo "i2c_read_bytes"
git grep -l 'i2c_read_bytes' | xargs sed -i 's/i2c_read_bytes/i2c_depr_read_bytes/g'
echo "i2c_read_byte"
git grep -l 'i2c_read_byte' | xargs sed -i 's/i2c_read_byte/i2c_depr_read_byte/g'
echo "i2c_read_regs"
git grep -l 'i2c_read_regs' | xargs sed -i 's/i2c_read_regs/i2c_depr_read_regs/g'
echo "i2c_read_reg"
git grep -l 'i2c_read_reg' | xargs sed -i 's/i2c_read_reg/i2c_depr_read_reg/g'
echo "i2c_write_bytes"
git grep -l 'i2c_write_bytes' | xargs sed -i 's/i2c_write_bytes/i2c_depr_write_bytes/g'
echo "i2c_write_byte"
git grep -l 'i2c_write_byte' | xargs sed -i 's/i2c_write_byte/i2c_depr_write_byte/g'
echo "i2c_write_regs"
git grep -l 'i2c_write_regs' | xargs sed -i 's/i2c_write_regs/i2c_depr_write_regs/g'
echo "i2c_write_reg"
git grep -l 'i2c_write_reg' | xargs sed -i 's/i2c_write_reg/i2c_depr_write_reg/g'
echo "i2c_poweron"
git grep -l 'i2c_poweron' | xargs sed -i 's/i2c_poweron/i2c_depr_poweron/g'
echo "i2c_poweroff"
git grep -l 'i2c_poweroff' | xargs sed -i 's/i2c_poweroff/i2c_depr_poweroff/g'

echo "periph/i2c"
git grep -l 'periph/i2c_depr.h' | xargs sed -i 's/periph\/i2c.h/periph\/i2c_depr.h/g'
