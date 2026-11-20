use core::ops::{Deref, DerefMut};

/// A mutual exclusion primitive similar to std::sync::RwLock, implemented using RIOT's rwlock.
///
/// Like crate::mutex::Mutex, this knows no poisoning.
///
/// It's unknown whether it's actually a good idea to go this way (with all the
pub struct RwLock<T> {
    rwlock: riot_sys::pthread_rwlock_t,
    data: T,
}

unsafe impl<T: Send> Send for RwLock<T> {}
unsafe impl<T: Send + Sync> Sync for RwLock<T> {}

impl<T> RwLock<T> {
    pub fn new(t: T) -> RwLock<T> {
        // FIXME consider deriving default
        let mut rwlock = riot_sys::pthread_rwlock_t {
            mutex: riot_sys::mutex_t {
                queue: riot_sys::list_node { next: 0 as *mut _ },
            },
            queue: riot_sys::priority_queue_t { first: 0 as *mut _ },
            readers: 0,
        };
        // FIXME actually needless given it only memsets
        unsafe { riot_sys::pthread_rwlock_init(&mut rwlock, 0 as *mut _) };
        RwLock {
            rwlock: rwlock,
            data: t,
        }
    }

    pub fn read(&self) -> RwLockReadGuard<T> {
        match unsafe { riot_sys::pthread_rwlock_rdlock(&self.rwlock as *const _ as *mut _) } {
            0 => RwLockReadGuard { rwlock: self },
            _ => panic!("Unexpected error from pthread_rwlock_rdlock"),
        }
    }

    pub fn try_read(&self) -> Option<RwLockReadGuard<T>> {
        match unsafe { riot_sys::pthread_rwlock_tryrdlock(&self.rwlock as *const _ as *mut _) }
            as u32
        {
            0 => Some(RwLockReadGuard { rwlock: self }),
            riot_sys::EBUSY => None,
            _ => panic!("Unexpected error from pthread_rwlock_tryrdlock"),
        }
    }

    pub fn write(&self) -> RwLockWriteGuard<T> {
        match unsafe { riot_sys::pthread_rwlock_wrlock(&self.rwlock as *const _ as *mut _) } {
            0 => RwLockWriteGuard { rwlock: self },
            _ => panic!("Unexpected error from pthread_rwlock_wrlock"),
        }
    }

    pub fn try_write(&self) -> Option<RwLockWriteGuard<T>> {
        match unsafe { riot_sys::pthread_rwlock_trywrlock(&self.rwlock as *const _ as *mut _) }
            as u32
        {
            0 => Some(RwLockWriteGuard { rwlock: self }),
            riot_sys::EBUSY => None,
            _ => panic!("Unexpected error from pthread_rwlock_trywrlock"),
        }
    }
}

impl<T> Drop for RwLock<T> {
    fn drop(&mut self) {
        match unsafe { riot_sys::pthread_rwlock_destroy(&mut self.rwlock) } {
            0 => (),
            _ => panic!("pthread_rwlock_destroy was unsuccessful, even though RwLock has no other references to it.")
        }
    }
}

pub struct RwLockReadGuard<'a, T> {
    rwlock: &'a RwLock<T>,
}

impl<'a, T> Deref for RwLockReadGuard<'a, T> {
    type Target = T;

    fn deref(&self) -> &T {
        &self.rwlock.data
    }
}

impl<'a, T> Drop for RwLockReadGuard<'a, T> {
    fn drop(&mut self) {
        match unsafe { riot_sys::pthread_rwlock_unlock(&self.rwlock.rwlock as *const _ as *mut _) }
        {
            0 => (),
            _ => panic!("pthread_rwlock_unlock was unsuccessful."),
        }
    }
}

pub struct RwLockWriteGuard<'a, T> {
    rwlock: &'a RwLock<T>,
}

impl<'a, T> Deref for RwLockWriteGuard<'a, T> {
    type Target = T;

    fn deref(&self) -> &T {
        &self.rwlock.data
    }
}

impl<'a, T> DerefMut for RwLockWriteGuard<'a, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *(&self.rwlock.data as *const _ as *mut _) }
    }
}

impl<'a, T> Drop for RwLockWriteGuard<'a, T> {
    fn drop(&mut self) {
        match unsafe { riot_sys::pthread_rwlock_unlock(&self.rwlock.rwlock as *const _ as *mut _) }
        {
            0 => (),
            _ => panic!("pthread_rwlock_unlock was unsuccessful."),
        }
    }
}
