use riot_wrappers::println;

pub const TABLE_ALIAS_NAMED: &[(&str, &str)] = &[
    ("a", "alias"),
    ("h", "help"),
];

pub const TABLE_ALIAS_ENUMERATED: &[&str] = &[
    "version",
    "ifconfig",
    "ping ::1",
];

//---- provably to be refactored like: function_alias!(...)
pub const TABLE_ALIAS_FUNCTION: &[&str] = &[
    "f0",
    "f1",
    "f2",
];

pub async fn run_function_alias(name: &str) {
    match name {
        "f0" => (|| println!("hello world!"))(),
        "f1" => test_async_sleep().await,
        "f2" => test_async_client().await,
        _ => println!("oops, code for function alias [{}] is missing!", name),
    }
}
//----

async fn test_async_sleep() {
    use riot_wrappers::ztimer::*;

    println!("test_async_sleep():");
    for count in 0..3 {
        println!("{}", count + 1);
        Clock::msec().sleep_async(Ticks(1_000)).await;
    }
}

async fn test_async_client() {
    println!("test_async_client(): TODO");

/* ok
active interface from PID KernelPID(6) ("gnrc_netdev_tap")
    Address fe80:0000:0000:0000:a4ec:98ff:fe0e:af24

$ libcoap/local/bin/coap-client -m get coap://[fe80:0000:0000:0000:a4ec:98ff:fe0e:af24%tap1]/.well-known/core
<>;ct=0;title="Landing page"</time>;ct=0;title="Clock"</poem>;sz=1338</cbor>;ct=60</ps/>;title="Process list"</vfs/>;ct=0</saul/>;title="SAUL index"
*/
}