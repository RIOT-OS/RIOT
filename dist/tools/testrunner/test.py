from util import deep_replace, dict_list_product

#repl = { "%A%" : [ "a", "b" ] }
repl = { "%A%" : "xx" }

template = {
        "%A%" : ["x", "y" ],
        "%B%" : ["c", "d" ],
        }


d = { "A" : { "A" : "%A%", "B" : [ "B1%B%", "B2%A%" ] } }

for repl in dict_list_product(template):
    print(deep_replace(d, repl))
