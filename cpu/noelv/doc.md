@defgroup       cpu_noelv NOEL-V
@ingroup        cpu
@brief          Frontgrade Gaisler NOEL-V RISC-V processor

The NOEL-V is a synthesizable RISC-V processor core from Frontgrade Gaisler,
distributed as part of the GRLIB IP library. It is XLEN-parametrizable, so
the ISA of a given bitstream is selected by the board through `CPU_MODEL`,
either `noelv_rv32imac` or `noelv_rv64imac`. This port targets the GRLIB
reference designs with the standard GRLIB peripheral set:
APBUART (serial), GRGPIO (GPIO), the RISC-V ACLINT (timer) and PLIC
(interrupt controller), and the GRETH Ethernet MAC.

@see cpu_riscv_common
