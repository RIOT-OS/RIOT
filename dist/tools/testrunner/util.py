import traceback, _thread
from itertools import product

def listify(something):
    if not something:
        return []
    if not type(something)==list:
        return [something]
    return something

def uniquify(seq):
    # Order preserving
    seen = set()
    return [x for x in seq if x not in seen and not seen.add(x)]

def print_exception():
    traceback.print_exc()

def dict_list_tuples2(_dict):
    for key, value_list in _dict.items():
        for value in value_list:
            yield (key,value)

def dict_list_tuples(_dict):
    _dict = _dict or {}
    for key in _dict.keys():
        yield dict_list_tuples2({key:_dict[key]})

def dict_list_product(_dict):
    for _tuple in product(*dict_list_tuples(_dict)):
        res = {}
        for key, val in _tuple:
            res[key] = val

        yield(res)

def deep_replace(obj, replace):
    if type(obj) == list:
        _obj = []
        for entry in obj:
            _obj.append(deep_replace(entry, replace))
        return _obj
    elif type(obj) == dict:
        _obj = {}
        for key, val in obj.items():
            _obj[key] = deep_replace(val, replace)
        return _obj
    elif type(obj) == str:
        for key, val in replace.items():
            obj = obj.replace(key, val)
        return obj
    else:
        return obj

_main_interrupted = False
def interrupt_main():
    global _main_interrupted
    _main_interrupted = True
    _thread.interrupt_main()

def is_main_interrupted():
    global _main_interrupted
    if _main_interrupted:
        _main_interrupted = False
        return True
