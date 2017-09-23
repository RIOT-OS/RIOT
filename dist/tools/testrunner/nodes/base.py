import os

class BaseNode(object):
    def __init__(s, _dict):
        s.data = _dict
        s.name=_dict.get("name", "unnamed")
        s.timeout=_dict.get("timeout", 10)
        s.tags = _dict.get("tags", set())

        s.env=os.environ.copy()
        env = _dict.get("env", {})
        if env:
            s.env.update(env)

        s.child=None

    def log(s, *args):
        print("%s:" % s.name, *args)
