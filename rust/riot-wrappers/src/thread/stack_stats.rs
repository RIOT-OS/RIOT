/// Gathered information about a thread, returned by [super::KernelPID::stack_stats()].
///
/// All accessors are unconditional, because the StackStats can't be obtained without develhelp in
/// the first place.
#[derive(Debug)]
#[non_exhaustive]
pub struct StackStats {
    pub(crate) start: *mut i8,
    pub(crate) size: usize,
    pub(crate) free: usize,
}

impl StackStats {
    pub fn start(&self) -> *mut i8 {
        self.start
    }

    pub fn size(&self) -> usize {
        self.size
    }

    pub fn end(&self) -> *mut i8 {
        // This is the last legal pointer to construct on this ... last-plus-one rule.
        unsafe { self.start.offset(self.size as isize) }
    }

    pub fn free(&self) -> usize {
        self.free
    }

    pub fn used(&self) -> usize {
        self.size - self.free
    }
}

#[non_exhaustive]
#[derive(Debug, Copy, Clone)]
pub enum StackStatsError {
    /// Requested PID does not correspond to a thread
    NoSuchThread,
    /// Details on the stack are unavailable because develhelp is disabled
    InformationUnavailable,
}

impl From<super::NoSuchThread> for StackStatsError {
    fn from(_: super::NoSuchThread) -> Self {
        StackStatsError::NoSuchThread
    }
}
