# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
`congure_test`-related shell interactions

Defines `congure_test`-related shell command interactions
"""

from riotctrl.shell import ShellInteraction


class CongureTest(ShellInteraction):
    @ShellInteraction.check_term
    def setup(self, ident=0, timeout=-1, async_=False):
        return self.cmd('cong_setup {ident}'.format(ident=ident),
                        timeout=timeout, async_=async_)

    @ShellInteraction.check_term
    def clear(self, timeout=-1, async_=False):
        return self.cmd('cong_clear', timeout=timeout, async_=async_)

    @ShellInteraction.check_term
    def init(self, ctx, timeout=-1, async_=False):
        return self.cmd('cong_init 0x{ctx:x}'.format(ctx=ctx),
                        timeout=timeout, async_=async_)

    @ShellInteraction.check_term
    def inter_msg_interval(self, msg_size, timeout=-1, async_=False):
        return self.cmd('cong_imi {msg_size}'.format(msg_size=msg_size),
                        timeout=timeout, async_=async_)

    @ShellInteraction.check_term
    def add_msg(self, send_time, size, resends, timeout=-1, async_=False):
        return self.cmd(
            'cong_add_msg {send_time} {size} {resends}'
            .format(send_time=send_time, size=size, resends=resends)
        )

    @ShellInteraction.check_term
    def msgs_reset(self, timeout=-1, async_=False):
        return self.cmd('cong_msgs_reset')

    @ShellInteraction.check_term
    def report(self, cmd, *args, timeout=-1, async_=False):
        args = ' '.join(str(a) for a in args)
        return self.cmd('cong_report {cmd} {args}'.format(cmd=cmd, args=args))

    def report_msg_sent(self, msg_size, timeout=-1, async_=False):
        return self.report('msg_sent', msg_size,
                           timeout=timeout, async_=async_)

    def report_msg_discarded(self, msg_size, timeout=-1, async_=False):
        return self.report('msg_discarded', msg_size,
                           timeout=timeout, async_=async_)

    def _report_msgs_timeout_lost_base(self, cmd, timeout=-1, async_=False):
        return self.report(cmd, timeout=timeout, async_=async_)

    def _report_msgs_timeout_lost(self, cmd, msgs, timeout=-1, async_=False):
        tmp = None
        for msg in msgs:
            tmp = self.add_msg(**msg)
            assert 'success' in tmp
        res = self._report_msgs_timeout_lost_base(
            cmd, timeout=timeout, async_=async_
        )
        return res

    def report_msgs_timeout_base(self, timeout=-1, async_=False):
        return self._report_msgs_timeout_lost_base(
            'msgs_timeout', timeout=timeout, async_=async_
        )

    def report_msgs_timeout(self, msgs, timeout=-1, async_=False):
        return self._report_msgs_timeout_lost(
            'msgs_timeout', msgs, timeout=timeout, async_=async_
        )

    def report_msgs_lost_base(self, timeout=-1, async_=False):
        return self._report_msgs_timeout_lost_base(
            'msgs_lost', timeout=timeout, async_=async_
        )

    def report_msgs_lost(self, msgs, timeout=-1, async_=False):
        return self._report_msgs_timeout_lost(
            'msgs_lost', msgs, timeout=timeout, async_=async_
        )

    def report_msg_acked_base(self, ack_recv_time, ack_id, ack_size, ack_clean,
                              ack_wnd, ack_delay, timeout=-1, async_=False):
        if isinstance(ack_clean, bool):
            ack_clean = int(ack_clean)
        return self.report('msg_acked', ack_recv_time, ack_id, ack_size,
                           ack_clean, ack_wnd, ack_delay,
                           timeout=-1, async_=False)

    def report_msg_acked(self, msg, ack, timeout=-1, async_=False):
        tmp = self.add_msg(**msg)
        assert 'success' in tmp
        res = self.report_msg_acked_base(
            **{'ack_{}'.format(k): v for k, v in ack.items()},
            timeout=timeout, async_=async_
        )
        return res

    def report_ecn_ce(self, time, timeout=-1, async_=False):
        return self.report('ecn_ce', time, timeout=timeout, async_=async_)
