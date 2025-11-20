--[[
    SPDX-FileCopyrightText: 2018 Freie Universität Berlin
    SPDX-License-Identifier: LGPL-2.1-only
]]
--[[
   @file repl.lua
   @brief   Read-eval-print loop for LUA
   @author  Juan Carrano <j.carrano@fu-berlin.de>
]]

local _R_EVAL = 0
local _R_CONT = 1
local _R_EXIT = 2
local _R_ERROR = 3

--[[ Read code from standard input (whatever stdin means for lua)
    @return action_code     what the eval loop should do
    @return code_or_msg     either code (_R_EVAL) or a message (_R_ERROR) or nil
                            (_R_CONT, _R_EXIT).
]]

local function re()
    io.write("L> ")
    io.flush()
    local ln = io.read()

    if not ln then
        return _R_EXIT
    elseif ln == "\n" then
        return _R_CONT
    end
    -- Try to see if we have an expression
    local maybe_code, compile_err = load("return "..ln)
    -- Try to see if we have a single-line statement
    if not maybe_code then
        maybe_code, compile_err = load(ln)
    end
    -- Try a multiline statement
    if not maybe_code then
        -- It's not really necessary to use a coroutine, but it shows that they
        -- work.
        local _get_multiline = coroutine.create(
            function ()
                coroutine.yield(ln.."\n")   -- We already have the first line of input
                while 1 do
                    io.write("L.. ")
                    io.flush()
                    local l = io.read()
                    if #l ~= 0 then
                        l = l.."\n"
                    end
                    coroutine.yield(l)
                end
            end
            )
        local get_multiline = function()
                local a, b = coroutine.resume(_get_multiline)
                if a then
                    return b
                else
                    return nil
                end
            end

        maybe_code, compile_err = load(get_multiline)
    end

    if not maybe_code then
        return _R_ERROR, compile_err
    else
        return _R_EVAL, maybe_code
    end
end

local function repl()
    io.write("Welcome to the interactive interpreter\n");

    while 1 do
        local action, fn_or_message = re()

        if action == _R_EVAL then
            local success, msg_or_ret = pcall(fn_or_message)
            if not success then
                print("Runtime error", msg_or_ret)
            elseif msg_or_ret ~= nil then
                print(msg_or_ret)
            end
        elseif action == _R_EXIT then
            print()
            return
        elseif action == _R_ERROR then
            print("Compile error:", fn_or_message)
        end -- (action == _R_CONT) : do nothing
    end
end

repl()
