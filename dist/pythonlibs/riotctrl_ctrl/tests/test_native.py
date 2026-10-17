# SPDX-FileCopyrightText: 2021 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import os.path
import riotctrl.ctrl

import riotctrl_ctrl.native


def test_reset():
    env = {'DEBUG_ADAPTER_ID': '', 'BOARD': 'native'}
    ctrl = riotctrl_ctrl.native.NativeRIOTCtrl(
        application_directory=os.path.normpath(os.path.join(
            os.path.abspath(__file__),
            '..', '..', '..', '..', '..',
            'examples', 'basic', 'hello-world'
        )),
        env=env,
    )
    print(ctrl.application_directory)
    assert not ctrl.env['DEBUG_ADAPTER_ID']
    ctrl.make_run(['flash'])
    with ctrl.run_term(reset=False):
        # DEBUG_ADAPTER_ID is now a PID
        assert int(ctrl.env['DEBUG_ADAPTER_ID'])
        ctrl.term.expect_exact('Hello World!')
        ctrl.reset()
        ctrl.term.expect_exact('!! REBOOT !!')
        ctrl.term.expect_exact('Hello World!')


def test_w_factory():
    env = {'BOARD': 'native'}
    factory = riotctrl.ctrl.RIOTCtrlBoardFactory(
        board_cls={'native': riotctrl_ctrl.native.NativeRIOTCtrl}
    )
    assert 'native' in factory.board_cls
    ctrl = factory.get_ctrl(env=env)
    # pylint: disable=unidiomatic-typecheck
    # in this case we want to know the exact type
    assert type(ctrl) is riotctrl_ctrl.native.NativeRIOTCtrl
