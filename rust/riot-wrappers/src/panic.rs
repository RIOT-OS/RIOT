#[panic_handler]
fn panic(info: &::core::panic::PanicInfo) -> ! {
    use crate::thread;

    let os_can_continue = !cfg!(feature = "panic_handler_crash")
        && crate::thread::InThread::new()
            // Panics with IRQs off are fatal because we can't safely re-enable them
            .map(|i| i.irq_is_enabled())
            // Panics in ISRs are always fatal because continuing in threads would signal to the
            // remaining system that the ISR terminated
            .unwrap_or(false);

    if !os_can_continue {
        // We can't abort on stable -- but even if we could: Set a breakpoint and wait for the
        // fault handler to reboot us of no debugger is attached? Spin endlessly? core_panic should
        // already answer all these questions.

        // Not attempting to print -- it would only get through on devices where stdio is provided
        // by a UART, and with these the debugger is usually also close enough that the risk of
        // smashing things by overflowing the ISR stack outweighs the benefits.

        unsafe {
            riot_sys::core_panic(
                riot_sys::core_panic_t_PANIC_GENERAL_ERROR,
                c"RUST PANIC".as_ptr() as _,
            )
        };
    } else {
        // I *guess* it's OK for a panic to simply make a thread into a zombie -- this does allow other
        // threads (including spawned Rust threads) to continue, but my layman's understanding of
        // panicking is that that's OK because whatever we were just mutating can simply never be used
        // by someone else ever again.

        let me = thread::get_pid();

        if cfg!(feature = "panic_handler_format") {
            use crate::stdio::println;

            println!(
                "Error in thread {:?} ({}):",
                me,
                me.get_name().unwrap_or("unnamed")
            );
            println!("{}", info);
        } else {
            let mut stdio = crate::stdio::Stdio {};
            use core::fmt::Write;
            let _ = stdio.write_str("Panic in thread ");
            let _ = stdio.write_str(me.get_name().unwrap_or("unnamed"));
            let _ = stdio.write_str("!\n");
        }

        // Not trying any unwinding -- this thread is just dead, won't be re-claimed, any mutexes it
        // holds are just held indefinitely rather than throwing poison errors.
        loop {
            thread::sleep();
        }
    }
}

// There is no need to set the lang item (recent Rust versions plainly err with "unwinding panics
// are not supported without std" anyway when attempting to build without panic="abort"), but the
// pre-built core library does panic in some situations. While CARGO_OPTIONS+=-Zbuild-std=core is a
// viable solution, it needs nightly and is thus not suitable for docker builds (where the number
// of installed toolchains is kept at a minimum). Instead, we merely define the symbol, which is
// enough to fix the linker errors that would otherwise be raised in nontrivial applications.
//
// We need to do this on precisely those RUST_TARGET values selected through RIOT that are built
// with unwinding panic (which is those that have std; at the time of writing,
// i686-unknown-linux-gnu and x86_64-unknown-linux-gnu), but there is no harm in defining the
// symbol on other platforms for simplicity.
#[no_mangle]
unsafe extern "C" fn rust_eh_personality() {
    loop {}
}
