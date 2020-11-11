if request.isInit:
	lastVal = 0
else:
	lastVal = lastVal + 1
	request.value = lastVal

self.NoisyLog("%s on TICKER at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
