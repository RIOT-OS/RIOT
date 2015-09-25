import unittest

int_pointer_type=gdb.lookup_type('int').pointer()

TIMER0_TAR=0x40030048
TIMER3_TAR=0x40033048


def mem_content(address):
	addr_val = gdb.Value(address)
	addr_ptr = addr_val.cast(int_pointer_type)
	return long(addr_ptr.dereference())

class XTimerTest(unittest.TestCase):

	def test_xtimer_period(self):
		gdb.execute("break main.c:102")
		gdb.execute("continue")
		start_time=mem_content(TIMER3_TAR)
		gdb.execute("continue")
		end_time=mem_content(TIMER3_TAR)
		print(end_time-start_time)

		self.assertLessEqual(4000000, end_time-start_time)
		self.assertLessEqual(end_time-start_time, 4002000)

		# get SP register value
		#print(gdb.parse_and_eval("$sp"))
		#print(gdb.execute("x 0x40033048"))

unittest.main()



