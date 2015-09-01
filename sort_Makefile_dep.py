import re

if __name__ == '__main__':
    with open('Makefile.dep', 'rt', 1) as f:
        s = list(str.rstrip(s) for s in f)

    lines = [[]]
    for s in s:
        if s:
            lines[-1].append(s)
        elif lines[-1]:
            lines.append([])

    lines = map('\n'.join, lines)
    lines = { re.findall(r'filter ([^,]+),', s)[0]: s for s in lines }
    lines = sorted(lines.items())
    lines = '\n\n'.join(s[1] for s in lines)

    with open('Makefile.dep', 'wt', 1) as f:
        f.write(lines + '\n')
