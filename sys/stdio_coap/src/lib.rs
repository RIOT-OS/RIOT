//! An implementation of stdio based on CoAP
//!
//! The stdout side is trivial in that it builds on stdout being a [scroll_ring::Buffer] and using
//! the implementation provided in [coap_scroll_ring_server] to expose it through CoAP.
//!
//! The stdin side is a manual implementation of a non-overwriting MPMC queue (which rejects
//! excessive input with a 503 error)
#![no_std]

use coap_handler_implementations::{HandlerBuilder, ReportingHandlerBuilder};
use ringbuf::{ring_buffer::RbBase, Rb};
use riot_wrappers::mutex::Mutex;

mod stdin;
use stdin::{Stdin, StdinHandler};

const STDOUT_BUFSIZE: usize = 1024;
const STDIN_BUFSIZE: usize = 128;

static mut STDOUT: Option<&'static scroll_ring::Buffer<STDOUT_BUFSIZE>> = None;

// FIXME: The whole construction of Stdin, STDIN and STDIN_WAITER could very well be described by a
// blocking SPMC thread-safe ring buffer (which we don't have in RIOT right now)

/// This is never held for long, and only awaited blockingly while also holding STDIN_WAITER.
///
/// It is only None during initialization.
static STDIN: Mutex<Option<&'static mut Stdin<STDIN_BUFSIZE>>> = Mutex::new(None);
/// A lock of this is stored in STDIN while any reader is waiting.
static STDIN_WAITER: Mutex<()> = Mutex::new(());

/// The CoAP handler for the stdout resource
///
/// If you do not use the auto-initialized CoAP tree, this handler needs to be placed discoverably
/// somewhere in the CoAP resource tree to make stdout accessible.
pub fn stdout_handler() -> coap_scroll_ring_server::BufferHandler<'static, STDOUT_BUFSIZE> {
    coap_scroll_ring_server::BufferHandler::new(unsafe { STDOUT }.expect("stdio was configured"))
}
///
/// The CoAP handler for the stdin resource
///
/// If you do not use the auto-initialized CoAP tree, this handler needs to be placed discoverably
/// somewhere in the CoAP resource tree to make stdin accessible.
pub fn stdin_handler() -> coap_handler_implementations::wkc::ConstantSingleRecordReport<
    'static,
    StdinHandler<STDIN_BUFSIZE>,
> {
    coap_handler_implementations::wkc::ConstantSingleRecordReport::new(
        StdinHandler(&STDIN),
        &[coap_handler::Attribute::Interface(
            "tag:riot-os.org,2021:ser-in",
        )],
    )
}

// FIXME: Declaring this creates a hard dependency on the auto_init module -- but we don't have an
// easy way to spot whether it's active right now. This practically makes the stdio_coap module
// depend on auto_init.

riot_wrappers::auto_init!(auto_init_stdio_coap_late, 42);

fn auto_init_stdio_coap_late() {
    let handler = coap_handler_implementations::new_dispatcher()
        .at(&["s", "o"], stdout_handler())
        .at(&["s", "i"], stdin_handler())
        ;

    // If we had feature(type_alias_impl_trait), we could use this, and drop the leaky_names
    // feature:
    // type HT = impl riot_wrappers::gcoap::Handler + 'static;
    // (By the way, I didn't come up with that type name ... I boxed things and had type_name
    // printed :-) )
    type HT = riot_wrappers::coap_handler::GcoapHandler<
        coap_handler_implementations::ForkingHandler<
            'static,
            coap_handler_implementations::wkc::ConstantSingleRecordReport<
                'static,
                StdinHandler<STDIN_BUFSIZE>,
            >,
            coap_handler_implementations::ForkingHandler<
                'static,
                coap_scroll_ring_server::BufferHandler<'static, STDOUT_BUFSIZE>,
                coap_handler_implementations::wkc::NotReporting<
                    coap_handler_implementations::NeverFound,
                >,
            >,
        >,
    >;
    static HANDLER: static_cell::StaticCell<HT> = static_cell::StaticCell::new();
    static LISTENER: static_cell::StaticCell<riot_wrappers::gcoap::SingleHandlerListener<HT>> =
        static_cell::StaticCell::new();

    let handler = HANDLER.init(riot_wrappers::coap_handler::GcoapHandler(handler));
    let listener = LISTENER.init(
        riot_wrappers::gcoap::SingleHandlerListener::new_with_link_encoder(
            core::ffi::CStr::from_bytes_with_nul(b"/s\0").unwrap(),
            riot_sys::COAP_GET
                | riot_sys::COAP_POST
                | riot_sys::COAP_FETCH
                | riot_sys::COAP_MATCH_SUBTREE,
            handler,
        ),
    );

    riot_wrappers::gcoap::register(listener);
}

#[no_mangle]
pub extern "C" fn stdio_init() {
    static SO: static_cell::StaticCell<scroll_ring::Buffer<STDOUT_BUFSIZE>> =
        static_cell::StaticCell::new();
    // Calling this twice would panic
    let so = SO.init_with(Default::default);

    // unsafe: There's only one place in the code that writes there, and this is guarded in the
    // previous line by a run-once condition.
    //
    // (And it may be safely read all the time, because there is no parallelism yet when this is
    // initialized)
    unsafe { STDOUT = Some(so) };

    static SI: static_cell::StaticCell<Stdin<STDIN_BUFSIZE>> = static_cell::StaticCell::new();
    let si = SI.init_with(|| Stdin {
        lock_for_waiter: None,
        buffer: Default::default(),
    });
    *STDIN.try_lock().unwrap() = Some(si);
}

#[no_mangle]
extern "C" fn stdio_write(
    buffer: *const riot_sys::libc::c_void,
    len: riot_sys::size_t,
) -> riot_sys::size_t {
    // unsafe: See its one writer.
    let stdout = unsafe { STDOUT };

    // unsafe: data is initialized per API
    let data = unsafe { core::slice::from_raw_parts(buffer as *const u8, len as _) };

    if let Some(stdout) = stdout {
        stdout.write(data);
        data.len() as _
    } else {
        // stdout not initialized
        0
    }
}

#[no_mangle]
extern "C" fn stdio_read(
    buffer: *mut riot_sys::libc::c_void,
    max_len: riot_sys::size_t,
) -> riot_sys::ssize_t {
    // API adjustments for type readability
    let mut buffer = buffer as *mut u8;
    let mut max_len = max_len as usize;

    if max_len == 0 {
        // The later loop requires that it can count down max_len after each popped item (which
        // requires popping first and checking after decrement).
        return 0;
    }

    loop {
        let waiter_lock = STDIN_WAITER.lock();

        // There's no guarantee that every reader from stdio is in a single thread, although that
        // appears to be a common assumption.
        let mut stdin = STDIN.lock();
        let mut stdin = stdin.as_mut().expect("STDIN was initialized");

        if stdin.buffer.is_empty() {
            // Move away waiter_lock on STDIN_WAITER, so when we're next entering the loop we're
            // blocked until there has been progress on stdin
            stdin.lock_for_waiter = Some(waiter_lock);
            drop(stdin);
            continue;
        }

        let mut assigned = 0;
        // This might be doable more efficiently, but then again, stdio_read is usually called with
        // a 1-long buffer anyway.
        for b in stdin.buffer.pop_iter() {
            // unsafe: Per C API
            unsafe { *buffer = b };
            // unsafe: Per C API
            buffer = unsafe { buffer.offset(1) };
            max_len -= 1;
            assigned += 1;
            if max_len == 0 {
                break;
            }
        }
        return assigned;
    }
}
