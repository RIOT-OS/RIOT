use core::time::Duration;
use riot_wrappers::random::Random;
use rand_core::RngCore as _;

extern "C" {
    fn get_seconds_since_unix_epoch() -> u32;
}

pub fn sys_rand(buf: &mut [u8]) {
    Random::new().fill_bytes(buf);
}

pub fn sys_epoch() -> Duration {
    let rtc_seconds = unsafe { get_seconds_since_unix_epoch() };
    Duration::from_secs(rtc_seconds as u64)
}
