-- $Id: nextvar.lua,v 1.79 2016/11/07 13:11:28 roberto Exp $
-- See Copyright Notice in file all.lua

print('testing tables, next, and for')

local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end


local a = {}

-- make sure table has lots of space in hash part
for i=1,100 do a[i.."+"] = true end
for i=1,100 do a[i.."+"] = nil end
-- fill hash part with numeric indices testing size operator
for i=1,100 do
  a[i] = true
  assert(#a == i)
end

-- testing ipairs
local x = 0
for k,v in ipairs{10,20,30;x=12} do
  x = x + 1
  assert(k == x and v == x * 10)
end

for _ in ipairs{x=12, y=24} do assert(nil) end

-- test for 'false' x ipair
x = false
local i = 0
for k,v in ipairs{true,false,true,false} do
  i = i + 1
  x = not x
  assert(x == v)
end
assert(i == 4)

-- iterator function is always the same
assert(type(ipairs{}) == 'function' and ipairs{} == ipairs{})


if not T then
  (Message or print)
    ('\n >>> testC not active: skipping tests for table sizes <<<\n')
else --[
-- testing table sizes

local function log2 (x) return math.log(x, 2) end

local function mp2 (n)   -- minimum power of 2 >= n
  local mp = 2^math.ceil(log2(n))
  assert(n == 0 or (mp/2 < n and n <= mp))
  return mp
end

local function fb (n)
  local r, nn = T.int2fb(n)
  assert(r < 256)
  return nn
end

-- test fb function
for a = 1, 10000 do   -- all numbers up to 10^4
  local n = fb(a)
  assert(a <= n and n <= a*1.125)
end
local a = 1024   -- plus a few up to 2 ^30
local lim = 2^30
while a < lim do
  local n = fb(a)
  assert(a <= n and n <= a*1.125)
  a = math.ceil(a*1.3)
end

 
local function check (t, na, nh)
  local a, h = T.querytab(t)
  if a ~= na or h ~= nh then
    print(na, nh, a, h)
    assert(nil)
  end
end


-- testing C library sizes
do
  local s = 0
  for _ in pairs(math) do s = s + 1 end
  check(math, 0, mp2(s))
end


-- testing constructor sizes
local lim = 40
local s = 'return {'
for i=1,lim do
  s = s..i..','
  local s = s
  for k=0,lim do 
    local t = load(s..'}', '')()
    assert(#t == i)
    check(t, fb(i), mp2(k))
    s = string.format('%sa%d=%d,', s, k, k)
  end
end


-- tests with unknown number of elements
local a = {}
for i=1,lim do a[i] = i end   -- build auxiliary table
for k=0,lim do
  local a = {table.unpack(a,1,k)}
  assert(#a == k)
  check(a, k, 0)
  a = {1,2,3,table.unpack(a,1,k)}
  check(a, k+3, 0)
  assert(#a == k + 3)
end


-- testing tables dynamically built
local lim = 130
local a = {}; a[2] = 1; check(a, 0, 1)
a = {}; a[0] = 1; check(a, 0, 1); a[2] = 1; check(a, 0, 2)
a = {}; a[0] = 1; a[1] = 1; check(a, 1, 1)
a = {}
for i = 1,lim do
  a[i] = 1
  assert(#a == i)
  check(a, mp2(i), 0)
end

a = {}
for i = 1,lim do
  a['a'..i] = 1
  assert(#a == 0)
  check(a, 0, mp2(i))
end

a = {}
for i=1,16 do a[i] = i end
check(a, 16, 0)
do
  for i=1,11 do a[i] = nil end
  for i=30,50 do a[i] = nil end   -- force a rehash (?)
  check(a, 0, 8)   -- only 5 elements in the table
  a[10] = 1
  for i=30,50 do a[i] = nil end   -- force a rehash (?)
  check(a, 0, 8)   -- only 6 elements in the table
  for i=1,14 do a[i] = nil end
  for i=18,50 do a[i] = nil end   -- force a rehash (?)
  check(a, 0, 4)   -- only 2 elements ([15] and [16])
end

-- reverse filling
for i=1,lim do
  local a = {}
  for i=i,1,-1 do a[i] = i end   -- fill in reverse
  check(a, mp2(i), 0)
end

-- size tests for vararg
lim = 35
function foo (n, ...)
  local arg = {...}
  check(arg, n, 0)
  assert(select('#', ...) == n)
  arg[n+1] = true
  check(arg, mp2(n+1), 0)
  arg.x = true
  check(arg, mp2(n+1), 1)
end
local a = {}
for i=1,lim do a[i] = true; foo(i, table.unpack(a)) end

end  --]


-- test size operation on empty tables
assert(#{} == 0)
assert(#{nil} == 0)
assert(#{nil, nil} == 0)
assert(#{nil, nil, nil} == 0)
assert(#{nil, nil, nil, nil} == 0)
print'+'


local nofind = {}

a,b,c = 1,2,3
a,b,c = nil


-- next uses always the same iteraction function
assert(next{} == next{})

local function find (name)
  local n,v
  while 1 do
    n,v = next(_G, n)
    if not n then return nofind end
    assert(v ~= nil)
    if n == name then return v end
  end
end

local function find1 (name)
  for n,v in pairs(_G) do
    if n==name then return v end
  end
  return nil  -- not found
end


assert(print==find("print") and print == find1("print"))
assert(_G["print"]==find("print"))
assert(assert==find1("assert"))
assert(nofind==find("return"))
assert(not find1("return"))
_G["ret" .. "urn"] = nil
assert(nofind==find("return"))
_G["xxx"] = 1
assert(xxx==find("xxx"))

-- invalid key to 'next'
checkerror("invalid key", next, {10,20}, 3)

-- both 'pairs' and 'ipairs' need an argument
checkerror("bad argument", pairs)
checkerror("bad argument", ipairs)

print('+')

a = {}
for i=0,10000 do
  if math.fmod(i,10) ~= 0 then
    a['x'..i] = i
  end
end

n = {n=0}
for i,v in pairs(a) do
  n.n = n.n+1
  assert(i and v and a[i] == v)
end
assert(n.n == 9000)
a = nil

do   -- clear global table
  local a = {}
  for n,v in pairs(_G) do a[n]=v end
  for n,v in pairs(a) do
    if not package.loaded[n] and type(v) ~= "function" and
       not string.find(n, "^[%u_]") then
     _G[n] = nil
    end
    collectgarbage()
  end
end


-- 

local function checknext (a)
  local b = {}
  do local k,v = next(a); while k do b[k] = v; k,v = next(a,k) end end
  for k,v in pairs(b) do assert(a[k] == v) end
  for k,v in pairs(a) do assert(b[k] == v) end
end

checknext{1,x=1,y=2,z=3}
checknext{1,2,x=1,y=2,z=3}
checknext{1,2,3,x=1,y=2,z=3}
checknext{1,2,3,4,x=1,y=2,z=3}
checknext{1,2,3,4,5,x=1,y=2,z=3}

assert(#{} == 0)
assert(#{[-1] = 2} == 0)
assert(#{1,2,3,nil,nil} == 3)
for i=0,40 do
  local a = {}
  for j=1,i do a[j]=j end
  assert(#a == i)
end

-- 'maxn' is now deprecated, but it is easily defined in Lua
function table.maxn (t)
  local max = 0
  for k in pairs(t) do
    max = (type(k) == 'number') and math.max(max, k) or max
  end
  return max
end

assert(table.maxn{} == 0)
assert(table.maxn{["1000"] = true} == 0)
assert(table.maxn{["1000"] = true, [24.5] = 3} == 24.5)
assert(table.maxn{[1000] = true} == 1000)
assert(table.maxn{[10] = true, [100*math.pi] = print} == 100*math.pi)

table.maxn = nil

-- int overflow
a = {}
for i=0,50 do a[2^i] = true end
assert(a[#a])

print('+')


-- erasing values
local t = {[{1}] = 1, [{2}] = 2, [string.rep("x ", 4)] = 3,
           [100.3] = 4, [4] = 5}

local n = 0
for k, v in pairs( t ) do
  n = n+1
  assert(t[k] == v)
  t[k] = nil
  collectgarbage()
  assert(t[k] == nil)
end
assert(n == 5)


local function test (a)
  assert(not pcall(table.insert, a, 2, 20));
  table.insert(a, 10); table.insert(a, 2, 20);
  table.insert(a, 1, -1); table.insert(a, 40);
  table.insert(a, #a+1, 50)
  table.insert(a, 2, -2)
  assert(not pcall(table.insert, a, 0, 20));
  assert(not pcall(table.insert, a, #a + 2, 20));
  assert(table.remove(a,1) == -1)
  assert(table.remove(a,1) == -2)
  assert(table.remove(a,1) == 10)
  assert(table.remove(a,1) == 20)
  assert(table.remove(a,1) == 40)
  assert(table.remove(a,1) == 50)
  assert(table.remove(a,1) == nil)
  assert(table.remove(a) == nil)
  assert(table.remove(a, #a) == nil)
end

a = {n=0, [-7] = "ban"}
test(a)
assert(a.n == 0 and a[-7] == "ban")

a = {[-7] = "ban"};
test(a)
assert(a.n == nil and #a == 0 and a[-7] == "ban")

a = {[-1] = "ban"}
test(a)
assert(#a == 0 and table.remove(a) == nil and a[-1] == "ban")

a = {[0] = "ban"}
assert(#a == 0 and table.remove(a) == "ban" and a[0] == nil)

table.insert(a, 1, 10); table.insert(a, 1, 20); table.insert(a, 1, -1)
assert(table.remove(a) == 10)
assert(table.remove(a) == 20)
assert(table.remove(a) == -1)
assert(table.remove(a) == nil)

a = {'c', 'd'}
table.insert(a, 3, 'a')
table.insert(a, 'b')
assert(table.remove(a, 1) == 'c')
assert(table.remove(a, 1) == 'd')
assert(table.remove(a, 1) == 'a')
assert(table.remove(a, 1) == 'b')
assert(table.remove(a, 1) == nil)
assert(#a == 0 and a.n == nil)

a = {10,20,30,40}
assert(table.remove(a, #a + 1) == nil)
assert(not pcall(table.remove, a, 0))
assert(a[#a] == 40)
assert(table.remove(a, #a) == 40)
assert(a[#a] == 30)
assert(table.remove(a, 2) == 20)
assert(a[#a] == 30 and #a == 2)

do   -- testing table library with metamethods
  local function test (proxy, t)
    for i = 1, 10 do
      table.insert(proxy, 1, i)
    end
    assert(#proxy == 10 and #t == 10)
    for i = 1, 10 do
      assert(t[i] == 11 - i)
    end
    table.sort(proxy)
    for i = 1, 10 do
      assert(t[i] == i and proxy[i] == i)
    end
    assert(table.concat(proxy, ",") == "1,2,3,4,5,6,7,8,9,10")
    for i = 1, 8 do
      assert(table.remove(proxy, 1) == i)
    end
    assert(#proxy == 2 and #t == 2)
    local a, b, c = table.unpack(proxy)
    assert(a == 9 and b == 10 and c == nil)
  end

  -- all virtual
  local t = {}
  local proxy = setmetatable({}, {
    __len = function () return #t end,
    __index = t,
    __newindex = t,
  })
  test(proxy, t)

  -- only __newindex
  local count = 0
  t = setmetatable({}, {
    __newindex = function (t,k,v) count = count + 1; rawset(t,k,v) end})
  test(t, t)
  assert(count == 10)   -- after first 10, all other sets are not new

  -- no __newindex
  t = setmetatable({}, {
    __index = function (_,k) return k + 1 end,
    __len = function (_) return 5 end})
  assert(table.concat(t, ";") == "2;3;4;5;6")

end


if not T then
  (Message or print)
    ('\n >>> testC not active: skipping tests for table library on non-tables <<<\n')
else --[
  local debug = require'debug'
  local tab = {10, 20, 30}
  local mt = {}
  local u = T.newuserdata(0)
  checkerror("table expected", table.insert, u, 40)
  checkerror("table expected", table.remove, u)
  debug.setmetatable(u, mt)
  checkerror("table expected", table.insert, u, 40)
  checkerror("table expected", table.remove, u)
  mt.__index = tab
  checkerror("table expected", table.insert, u, 40)
  checkerror("table expected", table.remove, u)
  mt.__newindex = tab
  checkerror("table expected", table.insert, u, 40)
  checkerror("table expected", table.remove, u)
  mt.__len = function () return #tab end
  table.insert(u, 40)
  assert(#u == 4 and #tab == 4 and u[4] == 40 and tab[4] == 40)
  assert(table.remove(u) == 40)
  table.insert(u, 1, 50)
  assert(#u == 4 and #tab == 4 and u[4] == 30 and tab[1] == 50)

  mt.__newindex = nil
  mt.__len = nil
  local tab2 = {}
  local u2 = T.newuserdata(0) 
  debug.setmetatable(u2, {__newindex = function (_, k, v) tab2[k] = v end})
  table.move(u, 1, 4, 1, u2)
  assert(#tab2 == 4 and tab2[1] == tab[1] and tab2[4] == tab[4])

end -- ]

print('+')

a = {}
for i=1,1000 do
  a[i] = i; a[i-1] = nil
end
assert(next(a,nil) == 1000 and next(a,1000) == nil)

assert(next({}) == nil)
assert(next({}, nil) == nil)

for a,b in pairs{} do error"not here" end
for i=1,0 do error'not here' end
for i=0,1,-1 do error'not here' end
a = nil; for i=1,1 do assert(not a); a=1 end; assert(a)
a = nil; for i=1,1,-1 do assert(not a); a=1 end; assert(a)

do
  print("testing floats in numeric for")
  local a
  -- integer count
  a = 0; for i=1, 1, 1 do a=a+1 end; assert(a==1)
  a = 0; for i=10000, 1e4, -1 do a=a+1 end; assert(a==1)
  a = 0; for i=1, 0.99999, 1 do a=a+1 end; assert(a==0)
  a = 0; for i=9999, 1e4, -1 do a=a+1 end; assert(a==0)
  a = 0; for i=1, 0.99999, -1 do a=a+1 end; assert(a==1)

  -- float count
  a = 0; for i=0, 0.999999999, 0.1 do a=a+1 end; assert(a==10)
  a = 0; for i=1.0, 1, 1 do a=a+1 end; assert(a==1)
  a = 0; for i=-1.5, -1.5, 1 do a=a+1 end; assert(a==1)
  a = 0; for i=1e6, 1e6, -1 do a=a+1 end; assert(a==1)
  a = 0; for i=1.0, 0.99999, 1 do a=a+1 end; assert(a==0)
  a = 0; for i=99999, 1e5, -1.0 do a=a+1 end; assert(a==0)
  a = 0; for i=1.0, 0.99999, -1 do a=a+1 end; assert(a==1)
end

-- conversion
a = 0; for i="10","1","-2" do a=a+1 end; assert(a==5)

do  -- checking types
  local c
  local function checkfloat (i)
    assert(math.type(i) == "float")
    c = c + 1
  end

  c = 0; for i = 1.0, 10 do checkfloat(i) end
  assert(c == 10)

  c = 0; for i = -1, -10, -1.0 do checkfloat(i) end
  assert(c == 10)

  local function checkint (i)
    assert(math.type(i) == "integer")
    c = c + 1
  end

  local m = math.maxinteger
  c = 0; for i = m, m - 10, -1 do checkint(i) end
  assert(c == 11)

  c = 0; for i = 1, 10.9 do checkint(i) end
  assert(c == 10)

  c = 0; for i = 10, 0.001, -1 do checkint(i) end
  assert(c == 10)

  c = 0; for i = 1, "10.8" do checkint(i) end
  assert(c == 10)

  c = 0; for i = 9, "3.4", -1 do checkint(i) end
  assert(c == 6)

  c = 0; for i = 0, " -3.4  ", -1 do checkint(i) end
  assert(c == 4)

  c = 0; for i = 100, "96.3", -2 do checkint(i) end
  assert(c == 2)

  c = 0; for i = 1, math.huge do if i > 10 then break end; checkint(i) end
  assert(c == 10)

  c = 0; for i = -1, -math.huge, -1 do
           if i < -10 then break end; checkint(i)
          end
  assert(c == 10)


  for i = math.mininteger, -10e100 do assert(false) end
  for i = math.maxinteger, 10e100, -1 do assert(false) end

end

collectgarbage()


-- testing generic 'for'

local function f (n, p)
  local t = {}; for i=1,p do t[i] = i*10 end
  return function (_,n)
           if n > 0 then
             n = n-1
             return n, table.unpack(t)
           end
         end, nil, n
end

local x = 0
for n,a,b,c,d in f(5,3) do
  x = x+1
  assert(a == 10 and b == 20 and c == 30 and d == nil)
end
assert(x == 5)



-- testing __pairs and __ipairs metamethod
a = {}
do
  local x,y,z = pairs(a)
  assert(type(x) == 'function' and y == a and z == nil)
end

local function foo (e,i)
  assert(e == a)
  if i <= 10 then return i+1, i+2 end
end

local function foo1 (e,i)
  i = i + 1
  assert(e == a)
  if i <= e.n then return i,a[i] end
end

setmetatable(a, {__pairs = function (x) return foo, x, 0 end})

local i = 0
for k,v in pairs(a) do
  i = i + 1
  assert(k == i and v == k+1)
end

a.n = 5
a[3] = 30

-- testing ipairs with metamethods
a = {n=10}
setmetatable(a, { __index = function (t,k)
                     if k <= t.n then return k * 10 end 
                  end})
i = 0
for k,v in ipairs(a) do
  i = i + 1
  assert(k == i and v == i * 10)
end
assert(i == a.n)

print"OK"
