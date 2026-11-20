import struct
import logging
from abc import abstractmethod
from collections import namedtuple

LDDW_STRUCT = struct.Struct("<BBHiBBHi")
LDDW = namedtuple(
    "LDDW", "opcode registers offset immediate_l null1 null2 null3 immediate_h"
)

LDDW_OPCODE = 0x18
LDDWD_OPCODE = 0xB8
LDDWR_OPCODE = 0xD8


class Instruction(object):

    OPERATION_STRUCT = struct.Struct("<BBhI")
    OPCODE = 0x00
    LENGTH = 8
    COMPRESSED = struct.Struct("<BB")

    def __init__(self, registers, offset, immediate, address=0, compressed_address=0):
        self.address = address
        self.compressed_address = compressed_address
        self.registers = registers
        self.offset = offset
        self.immediate = immediate

    @classmethod
    def from_bytes(cls, instruction: bytes, address=0, compressed_address=0):
        opcode, registers, offset, immediate = cls.OPERATION_STRUCT.unpack(instruction)
        if cls.opcode() != opcode:
            logging.critical(
                f"Opcode not matching expected, got {hex(opcode)}, expected {hex(cls.opcode())}"
            )
            return None
        logging.debug(
            f"Creating instruction {hex(opcode)} with {registers}, {offset}, {immediate}"
        )
        return cls(registers, offset, immediate, address, compressed_address)

    @classmethod
    def from_compressed(cls, instruction: bytes, address=0, compressed_address=0):
        fields = cls.COMPRESSED.unpack_from(instruction, 0)
        opcode, registers, offset, immediate = cls.expand_compressed(fields)
        if cls.opcode() != opcode:
            logging.critical(
                f"Opcode not matching expected, got {hex(opcode)}, expected {hex(cls.opcode())}"
            )
            return None
        return cls(registers, offset, immediate, address, compressed_address)

    @classmethod
    def expand_compressed(cls, fields):
        """
        :param fields: the fields from a compressed struct unpack
        :return: a tuple containing the opcode, the registers, the offset and the immediate
        """
        return fields[0], fields[1], 0, 0

    def set_compressed_address(self, addr):
        self.compressed_address = addr

    @classmethod
    def opcode(cls):
        return cls.OPCODE

    @property
    def src_register(self):
        return (self.registers & 0xF0) >> 4

    @property
    def dst_register(self):
        return self.registers & 0x0F

    def asm_print(self):
        return f"r{self.dst_register} = r{self.src_register}"

    def compressed_asm_print(self):
        return self.asm_print()

    def bytes(self):
        return self.OPERATION_STRUCT.pack(
            self.OPCODE, self.registers, self.offset, self.immediate
        )

    def full_print(self):
        hexdump = " ".join(map("{0:0>2x}".format, list(self.bytes())))
        asm = self.asm_print()
        return f"{hex(self.address).rjust(7)}:\t{hexdump} {asm}"

    def compressed_print(self):
        compressed_form = self.compress()
        hexdump = " ".join(map("{0:0>2x}".format, list(compressed_form)))
        asm = self.compressed_asm_print()
        return f"{hex(self.compressed_address).rjust(7)}:\t{hexdump.ljust(24)} {asm}"

    @abstractmethod
    def compress(self):
        pass

    @classmethod
    def compressed_size(cls):
        return cls.COMPRESSED.size


class AluInstruction(Instruction):

    OPERAND = "+"
    COMPRESSED = struct.Struct("<BB")

    @property
    def operand(self):
        return self.OPERAND

    def asm_print(self):
        return f"r{self.dst_register} {self.operand}= r{self.src_register}"

    def compress(self):
        return self.COMPRESSED.pack(self.OPCODE, self.registers)


class AluImmInstruction(AluInstruction):

    COMPRESSED = struct.Struct("<BBI")
    COMPRESSED_LEN = 6  # 2 byte

    def asm_print(self):
        return f"r{self.dst_register} {self.operand}= {self.immediate}"

    def compress(self):
        return self.COMPRESSED.pack(self.OPCODE, self.registers, self.immediate)

    @classmethod
    def expand_compressed(cls, fields):
        return fields[0], fields[1], 0, fields[2]


class AddImmInstruction(AluImmInstruction):
    OPERAND = "+"
    OPCODE = 0x07


class AddInstruction(AluInstruction):
    OPERAND = "+"
    OPCODE = 0x0F


class SubImmInstruction(AluImmInstruction):
    OPERAND = "-"
    OPCODE = 0x17


class SubInstruction(AluInstruction):
    OPERAND = "-"
    OPCODE = 0x1F


class MulImmInstruction(AluImmInstruction):
    OPERAND = "*"
    OPCODE = 0x27


class MulInstruction(AluInstruction):
    OPERAND = "*"
    OPCODE = 0x2F


class DivImmInstruction(AluImmInstruction):
    OPERAND = "/"
    OPCODE = 0x37


class DivInstruction(AluInstruction):
    OPERAND = "/"
    OPCODE = 0x3F


class OrImmInstruction(AluImmInstruction):
    OPERAND = "|"
    OPCODE = 0x47


class OrInstruction(AluInstruction):
    OPERAND = "|"
    OPCODE = 0x4F


class AndImmInstruction(AluImmInstruction):
    OPERAND = "&"
    OPCODE = 0x57


class AndInstruction(AluInstruction):
    OPERAND = "&"
    OPCODE = 0x5F


class LSHImmInstruction(AluImmInstruction):
    OPERAND = "<<"
    OPCODE = 0x67


class LSHInstruction(AluInstruction):
    OPERAND = "<<"
    OPCODE = 0x6F


class RSHImmInstruction(AluImmInstruction):
    OPERAND = ">>"
    OPCODE = 0x77


class RSHInstruction(AluInstruction):
    OPERAND = ">>"
    OPCODE = 0x7F


class NegInstruction(AluInstruction):
    OPERAND = "-"
    OPCODE = 0x87

    def asm_print(self):
        return f"r{self.dst_register} = -{self.src_register}"


class ModImmInstruction(AluImmInstruction):
    OPERAND = "%"
    OPCODE = 0x97


class ModInstruction(AluInstruction):
    OPERAND = "%"
    OPCODE = 0x9F


class XorImmInstruction(AluImmInstruction):
    OPERAND = "^"
    OPCODE = 0xA7


class XorInstruction(AluInstruction):
    OPERAND = "^"
    OPCODE = 0xAF


class MovImmInstruction(AluImmInstruction):
    OPERAND = ""
    OPCODE = 0xB7


class MovInstruction(AluInstruction):
    OPERAND = ""
    OPCODE = 0xBF


class ARSHImmInstruction(AluImmInstruction):
    OPERAND = ">>"
    OPCODE = 0xC7


class ARSHInstruction(AluInstruction):
    OPERAND = ">>"
    OPCODE = 0xCF


class MemInstruction(Instruction):

    COMPRESSED = struct.Struct("<BBh")

    @property
    def size(self):
        return (self.OPCODE & 0x18) >> 3

    @property
    def size_str(self):
        size_int = self.size
        if size_int == 3:
            return "uint64_t"
        elif size_int == 2:
            return "uint32_t"
        elif size_int == 1:
            return "uint16_t"
        elif size_int == 0:
            return "uint8_t"

    def compress(self):
        return self.COMPRESSED.pack(self.OPCODE, self.registers, self.offset)

    @classmethod
    def expand_compressed(cls, fields):
        return fields[0], fields[1], fields[2], 0


class LoadInstruction(MemInstruction):
    def asm_print(self):
        return f"r{self.dst_register} = {self.immediate}"


class LoadXInstruction(MemInstruction):
    def asm_print(self):
        return f"r{self.dst_register} = *({self.size_str}*)(r{self.src_register} + {self.offset})"


class StoreXInstruction(MemInstruction):
    def asm_print(self):
        return f"*({self.size_str}*)(r{self.dst_register} + {self.offset}) = r{self.src_register}"


class StoreInstruction(MemInstruction):

    COMPRESSED = struct.Struct("<BBhI")

    def asm_print(self):
        return f"*({self.size_str}*)(r{self.dst_register} + {self.offset}) = {self.immediate}"

    def compress(self):
        return self.COMPRESSED.pack(
            self.OPCODE, self.registers, self.offset, self.immediate
        )

    @classmethod
    def expand_compressed(cls, fields):
        return fields


class LDDWInstruction(LoadInstruction):

    COMPRESSED = struct.Struct("<BBQ")

    OPCODE = 0x18
    LENGTH = 16
    OPERATION_STRUCT = struct.Struct("<BBhIBBhI")

    def __init__(
        self,
        registers,
        offset,
        immediate_l,
        immediate_h,
        address=0,
        compressed_address=0,
    ):
        self.immediate_l = immediate_l
        self.immediate_h = immediate_h
        immediate = (self.immediate_h << 32) + self.immediate_l
        super().__init__(registers, offset, immediate, address, compressed_address)

    @classmethod
    def from_bytes(cls, instruction: bytes, address=0, compressed_address=0):
        (
            opcode,
            registers,
            offset,
            immediate_l,
            _,
            _,
            _,
            immediate_h,
        ) = cls.OPERATION_STRUCT.unpack(instruction)
        if cls.opcode() != opcode:
            logging.critical(
                f"Opcode not matching expected, got {opcode}, expected {cls.opcode()}"
            )
            return None
        return cls(
            registers, offset, immediate_l, immediate_h, address, compressed_address
        )

    @classmethod
    def from_compressed(cls, instruction: bytes, address=0, compressed_address=0):
        fields = cls.COMPRESSED.unpack_from(instruction, 0)
        opcode, registers, offset, immediate = cls.expand_compressed(fields)
        if cls.opcode() != opcode:
            logging.critical(
                f"Opcode not matching expected, got {hex(opcode)}, expected {hex(cls.opcode())}"
            )
            return None
        immediate_l = immediate & 0xFFFFFFFF
        immediate_h = immediate >> 32
        return cls(
            registers, offset, immediate_l, immediate_h, address, compressed_address
        )

    def bytes(self):
        return self.OPERATION_STRUCT.pack(
            self.OPCODE,
            self.registers,
            self.offset,
            self.immediate_l,
            0,
            0,
            0,
            self.immediate_h,
        )

    def compress(self):
        return self.COMPRESSED.pack(self.OPCODE, self.registers, self.immediate)

    @classmethod
    def expand_compressed(cls, fields):
        return fields[0], fields[1], 0, fields[2]


class LDDWDInstruction(LDDWInstruction):

    OPCODE = 0xB8

    def asm_print(self):
        return f"r{self.dst_register} = {self.immediate} + .data"


class LDDWRInstruction(LDDWInstruction):

    OPCODE = 0xD8

    def asm_print(self):
        return f"r{self.dst_register} = {self.immediate} + .rodata"


class LDXDWInstruction(LoadXInstruction):

    OPCODE = 0x79


class LDXWInstruction(LoadXInstruction):

    OPCODE = 0x61


class LDXHInstruction(LoadXInstruction):

    OPCODE = 0x69


class LDXBInstruction(LoadXInstruction):

    OPCODE = 0x71


class STXDWInstruction(StoreXInstruction):

    OPCODE = 0x7B


class STXWInstruction(StoreXInstruction):

    OPCODE = 0x63


class STXHInstruction(StoreXInstruction):

    OPCODE = 0x6B


class STXBInstruction(StoreXInstruction):

    OPCODE = 0x73


class STDWInstruction(StoreInstruction):

    OPCODE = 0x7A


class STWInstruction(StoreInstruction):

    OPCODE = 0x62


class STHInstruction(StoreInstruction):

    OPCODE = 0x6A


class STBInstruction(StoreInstruction):

    OPCODE = 0x72


class BranchInstruction(Instruction):

    OPERAND = "=="
    COMPRESSED = struct.Struct("<BBh")

    def __init__(self, registers, offset, immediate, address=0, compressed_address=0):
        self.target = None
        super().__init__(registers, offset, immediate, address, compressed_address)

    def set_target(self, target: Instruction):
        self.target = target
        self.offset = int((self.target.address - self.address - self.LENGTH) / 8)

    @property
    def operand(self):
        return self.OPERAND

    def _compressed_offset(self):
        if self.target is None:
            return None
        return (
            self.target.compressed_address
            - self.compressed_address
            - self.compressed_size()
        )

    @classmethod
    def expand_compressed(cls, fields):
        return fields[0], fields[1], fields[2], 0

    def compressed_asm_print(self):
        return f"if r{self.dst_register} {self.operand} r{self.src_register} goto {'{:+}'.format(self._compressed_offset())}"

    def asm_print(self):
        return f"if r{self.dst_register} {self.operand} r{self.src_register} goto {self.offset}"

    def compress(self):
        return self.COMPRESSED.pack(
            self.OPCODE, self.registers, self._compressed_offset()
        )


class BranchImmInstruction(BranchInstruction):

    COMPRESSED = struct.Struct("<BBhI")

    @classmethod
    def expand_compressed(cls, fields):
        return fields

    def compressed_asm_print(self):
        return f"if r{self.dst_register} {self.operand} {self.immediate} goto {'{:+}'.format(self._compressed_offset())}"

    def asm_print(self):
        return f"if r{self.dst_register} {self.operand} {self.immediate} goto {self.offset}"

    def compress(self):
        return self.COMPRESSED.pack(
            self.OPCODE, self.registers, self._compressed_offset(), self.immediate
        )


class AlwaysBranchInstruction(BranchInstruction):

    OPCODE = 0x05
    COMPRESSED = struct.Struct("<BBh")

    def asm_print(self):
        return f"goto {self.offset}"

    def compress(self):
        return self.COMPRESSED.pack(self.OPCODE, self.registers)

    @classmethod
    def expand_compressed(cls, fields):
        return fields[0], fields[1], 0, 0


class EqBranchInstruction(BranchInstruction):

    OPERAND = "=="
    OPCODE = 0x1D


class EqBranchImmInstruction(BranchImmInstruction):

    OPERAND = "=="
    OPCODE = 0x15


class GtBranchInstruction(BranchInstruction):

    OPERAND = ">"
    OPCODE = 0x2D


class GtBranchImmInstruction(BranchImmInstruction):

    OPERAND = ">"
    OPCODE = 0x25


class GeBranchInstruction(BranchInstruction):

    OPERAND = ">="
    OPCODE = 0x3D


class GeBranchImmInstruction(BranchImmInstruction):

    OPERAND = ">="
    OPCODE = 0x35


class LtBranchInstruction(BranchInstruction):

    OPERAND = "<"
    OPCODE = 0xAD


class LtBranchImmInstruction(BranchImmInstruction):

    OPERAND = "<"
    OPCODE = 0xA5


class LeBranchInstruction(BranchInstruction):

    OPERAND = "<="
    OPCODE = 0xBD


class LeBranchImmInstruction(BranchImmInstruction):

    OPERAND = "<="
    OPCODE = 0xB5


class SetBranchInstruction(BranchInstruction):

    OPERAND = "&"
    OPCODE = 0x4D


class SetBranchImmInstruction(BranchImmInstruction):

    OPERAND = "&"
    OPCODE = 0x45


class NeBranchInstruction(BranchInstruction):

    OPERAND = "!="
    OPCODE = 0x5D


class NeBranchImmInstruction(BranchImmInstruction):

    OPERAND = "!="
    OPCODE = 0x55


class SGtBranchInstruction(BranchInstruction):

    OPERAND = ">"
    OPCODE = 0x6D


class SGtBranchImmInstruction(BranchImmInstruction):

    OPERAND = ">"
    OPCODE = 0x65


class SGeBranchInstruction(BranchInstruction):

    OPERAND = ">="
    OPCODE = 0x7D


class SGeBranchImmInstruction(BranchImmInstruction):

    OPERAND = ">="
    OPCODE = 0x75


class SLtBranchInstruction(BranchInstruction):

    OPERAND = "<"
    OPCODE = 0xCD


class SLtBranchImmInstruction(BranchImmInstruction):

    OPERAND = "<"
    OPCODE = 0xC5


class SLeBranchInstruction(BranchInstruction):

    OPERAND = "<="
    OPCODE = 0xDD


class SLeBranchImmInstruction(BranchImmInstruction):

    OPERAND = "<="
    OPCODE = 0xD5


class CallInstruction(AluImmInstruction):

    OPCODE = 0x85

    def asm_print(self):
        return f"Call {self.immediate}"


class ReturnInstruction(Instruction):

    COMPRESSED = struct.Struct("<BB")

    OPCODE = 0x95

    def asm_print(self):
        return f"Return r0"

    def compress(self):
        return self.COMPRESSED.pack(self.OPCODE, self.registers)


INSTRUCTIONS = {
    AddImmInstruction.OPCODE: AddImmInstruction,
    AddInstruction.OPCODE: AddInstruction,
    SubImmInstruction.OPCODE: SubImmInstruction,
    SubInstruction.OPCODE: SubInstruction,
    MulImmInstruction.OPCODE: MulImmInstruction,
    MulInstruction.OPCODE: MulInstruction,
    DivImmInstruction.OPCODE: DivImmInstruction,
    DivInstruction.OPCODE: DivInstruction,
    OrImmInstruction.OPCODE: OrImmInstruction,
    OrInstruction.OPCODE: OrInstruction,
    AndImmInstruction.OPCODE: AndImmInstruction,
    AndInstruction.OPCODE: AndInstruction,
    LSHImmInstruction.OPCODE: LSHImmInstruction,
    LSHInstruction.OPCODE: LSHInstruction,
    RSHImmInstruction.OPCODE: RSHImmInstruction,
    RSHInstruction.OPCODE: RSHInstruction,
    NegInstruction.OPCODE: NegInstruction,
    ModImmInstruction.OPCODE: ModImmInstruction,
    ModInstruction.OPCODE: ModInstruction,
    XorImmInstruction.OPCODE: XorImmInstruction,
    XorInstruction.OPCODE: XorInstruction,
    MovImmInstruction.OPCODE: MovImmInstruction,
    MovInstruction.OPCODE: MovInstruction,
    ARSHImmInstruction.OPCODE: ARSHImmInstruction,
    ARSHInstruction.OPCODE: ARSHInstruction,
    LDDWInstruction.OPCODE: LDDWInstruction,
    LDXDWInstruction.OPCODE: LDXDWInstruction,
    LDXWInstruction.OPCODE: LDXWInstruction,
    LDXHInstruction.OPCODE: LDXHInstruction,
    LDXBInstruction.OPCODE: LDXBInstruction,
    STXDWInstruction.OPCODE: STXDWInstruction,
    STXWInstruction.OPCODE: STXWInstruction,
    STXHInstruction.OPCODE: STXHInstruction,
    STXBInstruction.OPCODE: STXBInstruction,
    STDWInstruction.OPCODE: STDWInstruction,
    STWInstruction.OPCODE: STWInstruction,
    STHInstruction.OPCODE: STHInstruction,
    STBInstruction.OPCODE: STBInstruction,
    AlwaysBranchInstruction.OPCODE: AlwaysBranchInstruction,
    EqBranchInstruction.OPCODE: EqBranchInstruction,
    EqBranchImmInstruction.OPCODE: EqBranchImmInstruction,
    GtBranchInstruction.OPCODE: GtBranchInstruction,
    GtBranchImmInstruction.OPCODE: GtBranchImmInstruction,
    GeBranchInstruction.OPCODE: GeBranchInstruction,
    GeBranchImmInstruction.OPCODE: GeBranchImmInstruction,
    LtBranchInstruction.OPCODE: LtBranchInstruction,
    LtBranchImmInstruction.OPCODE: LtBranchImmInstruction,
    LeBranchInstruction.OPCODE: LeBranchInstruction,
    LeBranchImmInstruction.OPCODE: LeBranchImmInstruction,
    SetBranchInstruction.OPCODE: SetBranchInstruction,
    SetBranchImmInstruction.OPCODE: SetBranchImmInstruction,
    NeBranchInstruction.OPCODE: NeBranchInstruction,
    NeBranchImmInstruction.OPCODE: NeBranchImmInstruction,
    SGtBranchInstruction.OPCODE: SGtBranchInstruction,
    SGtBranchImmInstruction.OPCODE: SGtBranchImmInstruction,
    SGeBranchInstruction.OPCODE: SGeBranchInstruction,
    SGeBranchImmInstruction.OPCODE: SGeBranchImmInstruction,
    SLtBranchInstruction.OPCODE: SLtBranchInstruction,
    SLtBranchImmInstruction.OPCODE: SLtBranchImmInstruction,
    SLeBranchInstruction.OPCODE: SLeBranchInstruction,
    SLeBranchImmInstruction.OPCODE: SLeBranchImmInstruction,
    CallInstruction.OPCODE: CallInstruction,
    ReturnInstruction.OPCODE: ReturnInstruction,
    # Custom rBPF
    LDDWDInstruction.OPCODE: LDDWDInstruction,
    LDDWRInstruction.OPCODE: LDDWRInstruction,
}


def from_bytes(instruction: bytes):
    opcode = instruction[0]
    instr = None
    if opcode in INSTRUCTIONS:
        instr = INSTRUCTIONS[opcode](instruction)
    return instr


def parse_text(text: bytes, compressed=False):
    instructions = []
    offset = 0
    compressed_offset = 0
    while (offset < len(text) and not compressed) or (
        compressed_offset < len(text) and compressed
    ):
        opcode = text[offset] if not compressed else text[compressed_offset]
        if opcode not in INSTRUCTIONS:
            logging.critical(f"Instruction {hex(opcode)} not found")
            return None
        instruction_type = INSTRUCTIONS[opcode]
        logging.debug(
            f"Found opcode {hex(opcode)} at {hex(offset)}/{hex(compressed_offset)} with width {instruction_type.LENGTH if not compressed else instruction_type.compressed_size()}"
        )
        if compressed:
            text_slice = text[compressed_offset:]
            instruction = instruction_type.from_compressed(
                text_slice, offset, compressed_offset
            )
        else:
            text_slice = text[offset : offset + instruction_type.LENGTH]
            instruction = instruction_type.from_bytes(
                text_slice, offset, compressed_offset
            )
        compressed_offset += instruction_type.compressed_size()
        offset += instruction_type.LENGTH
        instructions.append(instruction)

    for instruction in instructions:
        if isinstance(instruction, BranchInstruction):
            logging.debug(
                f"Instruction {type(instruction)} at {hex(instruction.address)} with offset is {instruction.offset}"
            )
            if compressed:
                target_address = (
                    instruction.compressed_address
                    + instruction.offset
                    + instruction.compressed_size()
                )
                logging.debug(f"Compressed address target at {hex(target_address)}")
            else:
                target_address = instruction.address + (instruction.offset + 1) * 8
                logging.debug(
                    f"target {hex(target_address)} = {instruction.address} + {instruction.offset} + 1"
                )
            for instr in instructions:
                compare_address = (
                    instr.compressed_address if compressed else instr.address
                )
                if compare_address == target_address:
                    instruction.set_target(instr)
                    logging.info(
                        f"Target address: {hex(target_address)} Matching {instruction} to {instr}"
                    )
                    break
            else:
                logging.critical(f"No target found for {instruction}")
    return instructions


def compress():
    pass
