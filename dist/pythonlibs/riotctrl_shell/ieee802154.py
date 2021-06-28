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

    def ieee802154_print_info(self, timeout=None, async_=False):
        cmd = "get_info" \
              .format()

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

        address_regex = re.compile(r"Address: (.*)$")
        channel_regex = re.compile(r"Channel: (.*)$")
        for line in res.splitlines():
            address_group = re.match(address_regex, line)
            channel_group = re.match(channel_regex, line)
            if (address_group):
                addr = address_group.groups()[0]
            elif (channel_group):
                channel = channel_group.groups()[0]
                break

        return addr, channel

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
    
    def ieee802154_reply_mode_cmd(self, reply_mode, timeout=None, async_=False):
        cmd = "reply {reply_mode} " \
                .format(reply_mode=reply_mode)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

        if "Success" not in res:
            raise RuntimeError(res)

    def ieee802154_txtsnd(self, long_addr, length, timeout=None, async_=False):
        cmd = "txtsnd {long_addr} {length} " \
              .format(long_addr=long_addr, length=length)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print("Error:", e)

    def ieee802154_txtspam(self, long_addr, length, number_of_packets, time_betweeen_packets, async_=False):
        timeout = (number_of_packets * time_betweeen_packets) / 1000 + 1
        cmd = "txtspam {long_addr} {length} {number_of_packets} {time_betweeen_packets} " \
              .format(long_addr=long_addr, length=length, number_of_packets=number_of_packets,
              time_betweeen_packets=time_betweeen_packets)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
            percentages = []
            number_ack_packets = None
            percentage_ack = None
            for line in res.splitlines():
                regex_summary = re.match("-------Summary of the test-------", line)
                regex_send_packets = re.match("Send Packets: (.*)$", line)
                regex_ack_packets = re.match("Acknowledged Packets: (.*)$", line)
                regex_received_packets = re.match("Received Packets: (.*)$", line)
                regex_percentage = re.match(".*Percentage: ([\d]+)\%$", line)
                if (regex_summary):
                    continue
                elif (regex_send_packets):
                    number_send_packets = int(regex_send_packets.groups()[0])
                elif (regex_ack_packets):
                    number_ack_packets = int(regex_ack_packets.groups()[0])
                elif (regex_percentage):
                    percentages.append(int(regex_percentage.groups()[0]))
                elif (regex_received_packets):
                    number_received_packets = int(regex_received_packets.groups()[0])

        except Exception as e:
            print("Error:", e)

        if len(percentages) == 2:
            percentage_ack = percentages[0]
            percentage_received = percentages[1]
        else:
            percentage_received = percentages[0]

        return {"tx_packets": number_send_packets, "ack_packets": number_ack_packets, "percentage_ack": percentage_ack,
        "num_rx": number_received_packets, "percentage_rx": percentage_received}
