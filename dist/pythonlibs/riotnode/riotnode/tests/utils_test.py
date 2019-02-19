"""rionode.utils test module."""

import time
import multiprocessing
import logging

import psutil
import pytest

import riotnode.utils


def _process_spawn(num, queue):
    """Function for a process which spawns `num` number of processes and reports
       their PIDs back.
    """
    procs = []
    procs_pids = []

    for _ in range(num):
        proc = multiprocessing.Process(target=_process_wait)
        proc.start()
        procs.append(proc)
        procs_pids.append(proc.pid)

    queue.put(procs_pids)

    # wait for all
    for proc in procs:
        proc.join()


def _process_wait():
    """Function for a dummy process which does nothing."""
    while True:
        time.sleep(1)


def test_ensuring_all_subproc_are_stopped():
    """Tests that all subprocesses stopped."""
    queue = multiprocessing.Queue()
    spawner = multiprocessing.Process(target=_process_spawn, args=(2, queue))
    spawner.start()
    procs_pids = queue.get()

    procs = []
    for pid in procs_pids:
        procs.append(psutil.Process(pid))

    # should raise an exception as processes are not stopped
    with pytest.raises(RuntimeError):
        with riotnode.utils.ensure_all_subprocesses_stopped(spawner.pid,
                                                            logging):
            pass

    # now all processes should be stopped
    running = False
    for proc in procs:
        if proc.is_running():
            running = True
            proc.terminate()

    try:
        parent = psutil.Process(spawner.pid)
        if parent.is_running():
            running = True
            parent.terminate()
    except psutil.NoSuchProcess:
        pass

    assert not running


def test_getting_pid_subprocesses():
    """Tests getting all subprocesses PIDs."""
    queue = multiprocessing.Queue()
    spawner = multiprocessing.Process(target=_process_spawn, args=(2, queue))
    spawner.start()
    procs_pids = queue.get()

    subprocesses = riotnode.utils.pid_subprocesses(spawner.pid)
    subprocesses_pids = (s.pid for s in subprocesses)

    try:
        # returns all subprocesses and the parent
        assert len(subprocesses) == len(procs_pids) + 1
        assert spawner.pid in subprocesses_pids
        for pid in procs_pids:
            assert pid in subprocesses_pids

        assert riotnode.utils.pid_subprocesses(None) == []
    finally:
        for proc in subprocesses:
            proc.terminate()


def test_ensuring_all_procs_are_stopped():
    """Tests that all processes are stopped."""
    queue = multiprocessing.Queue()
    spawner = multiprocessing.Process(target=_process_spawn, args=(2, queue))
    spawner.start()
    procs_pids = queue.get()

    procs = []
    for pid in procs_pids:
        procs.append(psutil.Process(pid))

    # Should raise exception as processes are running
    with pytest.raises(RuntimeError):
        riotnode.utils.ensure_processes_stopped(procs, logging)

    # All processes should be stopped
    running = False
    for proc in procs:
        if proc.is_running():
            running = True
            proc.terminate()

    spawner.terminate()
    assert not running

    # Now it should not raise exceptions
    riotnode.utils.ensure_processes_stopped(procs, logging)
