use riot_wrappers::{riot_sys::libc, println};
use crate::stream::{XStream, XStreamData, StreamExt};
use crate::static_borrow_mut;
use crate::alias::*;

extern "C" {
    fn shell_handle_input_line(command_list: *const libc::c_void, line: *const u8);
    fn async_shell_init() -> i8;
    fn async_shell_bufsize() -> usize;
    fn async_shell_prompt(tag_cstr: *const u8, highlight: bool);
    fn async_shell_app_commands() -> *const libc::c_void;
}

const ENABLE_RIOT_APP_COMMANDS: bool = true;
const ENABLE_PROMPT_TAGS: bool = true;
const ENABLE_PROMPT_HIGHLIGHT: bool = true;

const SHELL_BUFSIZE: usize = 128;
type ShellBuf = heapless::String<SHELL_BUFSIZE>;
static mut SHELL_BUF: ShellBuf = heapless::String::new();

const SHELL_STREAM_SIZE: usize = 2;
static mut SD: XStreamData<ShellBuf, SHELL_STREAM_SIZE> = XStream::init();

#[no_mangle]
pub extern fn async_shell_on_char(ch: u8) {
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
                        println!("input too long (> {}); to be ignored", SHELL_BUFSIZE - 1);
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
                            // println!("NOP; input (> SHELL_BUFSIZE={})", SHELL_BUFSIZE); // debug
                        });
                }
            },
        }
    }
}

mod statics { // add RIOT-wrappers' static command examples
    use super::*;
    use riot_wrappers::{static_command, stdio::Stdio, shell::Args};
    use riot_shell_commands::{ps, sleep as do_sleep};

    fn do_echo(_stdio: &mut Stdio, args: Args) {
        println!("Running args of run:");
        for a in args.iter() {
            println!("{:?}", a);
        }
    }

    fn do_ps(stdio: &mut Stdio, _args: Args) { ps(stdio); }

    static_command!(static_echo, "echo.rs", "Print the arguments in separate lines", do_echo);
    static_command!(static_ps, "ps.rs", "List the processes", do_ps);
    static_command!(static_sleep, "sleep.rs", "Pause the shell prompt", do_sleep);
}

pub async fn start() -> Result<(), i8> {
    assert_eq!(unsafe { async_shell_bufsize() }, SHELL_BUFSIZE);

    match riot_wrappers::BOARD {
        "native" => {
            let ret = unsafe { async_shell_init() };
            if ret > 0 { return Err(ret); }
        },
        _ => panic!("unsupported board"),
    }

    let riot_c_shell_commands = if ENABLE_RIOT_APP_COMMANDS {
        unsafe { async_shell_app_commands() } // add RIOT-c app commands
    } else {
        core::ptr::null() // use RIOT-c system commands only
    };

    let mut xs = XStream::get(static_borrow_mut!(SD));
    print_aliases();
    prompt();

    let trim = |line: &mut ShellBuf| {
        let lc = line.clone();
        line.clear();
        line.push_str(lc[..lc.len() - 1].trim()).unwrap();
        line.push('\0').unwrap();
    };

    // process line input stream
    while let Some(mut line) = xs.next().await {
        assert!(line.ends_with("\0"));

        trim(&mut line);
        if line != "\0" {
            if let Some(fut) = apply_alias(&mut line) {
                fut.await; // run function alias
            }

            unsafe { shell_handle_input_line(riot_c_shell_commands, line.as_ptr()); }
        }

        prompt();
    }
    panic!("should be never reached");
}

fn prompt() {
    let tag = if ENABLE_PROMPT_TAGS {
        Some("(async)\0")
    } else { None };

    let tag = if let Some(x) = tag {
        assert!(x.ends_with("\0"));
        x.as_ptr()
    } else {
        core::ptr::null()
    };

    unsafe { async_shell_prompt(tag, ENABLE_PROMPT_HIGHLIGHT); }
}

fn prompt_is_ready() -> Option<XStream<ShellBuf, SHELL_STREAM_SIZE>> {
    let xs = XStream::get(static_borrow_mut!(SD));

    if xs.len() == 0 { // no pending items
        Some(xs)
    } else { None }
}

fn print_aliases() {
    println!("---- named alias ----");
    TABLE_ALIAS_NAMED.iter()
        .for_each(|(name, cmd)| println!("[{}] {}", name, cmd));

    println!("---- function alias ----");
    TABLE_ALIAS_FUNCTION.iter()
        .for_each(|name| println!("[{}] <function>", name));

    println!("---- enumerated alias ----");
    TABLE_ALIAS_ENUMERATED.iter().enumerate()
        .for_each(|(idx, cmd)| println!("[{}] {}", idx, cmd));
}

fn apply_alias(line: &mut ShellBuf) -> Option<impl core::future::Future> {
    assert!(line.ends_with("\0"));

    let update = |line: &mut ShellBuf, alias| {
        line.clear();
        line.push_str(alias).unwrap();
        line.push('\0').unwrap();
    };

    let ln = &line[..line.len() - 1]; // chars that precede '\0'

    if ln == "alias" || ln == "a" {
        print_aliases();
        update(line, "");
    } else if let Some(item) = TABLE_ALIAS_NAMED.iter().find(|item| item.0 == ln) {
        update(line, item.1);
    } else if let Some(item) = TABLE_ALIAS_FUNCTION.iter().find(|item| **item == ln) {
        update(line, "");
        return Some(run_function_alias(item));
    } else if let Ok(x) = ln.parse::<usize>() {
        if x < TABLE_ALIAS_ENUMERATED.len() {
            update(line, TABLE_ALIAS_ENUMERATED[x]);
        }
    }

    None
}