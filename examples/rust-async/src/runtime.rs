//use riot_wrappers::println;
use crate::executor::Executor;
use crate::util::get_static;

#[embassy_executor::task]
async fn task_main() {
    crate::async_main().await;
}

#[embassy_executor::task]
async fn task_shell_stream() {
    crate::shell::process_shell_stream().await.unwrap();
}

#[embassy_executor::task]
async fn task_callback_stream() {
    //crate::callback::process_callback_stream().await.unwrap();
}

/*
#[embassy_executor::task]
async fn task_gcoap_server_stream() {
    xbd::process_gcoap_server_stream().await.unwrap();
}
*/

const RUNTIME_THROTTLE_MS: u32 = 100;

pub struct Runtime(Executor);

impl Runtime {
    pub fn new() -> Self {
        Self(Executor::new(Some(RUNTIME_THROTTLE_MS)))
    }

    pub fn run(&mut self) -> ! {
        get_static(self).0.run(|spawner| {
            spawner.spawn(task_main()).unwrap();
            spawner.spawn(task_shell_stream()).unwrap();
            spawner.spawn(task_callback_stream()).unwrap();
            // spawner.spawn(task_gcoap_server_stream()).unwrap();
        });
    }
}