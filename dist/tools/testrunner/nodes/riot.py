import os, signal, subprocess, sys, time
from pexpect import spawnu, TIMEOUT

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
from .base import BaseNode
import nodes.map

from util import interrupt_main

class RiotNode(BaseNode):
    def __init__(s, _dict):
        super().__init__(_dict)
        s.application = _dict.get("app", None)

    def _make(s, *args):
        if s.application:
            appdir = [ '-C', os.path.join(os.environ.get("RIOTBASE"), s.application)]
        else:
            appdir = []
        return ["make"] + appdir + list(args)

    def spawn(s):
        print(" ".join(s._make("term")))
        s.child = spawnu(" ".join(s._make("term")), env=s.env, timeout=s.timeout, codec_errors='ignore')
        s.child.logfile_read = sys.stdout

    def drain(s):
        time.sleep(2)
        try:
            s.log("drain")
            res = s.child.read_nonblocking(1000, timeout=5)
            s.log("drained:", res)
        except TIMEOUT:
            pass

    def flash(s):
        s.log("flash")

        try:
            output = subprocess.check_output(s._make('-B', 'flash'), env=s.env, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            print(e)
            print(e.output)
            interrupt_main()
#            if e.returncode in { -2, -3, -9, -15}:
#                print("child killed")
#                raise KeyboardInterrupt()
#            print(e)

    def reset(s):
        s.log("reset")

#        s.drain()

        try:
            subprocess.check_output(s._make('reset'), env=s.env,
                                    stderr=subprocess.PIPE)
        except subprocess.CalledProcessError:
            # make reset yields error on some boards even if successful
            pass

    def launch(s):
        s.flash()
        s.spawn()
        s.reset()
        return s

    def close(s):
        s.log("close")
        if s.child:
            os.killpg(os.getpgid(s.child.pid), signal.SIGKILL)
            s.child.close()

nodes.map.register("riot", RiotNode)
