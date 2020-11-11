if request.isInit:
    lastVal = -1
elif request.isRead:
    request.value = lastVal + 1
    lastVal += 1

self.NoisyLog("%s on COUNTER at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
