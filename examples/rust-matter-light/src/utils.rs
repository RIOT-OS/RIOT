use core::time::Duration;
use log::info;
use rs_matter::utils::epoch::MATTER_EPOCH_SECS;

extern "C" {
    fn get_seconds_since_matter_epoch() -> u64;
}

pub fn get_timestamp() -> u64 {
    let rtc_seconds = unsafe { get_seconds_since_matter_epoch() };
    rtc_seconds + MATTER_EPOCH_SECS
}

pub fn get_epoch() -> Duration {
    Duration::from_secs(get_timestamp())
}