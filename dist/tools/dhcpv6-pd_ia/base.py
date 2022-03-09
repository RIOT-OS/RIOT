# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


import atexit
import os
import shutil
import subprocess
import time
import threading
import signal
import sys

import pkg


__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"


# see https://refactoring.guru/design-patterns/singleton/python/example
class _SingletonMeta(type):
    """
    This is a thread-safe implementation of Singleton.
    """

    _instance = None

    _lock = threading.Lock()
    """
    We now have a lock object that will be used to synchronize threads
    during first access to the Singleton.
    """

    def __call__(cls, *args, **kwargs):
        # Now, imagine that the program has just been launched. Since
        # there's no Singleton instance yet, multiple threads can
        # simultaneously pass the previous conditional and reach this point
        # almost at the same time. The first of them will acquire lock and
        # will proceed further, while the rest will wait here.
        with cls._lock:
            # The first thread to acquire the lock, reaches this
            # conditional, goes inside and creates the Singleton instance.
            # Once it leaves the lock block, a thread that might have been
            # waiting for the lock release may then enter this section. But
            # since the Singleton field is already initialized, the thread
            # won't create a new object.
            if not cls._instance:
                cls._instance = super().__call__(*args, **kwargs)
        return cls._instance


class DHCPv6Server(metaclass=_SingletonMeta):
    """
    Inspired by Daemon class
    https://web.archive.org/web/20160305151936/\
    http://www.jejik.com/articles/2007/02/a_simple_unix_linux_daemon_in_python/
    """
    PORT = 547
    command = None
    package = None
    installer = None

    def __init__(self, daemonized=False, pidfile=None, stdin=None, stdout=None,
                 stderr=None):
        if self.command is None or self.package is None:
            raise NotImplementedError("Please inherit from {} and set the "
                                      "the static attributes `command` and "
                                      "`packet`".format(type(self)), file=sys)
        assert (not daemonized) or (pidfile is not None)
        self.daemonized = daemonized
        self.pidfile = pidfile
        self.stdin = stdin
        self.stdout = stdout
        self.stderr = stderr

    @classmethod
    def is_installed(cls):
        return shutil.which(cls.command[0]) is not None

    def install(self):
        self.installer = pkg.PackageManagerFactory.get_installer()
        self.installer.install(self.package)

    def daemonize(self):
        """
        do the UNIX double-fork magic, see Stevens' "Advanced
        Programming in the UNIX Environment" for details (ISBN 0201563177)
        http://www.erlenstar.demon.co.uk/unix/faq_2.html#SEC16
        """
        try:
            pid = os.fork()
            if pid > 0:
                # exit first parent
                sys.exit(0)
        except OSError as e:
            sys.stderr.write("fork #1 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1)

        # decouple from parent environment
        os.chdir("/")
        os.setsid()
        os.umask(0)

        # do second fork
        try:
            pid = os.fork()
            if pid > 0:
                # exit from second parent
                sys.exit(0)
        except OSError as e:
            sys.stderr.write("fork #2 failed: %d (%s)\n" %
                             (e.errno, e.strerror))
            sys.exit(1)

        # redirect standard file descriptors
        if self.stdin:
            si = open(self.stdin, 'r')
            os.dup2(si.fileno(), sys.stdin.fileno())
        if self.stdout:
            sys.stdout.flush()
            so = open(self.stdout, 'a+')
            os.dup2(so.fileno(), sys.stdout.fileno())
        if self.stderr:
            sys.stderr.flush()
            se = open(self.stderr, 'a+', 0)
            os.dup2(se.fileno(), sys.stderr.fileno())

        atexit.register(self.delpid)
        # write pidfile
        with open(self.pidfile, "w+") as f:
            f.write("{}\n".format(os.getpid()))

    def delpid(self):
        os.remove(self.pidfile)

    def stop(self):
        """
        Stop the daemon
        """
        # Get the pid from the pidfile
        try:
            pf = open(self.pidfile, 'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None
        if not pid:
            message = "pidfile %s does not exist. Daemon not running?\n"
            sys.stderr.write(message % self.pidfile)
            return  # not an error in a restart

        # Try killing the daemon process
        try:
            while 1:
                os.kill(pid, signal.SIGTERM)
                time.sleep(0.1)
        except OSError as err:
            err = str(err)
            if err.find("No such process") > 0:
                if os.path.exists(self.pidfile):
                    os.remove(self.pidfile)
            else:
                print(str(err), file=sys.stderr)
                sys.exit(1)

    def pre_run(self):
        # may be overridden by implementation to do things before running
        # the daemon or script
        pass

    def run(self):
        if not self.is_installed():
            self.install()
        if self.daemonized:
            """
            Start the daemon
            """
            # Check for a pidfile to see if the daemon already runs
            try:
                pf = open(self.pidfile, 'r')
                pid = int(pf.read().strip())
                pf.close()
            except (IOError, ValueError):
                pid = None
            if pid:
                message = "pidfile %s already exist. Daemon already running?\n"
                sys.stderr.write(message % self.pidfile)
                sys.exit(1)
            # Start the daemon
            self.daemonize()
        self.pre_run()
        subprocess.run(self.command)
