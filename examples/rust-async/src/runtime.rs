use riot_wrappers::{println, ztimer};
use crate::executor::Executor;

async fn async_yield() {
    println!("tok");
}

#[embassy_executor::task]
async fn task_main(throttle: u32) {
    crate::async_main().await;

    let timer = ztimer::Clock::msec();
    loop {
        timer.sleep_ticks(throttle);
        println!("tik");
        async_yield().await;
    }
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

#[embassy_executor::task]
async fn task_shell_stream() {
    xbd::process_shell_stream().await.unwrap();
}
*/

pub struct Runtime(Executor);

impl Runtime {
    pub fn new() -> Self {
        Self(Executor::new())
    }

    pub fn run(&'static mut self) -> ! {
        let throttle = 200;
        println!("@@ task_main(): throttle: {} ms", throttle);

        self.0.run(|spawner| {
            spawner.spawn(task_main(throttle)).unwrap();
            // spawner.spawn(task_shell_stream()).unwrap();
            // spawner.spawn(task_gcoap_server_stream()).unwrap();
            // spawner.spawn(task_api_stream()).unwrap();
        });
    }
}