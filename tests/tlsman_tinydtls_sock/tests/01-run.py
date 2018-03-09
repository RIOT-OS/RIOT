#!/usr/bin/env python3

# Copyright (C) 2018 Raul Fuentes <raul.fuentes-samaniego@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact(u"Calling test_load_stack()")
    child.expect_exact(u"Calling test_creating_context(&local, &remote, &udp_sock, &dtls_session)")
    child.expect_exact(u"Calling test_creating_channel(&dtls_session, tlsman_flags, packet_rcvd)")
    child.expect_exact(u"Calling test_checking_errors()")
    child.expect_exact(u"Calling test_sending_uper_data(&dtls_session)")
    child.expect_exact(u"Calling test_retrieve_data(&dtls_session, packet_rcvd, DTLS_MAX_BUF)")
    child.expect_exact(u"Calling test_release_resources_IGNORING_ALERT(&dtls_session, &udp_sock, 0, NULL)")
    child.expect_exact(u"Calling test_reconnect_COMMON_ERROR(&local, &remote, &udp_sock, &dtls_session, tlsman_flags, packet_rcvd, sizeof(packet_rcvd))")
    child.expect_exact(u"Calling test_sending_uper_data_MULTIPLE(&dtls_session, packet_rcvd, DTLS_MAX_BUF)")
    child.expect_exact(u"Calling test_reconnect_CLOSING_SOCKETS(&local, &remote, &udp_sock, &dtls_session, tlsman_flags, packet_rcvd, sizeof(packet_rcvd))")
    child.expect_exact(u"Calling test_retrieve_data_TIMEOUT(&dtls_session, packet_rcvd, DTLS_MAX_BUF)")
    child.expect_exact(u"Calling test_closing_channel(&dtls_session, packet_rcvd, DTLS_MAX_BUF)")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")

if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    timer = 10 # NOTE: DTLS timeout is around 10 seconds
    sys.exit(run(testfunc, timeout=timer))
