//use riot_wrappers::println;
use crate::executor::Executor;

#[embassy_executor::task]
async fn task_main() {
    crate::async_main().await;
}

#[embassy_executor::task]
async fn task_shell_stream() {
    //crate::shell::process_shell_stream().await.unwrap();
}

/*
#[embassy_executor::task]
async fn task_api_stream() {
    xbd::process_api_stream().await.unwrap();
}

#[embassy_executor::task]
async fn task_gcoap_server_stream() {
    xbd::process_gcoap_server_stream().await.unwrap();
}
*/

pub struct Runtime(Executor);

impl Runtime {
    pub fn new() -> Self {
        Self(Executor::new(Some(100)))
    }

    pub fn run(&'static mut self) -> ! {
        self.0.run(|spawner| {
            spawner.spawn(task_main()).unwrap();
            spawner.spawn(task_shell_stream()).unwrap();
            // spawner.spawn(task_gcoap_server_stream()).unwrap();
            // spawner.spawn(task_api_stream()).unwrap();
        });
    }
}