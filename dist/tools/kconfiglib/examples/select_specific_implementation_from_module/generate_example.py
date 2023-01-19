import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-m', '--module-name', default='FOO',
                    help='Module name [FOO]')
parser.add_argument('-n', '--num', default=3, type=int,
                    help='Amount of implementations [3]')
parser.add_argument('-i', '--ignore-has', action='store_false',
                    help='prevents HAS_*')

args = parser.parse_args()

module_name = args.module_name
implementation_amount = args.num
has = args.ignore_has

print(f"""menuconfig MODULE_{module_name}
    bool "Module {module_name}"

if MODULE_{module_name}
""")
for idx in range(implementation_amount):
    print(f"""config REQUIRES_{module_name}_{idx}
    bool
""")

print(f"""config ERROR_{module_name}
    bool""")

for idx in range(implementation_amount):
    print(f"    default y if !MODULE_{module_name}_{idx} && REQUIRES_{module_name}_{idx}")

print(f"""
choice {module_name}_IMPLEMENTATION
    bool
    prompt "{module_name} Implementation\"""")
for idx in range(implementation_amount):
    print(f"    default MODULE_{module_name}_{idx} if REQUIRES_{module_name}_{idx}")
if has:
    for idx in range(implementation_amount):
        print(f"    default MODULE_{module_name}_{idx} if HAS_{module_name}_{idx}")


for idx in range(implementation_amount):
    print(f"""
config MODULE_{module_name}_{idx}
bool "Implementation {idx}\"""")

print(f"\nendchoice # {module_name}_IMPLEMENTATION")
print(f"endif # MODULE_{module_name}\n")

if has:
    for idx in range(implementation_amount):
        print(f"""config HAS_{module_name}_{idx}
    bool
""")
