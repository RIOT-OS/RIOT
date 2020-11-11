if request.isInit:
    lastVal = 0
else:
    if lastVal == 0:
        lastVal = 1
    else:
        lastVal = (lastVal << 1) & 0xFFFFFFFF
    request.value = lastVal

self.NoisyLog("%s on ROLLING_BIT at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
