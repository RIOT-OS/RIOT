nodemap = {}

def register(name, _class):
    nodemap[name] = _class

def get(name):
    return nodemap.get(name)
