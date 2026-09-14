@defgroup       cpu_noelv NOEL-V
@ingroup        cpu
@brief          Frontgrade Gaisler NOEL-V RISC-V processor

The NOEL-V is a synthesizable 64-bit RISC-V (RV64IMAC) processor core from
Frontgrade Gaisler, distributed as part of the GRLIB IP library. This port
targets the GRLIB reference design with the standard GRLIB peripheral set:
APBUART (serial), GRGPIO (GPIO), the RISC-V ACLINT (timer) and PLIC
(interrupt controller), and the GRETH Ethernet MAC.

@see cpu_riscv_common
