if request.isInit:
    lastVal = 0
elif request.isRead:
    request.value = lastVal
elif request.isWrite:
    lastVal = request.value

self.NoisyLog("%s on REPEATER at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
