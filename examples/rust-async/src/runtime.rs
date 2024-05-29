use crate::executor::Executor;
use crate::util::get_static;
use riot_wrappers::println;

#[embassy_executor::task]
async fn task_main() {
    println!("task_main(): starting");
    crate::async_main().await;
}

#[embassy_executor::task]
async fn task_server() {
    println!("task_server(): starting");
    crate::server::start().await;
}

#[embassy_executor::task]
async fn task_shell() {
    println!("task_shell(): starting");
    crate::shell::process_shell_stream().await.unwrap();
}

const RUNTIME_THROTTLE_MS: u32 = 100;

pub struct Runtime(Executor);

impl Runtime {
    pub fn new() -> Self {
        Self(Executor::new(Some(RUNTIME_THROTTLE_MS)))
    }

    pub fn run(&mut self) -> ! {
        get_static(self).0.run(|spawner| {
            spawner.spawn(task_shell()).unwrap();
            spawner.spawn(task_main()).unwrap();
            spawner.spawn(task_server()).unwrap();
        });
    }
}