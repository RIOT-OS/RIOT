/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <algorithm>
#include <iostream>
#include <cinttypes>
#include "output_format.h"
#include "pio_disassembler.h"

struct riot_output : public output_format {
    struct factory {
        factory() {
            output_format::add(new riot_output());
        }
    };

    riot_output() : output_format("RIOT") {}

    std::string get_description() override
    {
        return "C header suitable for use with RIOT-OS";
    }

    void output_symbols(FILE *out, std::string prefix, const std::vector<compiled_source::symbol> &symbols)
    {
        int count = 0;
        for (const auto &s : symbols) {
            if (!s.is_label) {
                fprintf(out, "#define %s%s %d\n", prefix.c_str(), s.name.c_str(), s.value);
                count++;
            }
        }
        fprintf(out, count ? "\n" : "");
        count = 0;
        for (const auto &s : symbols) {
            if (s.is_label) {
                fprintf(out, "#define %sLABEL_%s %d\n", prefix.c_str(), s.name.c_str(), s.value);
                count++;
            }
        }
        fprintf(out, count ? "\n" : "");
    }

    int output(std::string destination, std::vector<std::string> output_options,
               const compiled_source &source) override
    {
        for (const auto &program : source.programs) {
            for(const auto &p : program.lang_opts) {
                if (p.first.size() >= name.size() && p.first.compare(0, name.size(), name) == 0) {
                    std::cerr << "warning: " << name << " does not support output options; " << p.first << " lang_opt ignored.\n";
                }
            }
        }
        FILE *out = open_single_output(destination);
        if (!out) {
            return 1;
        }
        std::string fname = destination.substr(destination.find_last_of("/\\") + 1);
                    fname = fname.substr(0, std::max(fname.find_first_of("."), (std::size_t)0));
        std::transform(fname.begin(), fname.end(), fname.begin(), ::toupper);
        fprintf(out, "#ifndef PIO_%s_H\n", fname.c_str());
        fprintf(out, "#define PIO_%s_H\n", fname.c_str());
        fprintf(out, "\n"
                     "#include <stdbool.h>\n"
                     "#include \"periph/pio.h\"\n");
        fprintf(out, "\n"
                     "#ifdef __cplusplus\n"
                     "extern \"C\" {\n"
                     "#endif\n");
        fprintf(out, "\n");
        output_symbols(out, "", source.global_symbols);

        for (const auto &program : source.programs) {
            std::string prefix = "PIO_" + program.name + "_";
            std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
            fprintf(out, "/* %s */\n", program.name.c_str());
            fprintf(out, "#define %sSIDESET_COUNT       %d\n", prefix.c_str(),
                program.sideset_bits_including_opt.is_specified()
                    ? program.sideset_bits_including_opt.value - !!program.sideset_opt : 0);
            fprintf(out, "#define %sSIDESET_OPTIONAL    %s\n", prefix.c_str(), program.sideset_opt ? "true" : "false");
            fprintf(out, "#define %sSIDESET_PINDIRS     %s\n", prefix.c_str(), program.sideset_pindirs ? "true" : "false");
            fprintf(out, "#define %sWRAP_BOTTOM         %d\n", prefix.c_str(), program.wrap_target);
            fprintf(out, "#define %sWRAP_TOP            %d\n", prefix.c_str(), program.wrap);
            fprintf(out, "\n");
            output_symbols(out, prefix, program.symbols);
            fprintf(out, "#define %sPROGRAM { \\\n", prefix.c_str());
            for (int i = 0; i < (int)program.instructions.size(); i++) {
                const auto &inst = program.instructions[i];
                const char comma = i == (int)program.instructions.size() -1 ? ' ' : ',';
                const std::string disasm = disassemble(inst,
                                                       program.sideset_bits_including_opt.get(),
                                                       program.sideset_opt);
                if (i == program.wrap_target) {
                    fprintf(out, "    /* .wrap_target */ \\\n");
                }
                fprintf(out, "    0x%04x%c /* %2d: %s */ \\\n", inst, comma, i, disasm.c_str());
                if (i == program.wrap) {
                    fprintf(out, "    /* .wrap */ \\\n");
                }
            }
            fprintf(out, "}\n");
            fprintf(out, "#define %sPROGRAM_LENGTH %ld\n", prefix.c_str(), program.instructions.size());
            fprintf(out, "#ifndef %sLABEL_entry_point\n"
                         "#define %sLABEL_entry_point 0\n"
                         "#endif\n",
                         prefix.c_str(), prefix.c_str());
            fprintf(out, "#define %sPROGRAM_CONF { \\\n"
                         "    %sLABEL_entry_point, \\\n"
                         "    %sWRAP_BOTTOM, \\\n"
                         "    %sWRAP_TOP, \\\n"
                         "    %sSIDESET_COUNT, \\\n"
                         "    %sSIDESET_OPTIONAL, \\\n"
                         "    %sSIDESET_PINDIRS \\\n"
                         "}\n",
                         prefix.c_str(), prefix.c_str(), prefix.c_str(), prefix.c_str(),
                         prefix.c_str(), prefix.c_str(), prefix.c_str()
                   );

            /* generate functions */
            fprintf(out, "\n");
            fprintf(out, "static inline pio_program_t pio_%s_create_program(void)\n"
                          "{\n"
                          "    pio_program_t pro = {\n"
                          "        .instr_numof = %sPROGRAM_LENGTH,\n"
                          "        .location = PIO_PROGRAM_NOT_LOADED\n"
                          "    };\n"
                          "    return pro;\n"
                          "}\n",
                          program.name.c_str(),
                          prefix.c_str());

            /* todo maybe have some code blocks inside or outside here? */
            for(const auto& o : program.code_blocks) {
                fprintf(out, "\n");
                if (o.first == name) {
                    for(const auto &contents : o.second) {
                        fprintf(out, "%s", contents.c_str());
                        fprintf(out, "\n");
                    }
                }
            }
        }

        fprintf(out, "#ifdef __cplusplus\n"
                     "}\n"
                     "#endif\n");
        fprintf(out, "#endif /* PIO_%s_H */\n", fname.c_str());

        if (out != stdout) {
            fclose(out);
        }
        return 0;
    }
};

static riot_output::factory creator;
