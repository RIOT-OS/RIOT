"""Some utilities functions"""

import contextlib

import psutil


@contextlib.contextmanager
def ensure_all_subprocesses_stopped(pid, logger, timeout=3):
    """Ensure all subprocesses for 'pid' are correctly stopped.

    context manager.
    """
    try:
        processes = pid_subprocesses(pid)
        yield
    finally:
        ensure_processes_stopped(processes, logger, timeout=timeout)


def pid_subprocesses(pid):
    """Return subprocesses for pid including itself.

    If pid is None return nothing.
    """
    if pid is None:
        return []

    try:
        proc = psutil.Process(pid)
        processes = [proc]
        processes.extend(proc.children(recursive=True))
        return processes
    except psutil.NoSuchProcess:
        return []


def ensure_processes_stopped(processes, logger, timeout=3):
    """Ensure processes are correctly stopped and kill them if not.

    Raise a RuntimeError if it is not the case.
    """
    # Use a list to call all of them
    stopped_processes = [_ensure_process_stopped(p, logger, timeout)
                         for p in processes]
    if not all(stopped_processes):
        raise RuntimeError('Some term process where not stopped')


def _ensure_process_stopped(proc, logger, timeout=3):
    """Ensure the given process has been stopped.

    If it has not, do a critical logging message and kill it.
    :type proc: psutil.Process
    :return: True if process was stopped
    """
    if not proc.is_running():
        return True
    logger.critical('process %u:%s was not stopped', proc.pid, proc.status())

    proc.kill()
    try:
        proc.wait(timeout)
    except psutil.TimeoutExpired:  # pragma: no cover
        # No sure how to cover this…
        logger.critical(
            'process %u:%s is not killable', proc.pid, proc.status())

    return False
