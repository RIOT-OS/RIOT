import re

from riotctrl.shell import ShellInteraction

# ==== ShellInteractions ====

class IEEE802154Phy(ShellInteraction):
    def ieee802154_config_phy(self, phy_mode, channel, tx_pow, timeout=None, async_=False):
        cmd = "config_phy {phy_mode} {channel} {tx_pow}" \
              .format(phy_mode=phy_mode, channel=channel, tx_pow=tx_pow)
        
        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)
        
        if "Success" not in res:
            raise RuntimeError(res)

        if str(channel) not in res:
            raise RuntimeError(res)

    def ieee802154_get_phy(self, timeout=None, async_=False):
        cmd = "get_phy" \
              .format()

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

        count = 0
        for line in res.splitlines():
            count+=1
            if count > 2:
                break
            pass
        addr = line
        return addr

    def ieee802154_tx_mode(self, command, timeout=None, async_=False):
        cmd = "tx_mode {command}" \
                .format(command=command)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

        if "Ok" not in res:
            raise RuntimeError(res)

    def ieee802154_check_last_packet(self, long_addr, channel, timeout=None, async_=False):
        cmd = "check_last_packet {long_addr} {channel}" \
                .format(long_addr=long_addr, channel=channel)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

        if "Success" not in res:
            raise RuntimeError(res)
    
    def ieee802154_reply(self, reply_mode, timeout=None, async_=False):
        cmd = "reply {reply_mode} " \
                .format(reply_mode=reply_mode)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

        if "Success" not in res:
            raise RuntimeError(res)

    def ieee802154_txtsnd(self, long_addr, len, request_ack, timeout=None, async_=False):
        cmd = "txtsnd {long_addr} {len} {request_ack} " \
              .format(long_addr=long_addr, len=len, request_ack=request_ack)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

    def ieee802154_txtspam(self, long_addr, len, number_of_packets, time_betweeen_packets, request_ack, timeout=None, async_=False):
        cmd = "txtspam {long_addr} {len} {number_of_packets} {time_betweeen_packets} {request_ack} " \
              .format(long_addr=long_addr, len=len, number_of_packets=number_of_packets,
              time_betweeen_packets=time_betweeen_packets, request_ack=request_ack)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)
