use riot_wrappers::println;

pub const TABLE_ALIAS_NAMED: &[(&str, &str)] = &[
    ("a", "alias"),
    ("h", "help"),
    ("p", "ps.rs"),
];

pub const TABLE_ALIAS_ENUMERATED: &[&str] = &[
    "version",
    "ifconfig",
    "ping ::1",
];

pub const TABLE_ALIAS_FUNCTION: &[&str] = &[
    "f0",
    "f1",
    "f2",
];

pub async fn run_function_alias(name: &str) {
    match name {
        "f0" => (|| println!("hello world!"))(),
        "f1" => test_async_sleep().await,
        "f2" => test_async_timeout().await,
        _ => println!("oops, code for function alias [{}] is missing!", name),
    }
}

async fn test_async_sleep() {
    println!("test_async_sleep():");

    for count in 1..=3 {
        println!("{}", count);
        crate::util::sleep_msec(1000).await;
    }
}

async fn test_async_timeout() {
    println!("test_async_timeout():");

    crate::util::set_timeout(2000, || println!("it works!")).await;
}