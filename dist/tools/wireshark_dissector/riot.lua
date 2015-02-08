-- RIOT native support for Wireshark
-- A Lua implementation for dissection of RIOT native packets in wireshark
-- @Version: 0.0.1
-- @Author: Martine Lenders
-- @E-Mail: mlenders@inf.fu-berlin.de

do
    --Protocol name "RIOT"
    local p_riot = Proto("RIOT", "RIOT native packet")

    --Protocol Fields
    local f_length = ProtoField.uint16("RIOT.length", "Length", base.DEC, nil)
    local f_dst = ProtoField.uint16("RIOT.dst", "Destination", base.DEC, nil)
    local f_src = ProtoField.uint16("RIOT.src", "Source", base.DEC, nil)
    local f_pad = ProtoField.bytes("RIOT.pad", "Padding")

    p_riot.fields = { f_length, f_dst, f_src }

    local data_dis = Dissector.get("data")
    -- local next_dis = Dissector.get("6lowpan")   -- for 6LoWPAN
    local next_dis = Dissector.get("wpan")      -- for IEEE 802.15.4

    function riot_dissector(buf, pkt, root)
        local buf_len = buf:len()
        local riot_tree = root:add(p_riot, buf)

        if buf_len < 6 then return false end

        local packet_len = buf(0,2):uint()
        local dst = buf(2,2):uint()
        local src = buf(4,2):uint()

        if packet_len >= 46 and buf_len - 6 ~= packet_len then return false end

        riot_tree:append_text(", Dst: ")
        riot_tree:append_text(dst)
        riot_tree:append_text(", Src: ")
        riot_tree:append_text(src)
        riot_tree:append_text(", Length: ")
        riot_tree:append_text(packet_len)

        riot_tree:add(f_length, buf(0, 2))
        riot_tree:add(f_dst, buf(2, 2))
        riot_tree:add(f_src, buf(4, 2))

        -- to show the padding for small packets uncomment the
        -- following line and append "f_pad" to p_riot.fields above.
        -- riot_tree:add(f_pad, buf(packet_len + 6))

        next_dis:call(buf(6, packet_len):tvb(), pkt, root)

        return true
    end

    function p_riot.dissector(buf, pkt, root)
        if not riot_dissector(buf, pkt, root) then
            data_dis:call(buf, pkt, root)
        end
    end

    local eth_encap_table = DissectorTable.get("ethertype")

    --handle ethernet type 0x1234

    eth_encap_table:add(0x1234, p_riot)
end
