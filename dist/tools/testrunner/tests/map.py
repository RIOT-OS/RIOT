testmap = {}

def register(name, _class):
    testmap[name] = _class

def get(name):
    return testmap.get(name)
