if request.isInit:
    lastVal = 0
else:
    lastVal = 1 - lastVal
    request.value = lastVal * 0xFFFFFFFF

self.NoisyLog("%s on FLIPFLOP at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
