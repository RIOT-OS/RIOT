import struct
import logging
from collections import namedtuple
from elftools.elf.elffile import ELFFile
from rbpf import instructions
import itertools

MAGIC = int.from_bytes(b"rBPF", "little")

HEADER_STRUCT = struct.Struct("<IIIIIII")
HEADER = namedtuple(
    "Header", "magic version flags data_len rodata_len text_len functions_len"
)

SYMBOL_STRUCT = struct.Struct("<HHH")
SYMBOL = namedtuple("Symbol", "name_offset flags location_offset")

TEXT = ".text"
DATA = ".data"
RODATA = ".rodata"
SYMBOLS = ".symtab"
RELOCATIONS = ".rel.text"

COMPRESSED = 0x01


class Symbol(object):
    def __init__(self, location, name, instruction=None):
        self.location = location
        self.name = name
        self.instruction = instruction


class RBF(object):
    def __init__(self, data, rodata, text, symbols, header=None):
        self.data = data
        self.rodata = rodata
        self.text = text
        self.header = header
        if header:
            self.flags = self.header.flags
        else:
            self.flags = 0
        compressed = bool(self.flags & COMPRESSED)
        self.instructions = instructions.parse_text(self.text, compressed=compressed)
        self.symbols = symbols

        def _round_len(bstr):
            if (len(bstr) % 8) != 0:
                bytes_to_append = 8 - len(bstr) % 8
                logging.debug(f"appending {bytes_to_append} bytes")
                bstr += bytes([0x00] * bytes_to_append)

        _round_len(self.data)
        _round_len(self.rodata)

        if (len(text) % 8) != 0:
            logging.error(
                f"Length of the text is not a whole number of instructions: {len(text)}"
            )

    def _parse_symbols(self, symbols):
        syms = []
        for symbol in symbols:
            rodata_offset = symbol.name_offset
            name = self.rodata[rodata_offset:].split(b"\00")[0].decode("ascii")
            instruction = self.instruction_by_address(symbol.location_offset)
            syms.append(Symbol(symbol.location_offset, name, instruction))
        return syms

    def instruction_by_address(self, address):
        for instruction in self.instructions:
            if instruction.address == address:
                return instruction
        return None

    @staticmethod
    def _hex_dump(data):
        return " ".join(map("0x{0:0>2X}".format, data))

    @staticmethod
    def _split_instructions(data):
        iterator = [iter(data)] * 8
        return list(itertools.zip_longest(*iterator))

    @staticmethod
    def obj_hexstr(bstr):
        addr = 0
        while len(bstr) > 0:
            slice_len = 8 if len(bstr) > 8 else len(bstr)
            line = bstr[:slice_len]
            bstr = bstr[slice_len:]
            yield "{:>5x}: ".format(addr) + " ".join(
                map("0x{0:0>2x}".format, line)
            ) + "\n"
            addr += 8

    def dump(self, compressed=False):
        print(
            f"Magic:\t\t{hex(self.header.magic)}\n"
            f"Version:\t{self.header.version}\n"
            f"flags:\t{hex(self.flags)}\n"
            f"Data length:\t{self.header.data_len} B\n"
            f"RoData length:\t{self.header.rodata_len} B\n"
            f"Text length:\t{self.header.text_len} B\n"
            f"No. functions:\t{self.header.functions_len}\n"
        )
        print("functions:")
        syms = {sym.location: sym for sym in self._parse_symbols(self.symbols)}
        for symbol in syms.values():
            print(f'\t"{symbol.name}": {hex(symbol.location)}')
        print()

        print("data:")
        print("".join(data for data in RBF.obj_hexstr(self.data)))

        print("rodata:")
        print("".join(data for data in RBF.obj_hexstr(self.rodata)))

        print("text:")
        for instr in self.instructions:
            if compressed:
                print(instr.compressed_print())
            else:
                if instr.address in syms:
                    symbol = syms[instr.address]
                    print(f"<{symbol.name}>")
                print(instr.full_print())

    def format(self):
        if not self.header:
            self.header = HEADER(
                MAGIC,
                0,
                0,
                len(self.data),
                len(self.rodata),
                len(self.text),
                len(self.symbols),
            )

        data = bytearray(HEADER_STRUCT.pack(*self.header))
        data += self.data
        data += self.rodata
        data += self.text
        for symbol in self.symbols:
            data += SYMBOL_STRUCT.pack(*symbol)
        return data

    def format_compressed(self):
        compressed_text = bytes().join(instr.compress() for instr in self.instructions)
        if not self.header:
            self.header = HEADER(
                MAGIC,
                0,
                COMPRESSED,
                len(self.data),
                len(self.rodata),
                len(compressed_text),
                len(self.symbols),
            )
        data = bytearray(HEADER_STRUCT.pack(*self.header))
        data += self.data
        data += self.rodata
        data += compressed_text
        for symbol in self.symbols:
            data += SYMBOL_STRUCT.pack(*symbol)
        return data

    @staticmethod
    def from_rbf(byte_data):
        header = HEADER._make(HEADER_STRUCT.unpack_from(byte_data, 0))
        offset = HEADER_STRUCT.size
        data_start = offset
        offset += header.data_len
        rodata_start = data_end = offset
        offset += header.rodata_len
        text_start = rodata_end = offset
        offset += header.text_len
        syms_start = text_end = offset
        rodata = byte_data[rodata_start:rodata_end]
        data = byte_data[data_start:data_end]
        text = byte_data[text_start:text_end]
        syms = byte_data[syms_start:]

        syms_array = []
        while len(syms):
            syms_array.append(SYMBOL._make(SYMBOL_STRUCT.unpack_from(syms, 0)))
            syms = syms[SYMBOL_STRUCT.size :]
        return RBF(data, rodata, text, syms_array, header)

    @staticmethod
    def _get_section_lddw_opcode(section):
        if section == RODATA:
            return instructions.LDDWR_OPCODE
        elif section == DATA:
            return instructions.LDDWD_OPCODE

    @staticmethod
    def _patch_text(text, elffile, relocation):
        entry = relocation.entry
        location = entry.r_offset
        symbols = elffile.get_section_by_name(SYMBOLS)
        symbol = symbols.get_symbol(entry.r_info_sym)
        if symbol.entry.st_info.type == "STT_SECTION":
            # refers to an offset in a section
            section_name = elffile.get_section(symbol.entry.st_shndx).name
            offset = 0
            pass
        elif symbol.entry.st_info.type == "STT_OBJECT":
            section_name = elffile.get_section(symbol.entry.st_shndx).name
            offset = symbol.entry.st_value
        opcode = RBF._get_section_lddw_opcode(section_name)
        if text[location] != instructions.LDDW_OPCODE:
            logging.error(f"No LDDW instruction at {hex(location)}")
        else:
            instruction = instructions.LDDW._make(
                instructions.LDDW_STRUCT.unpack_from(text, location)
            )
            logging.info(
                f"Replacing {instruction} at {location} with {opcode} at {offset}"
            )
            text[location : location + 16] = instructions.LDDW_STRUCT.pack(
                opcode,
                instruction.registers,
                instruction.offset,
                instruction.immediate_l + offset,
                0,
                0,
                0,
                instruction.immediate_h,
            )

    @staticmethod
    def from_elf(elf, relocations=True):
        # Read data from the input file and construct the RBF object
        elffile = ELFFile(elf)
        relocations = elffile.get_section_by_name(RELOCATIONS)
        elf_text = elffile.get_section_by_name(TEXT)
        elf_data = elffile.get_section_by_name(DATA)
        elf_rodata = elffile.get_section_by_name(RODATA)
        if not elf_rodata:
            rodata = bytearray()
        else:
            rodata = bytearray(elf_rodata.data())
        if not elf_text:
            text = bytearray()
        else:
            text = bytearray(elf_text.data())
        if not elf_data:
            data = bytearray()
        else:
            data = bytearray(elf_data.data())

        symbols = elffile.get_section_by_name(SYMBOLS)

        rbf_symbols = []
        for symbol in symbols.iter_symbols():
            entry = symbol.entry
            info = entry["st_info"]
            if info["type"] == "STT_FUNC" and info["bind"] == "STB_GLOBAL":
                name = symbol.name
                text_offset = entry["st_value"]
                logging.info(f"Found global function {name} at offset {text_offset}")
                rbf_symbols.append((name, text_offset, 0))  # potential flags

        symbol_structs = []
        logging.debug(f"rodata length: {len(rodata)}")
        for name, text_offset, flags in rbf_symbols:
            offset = len(rodata)
            rodata += bytes(name, "UTF-8") + b"\00"
            sym_str = SYMBOL(offset, flags, text_offset)
            symbol_structs.append(sym_str)
            logging.debug(
                f"symbol {sym_str} generated with {name} and appended at {offset}"
            )
        logging.info(f"Total rodata size: {len(rodata)}. Total data size: {len(data)}")

        if relocations:
            for relocation in relocations.iter_relocations():
                logging.debug(relocation.entry)
                entry = relocation.entry
                symbol = symbols.get_symbol(entry["r_info_sym"])
                if symbol.entry["st_info"]["type"] == "STT_SECTION":
                    name = elffile.get_section(symbol.entry["st_shndx"]).name
                    logging.info(
                        f"relocation at instruction {hex(entry['r_offset'])} for section {name} at offset {symbol.entry.st_value}"
                    )
                else:
                    name = symbol.name
                    section = elffile.get_section(symbol.entry.st_shndx)
                    logging.info(
                        f"relocation at instruction {hex(entry['r_offset'])} for symbol {name} in {section.name} at {symbol.entry.st_value}"
                    )

                RBF._patch_text(text, elffile, relocation)

        return RBF(data=data, rodata=rodata, text=text, symbols=symbol_structs)
