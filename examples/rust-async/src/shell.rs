use super::stream::{XStream, XStreamData, StreamExt};
use crate::{println, static_borrow_mut};

extern "C" {
    fn xbd_shell_get_commands() -> *const ();
    fn xbd_async_shell_init() -> i8;
    fn xbd_async_shell_bufsize() -> usize;
    fn xbd_async_shell_prompt(tag_cstr: *const u8, highlight: bool);
    fn handle_input_line_minerva(command_list: *const (), line: *const u8);
}

const SHELL_BUFSIZE: usize = 128;
type ShellBuf = heapless::String<SHELL_BUFSIZE>;
static mut SHELL_BUF: ShellBuf = heapless::String::new();

const SHELL_STREAM_SIZE: usize = 2;
static mut SD: XStreamData<ShellBuf, SHELL_STREAM_SIZE> = XStream::init();

#[no_mangle]
pub extern fn xbd_async_shell_on_char(ch: u8) {
    //println!("@@ xbd_async_shell_on_char(): {}", ch);

    if let Some(mut xs) = prompt_is_ready() {
        let ch = ch as char;
        match ch {
            '\0' => { // end of input
                unsafe {
                    let null_terminated;
                    if let Ok(_) = SHELL_BUF.push(ch) {
                        null_terminated = SHELL_BUF.clone();
                        SHELL_BUF.clear();
                    } else {
                        println!("@@ input too long (> {}); to be ignored", SHELL_BUFSIZE - 1);
                        SHELL_BUF.clear();
                        SHELL_BUF.push(ch).unwrap();
                        null_terminated = SHELL_BUF.clone();
                    }

                    xs.add(null_terminated);
                }
            },
            '\r' | '\n' => (), // ignore it
            _ => { // keep it
                unsafe {
                    SHELL_BUF
                        .push(ch)
                        .unwrap_or_else(|_| { // input too long
                            //println!("@@ NOP; input (> SHELL_BUFSIZE={})", SHELL_BUFSIZE);
                        });
                }
            },
        }
    }
}

pub async fn process_shell_stream() -> Result<(), i8> {
    assert_eq!(unsafe { xbd_async_shell_bufsize() }, SHELL_BUFSIZE);

    let ret = unsafe { xbd_async_shell_init() };
    match ret {
        0 => (), // ok, continue
        2 => { // kludge
            println!("@@ process_shell_stream(): TODO - support non-native board");
            return Ok(());
        },
        _ => return Err(ret),
    }

    //let shell_commands = core::ptr::null(); // system commands only
    let shell_commands = unsafe { xbd_shell_get_commands() };

    let mut xs = XStream::get(static_borrow_mut!(SD));
    print_aliases();
    prompt();

    while let Some(mut line) = xs.next().await {
        assert!(line.ends_with("\0"));

        println!("[async shell] (null terminated) line: {} (len: {} SHELL_BUFSIZE: {})",
                 line, line.len(), SHELL_BUFSIZE);
        //println!("  line.as_bytes(): {:?}", line.as_bytes());
        //println!("  line: {:?}", line);

        if line.trim() != "\0" {
            if match_alias(&mut line).await {
                assert!(line.ends_with("\0"));
            }

            unsafe { handle_input_line_minerva(shell_commands, line.as_ptr()); }
        }

        prompt();
    }

    Ok(())
}

fn prompt() {
    //let tag: Option<&str> = None;
    let tag = Some("(async)\0");

    let tag = if let Some(x) = tag {
        assert!(x.ends_with("\0"));
        x.as_ptr()
    } else {
        core::ptr::null()
    };

    unsafe { xbd_async_shell_prompt(tag, true); }
}

fn prompt_is_ready() -> Option<XStream<ShellBuf, SHELL_STREAM_SIZE>> {
    let xs = XStream::get(static_borrow_mut!(SD));

    if xs.len() == 0 { // no pending items
        Some(xs)
    } else { None }
}

//

async fn test_async_sleep() {
    println!("test_async_sleep(): ^^");
//    crate::Xbd::async_sleep(2_000).await;
    println!("test_async_sleep(): $$");
}

async fn test_async_blockwise_get() {
    println!("test_async_blockwise_get(): ^^");
/*
    let mut bs = crate::Xbd::async_gcoap_get_blockwise(
        "[::1]:5683", "/const/song.txt").unwrap();
    while let Some(req) = bs.next().await {
        println!("@@ out: {:?}", req.await);
    }
*/
    println!("test_async_blockwise_get(): $$");
}

//

const ARRAY_ALIAS_NAMED: &[(&str, &str)] = &[
    ("a", "alias"),
    ("h", "help"),
];

const ARRAY_ALIAS_ENUMERATED: &[&str] = &[
    "version",
    "ifconfig",
    "gcoap get [::1] /riot/board",
    "gcoap get [::1] /const/song.txt",
];

const ARRAY_ALIAS_FUNCTION: &[&str] = &[
    "f0",
    "f1",
    "f2",
    "f3",
    //"f99", // test undefined
];

async fn run_function_alias(name: &str) {
    match name {
        "f0" => (|| println!("hello world!"))(),
        "f1" => test_async_sleep().await,
        "f2" => test_async_blockwise_get().await,
//        "f3" => crate::test_blockwise("[::1]:5683").await.unwrap(),
        _ => println!("function alias [{}] is not defined!", name),
    }
}

fn print_aliases() {
    println!("---- named alias ----");
    ARRAY_ALIAS_NAMED.iter()
        .for_each(|(name, cmd)| println!("[{}] {}", name, cmd));

    println!("---- function alias ----");
    ARRAY_ALIAS_FUNCTION.iter()
        .for_each(|name| println!("[{}] <function>", name));

    println!("---- enumerated alias ----");
    ARRAY_ALIAS_ENUMERATED.iter().enumerate()
        .for_each(|(idx, cmd)| println!("[{}] {}", idx, cmd));
}

async fn match_alias(line: &mut ShellBuf) -> bool {
    assert!(line.ends_with("\0"));

    let expand = |line: &mut ShellBuf, alias| {
        line.clear();
        line.push_str(alias).unwrap();
        line.push('\0').unwrap();

        true // `line` updated
    };

    let ln = &line[..line.len() - 1]; // chars that precede '\0'

    if ln == "alias" || ln == "a" {
        print_aliases();
        return expand(line, "");
    } else if let Some(item) = ARRAY_ALIAS_NAMED.iter().find(|item| item.0 == ln) {
        return expand(line, item.1);
    } else if let Some(item) = ARRAY_ALIAS_FUNCTION.iter().find(|item| **item == ln) {
        run_function_alias(item).await;
        return expand(line, "");
    } else if let Ok(x) = ln.parse::<usize>() {
        if x < ARRAY_ALIAS_ENUMERATED.len() {
            return expand(line, ARRAY_ALIAS_ENUMERATED[x]);
        }
    }

    false
}