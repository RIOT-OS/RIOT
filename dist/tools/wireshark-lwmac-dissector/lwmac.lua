-- Author: Daniel Krebs <github@daniel-krebs.net>
-- Date: 2016-01-18
--

local lwmac = Proto("lwmac", "LwMAC dissector")

local FRAME_WR          = 0x01
local FRAME_WA          = 0x02
local FRAME_DATA        = 0x03
local FRAME_BROADCAST   = 0x04

local frame_str = {
    [FRAME_WR]          = "Wakeup Request (WR)",
    [FRAME_WA]          = "Wakeup Acknowledge (WA)",
    [FRAME_DATA]        = "Unicast data (DATA)",
    [FRAME_BROADCAST]   = "Broadcast data (BROADCAST)"
}

function dissector(tvb, pinfo, tree)
    
    if tvb:len() < 1 then
        print("Packet too short (0 Byte)\n")
        return
    end

    -- Index in buffer for next field to parse
    index = 0

    -- Length of next field to parse
    count = 0

    
    subtree = tree:add(lwmac, tvb(), "LwMAC protocol")

    -- Parse header (currently only frame type)
    count = 1
    frame_type = tvb(index, count):uint()

    subtree:add(tvb(index, count), "Type: " .. frame_str[frame_type])
    index = index + count

    -- Display frame type in 'info' field    
    pinfo.cols.info = frame_str[frame_type]


    if frame_type == FRAME_WR then 
        -- This is WR, nothing to do
    elseif frame_type == FRAME_WA then
        -- Parse L2 address
        count = 8
        l2_addr = tvb(index, count):bytes()

        -- assemble L2 address string
        out = ""
        for i = 0, l2_addr:len()-1 do
            out = out .. tostring(l2_addr:subset(i, 1))
            if i < (l2_addr:len()-1) then
                out = out .. ":"
            end
        end

        subtree:add(tvb(index, count), "Destination: " .. out)        
        index = index + count
    elseif frame_type == FRAME_DATA then
        -- This is DATA, nothing to do
    elseif frame_type == FRAME_BROADCAST then
        -- Parse sequence number                
        count = 1
        seq = tvb(index, count)

        subtree:add(seq, string.format("Sequence number: %d", seq:uint()))
        index = index + count
    else
        print("Unsupported frame type: " .. frame_type)
    end
    
    subtree:set_len(index)
    pinfo.cols.protocol = "LwMAC"

    if frame_type == FRAME_DATA or frame_type == FRAME_BROADCAST then
        -- Call dissectors for nested payload
        local dis = Dissector.get("6lowpan")
        dis(tvb(index):tvb(), pinfo, tree)
    end
    
    return index
end
lwmac.dissector = dissector

-- Register as a heuristic dissector, that gets called for all wpan
-- packets. We'd want to pass foo.dissector here, but it turns out
-- register_heuristic needs an actual function. Passing a lambda
-- doesn't work (since calling foo.dissector(...) discards the
-- return value), so instead we define the dissector function in two
-- steps above, so we can directly access the real function here.
-- See also https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=10695
lwmac:register_heuristic("wpan", dissector)

-- Register as the dissector for panid 3. Will be automatically
-- called for packets with panid 3 (picking a panid is mandatory,
-- see https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=10696).
-- Can additionally be manually selected using the "Decode as..."
-- option.
table = DissectorTable.get("wpan.panid")
table:add(26, lwmac)
