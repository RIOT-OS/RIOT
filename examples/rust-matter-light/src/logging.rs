use log::{Level, LevelFilter, Log, Record, SetLoggerError};
use riot_wrappers::println;

struct RiotLogger;

impl Log for RiotLogger {
    fn enabled(&self, metadata: &log::Metadata) -> bool {
        metadata.level() >= Level::Info
    }

    fn log(&self, record: &Record) {
        if self.enabled(record.metadata()) {
            println!("[{}] {}", record.level(), record.args());
        }
    }

    fn flush(&self) {}
}

static LOGGER: RiotLogger = RiotLogger;

pub fn init_logger(level: LevelFilter) -> Result<(), SetLoggerError> {
    //let log_level: &'static str = env!("RIOT_LOG_LEVEL");
    //println!("the RIOT_LOG_LEVEL variable at the time of compiling was: {log_level}");
    log::set_logger(&LOGGER)
        .map(|_| log::set_max_level(level))
}