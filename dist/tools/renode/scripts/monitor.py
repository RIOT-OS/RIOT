from time import sleep

current_value = 0

def mc_next_value(offset = 0):
    global current_value
    print "%d" % (current_value + offset)
    current_value = current_value + 1

def mc_sleep(time):
    sleep(float(time))

def mc_console_log(string):
    System.Console.WriteLine(string)

def mc_echo(*value):
    if len(value) == 2:
        if value[0] == "-n":
            sys.stdout.write(value[1])
            return
    elif len(value) == 1:
        print value[0]
        return
    elif len(value) == 0:
        print
        return
    print "usage: echo [-n] [string]"

def mc_dump(mem_start_val, mem_count_val, wid_val = 16):
    wid = int(wid_val)
    sysbus = self.Machine["sysbus"]
    mem_start = int(mem_start_val)
    mem_count = int(mem_count_val)
    for a in range(mem_start, mem_start + mem_count, wid):
        data = sysbus.ReadBytes(a, wid)
        print "0x%08X |" % a,
        for b in range(0, wid):
            print "%02X" % data[b] ,
        print "| " ,
        for b in range(0, wid):
            c = data[b]
            if not ((c < 0x20) or (c > 127)):
                print "%c%c" % (chr(8), chr(c)) ,
            else:
                print "%c." % chr(8) ,
        print

def mc_dump_file(mem_start_val, mem_count_val, filename):
    sysbus = self.Machine["sysbus"]
    mem_start = int(mem_start_val)
    mem_count = int(mem_count_val)
    tab = sysbus.ReadBytes(mem_start, mem_count)
    fl = System.IO.FileStream(filename, System.IO.FileMode.Create, System.IO.FileAccess.Write)
    fl.Write(tab, 0, mem_count)
    fl.Close()

def mc_get_environ(variable):
    v = System.Environment.GetEnvironmentVariable(variable)
    if v != None:
        print v
