//! Tools for running RIOT's built-in shell
//!
//! This module can be used in two ways:
//!
//! * Declare static commands using [`static_command!`](crate::static_command!); these only take a
//!   `fn` (not a closure) because shell commands don't have an arg pointer.
//!
//!   This works even in RIOT modules that are included in a C application that starts a shell, and
//!   show up in shells created through Rust without explicit inclusion.
//!
//! * Use [new] to start building a [CommandList]. This can have full closures as commands, but
//!   these are available only when the shell is then started through the CommandList's run
//!   methods.
//!
//! ## Note on complexity of this module
//!
//! Quite a bit of complexity in this module is due to building the array of commands, and
//! moreover, creating trampoline functions that go through a global mutex to get a hold of the
//! command list -- an exercise necessary due to the RIOT commands' lack of a `*void data`
//! argument. This does allow the Rust wrappers to "just so" use a closure as a command handler,
//! but also needs a lot of code.
//!
//! That complexity is not pulled in when only using [`static_command!`](crate::static_command!)
//! and running on an otherwise empty command list.

use crate::libc;
use crate::{mutex, stdio};
use core::ffi::CStr;
use riot_sys::{shell_command_t, shell_run_forever, shell_run_once};

mod args;

pub use args::Args;
// re-exported only as long as users can't just make a TAIT out of the Args return type.
pub use args::ArgsIterator;

/// Something that can build a suitable command array for itself and its next commands using
/// `shell_run_once` etc.
///
/// This is unsafe to impleemnt as all implementers must guarantee that a reference to the Built
/// type can be cast to a shell_command_t and that all commands in there are contiguous up until a
/// nulled one.
pub unsafe trait CommandListInternals: Sized {
    type Built: 'static;

    fn build_shell_command<Root: CommandListInternals>(&self) -> Self::Built;

    // Common code of run_once and run_forever. It is generic over F rather than taking F: unsafe
    // extern "C" because while shell_run_once is extern "C", shell_run_forever is actually in Rust
    // representation as it's a static inline in C.
    //
    // The R return value is then either () or !.
    //
    // It is set to always inline because situations in which both run_once and run_forever are
    // used are expected to be very rare.
    #[inline(always)]
    fn run_any<R, F: Fn(*const riot_sys::shell_command_t, *mut libc::c_char, i32) -> R>(
        &mut self,
        linebuffer: &mut [u8],
        cb: F,
    ) -> R {
        let mut global = CURRENT_SHELL_RUNNER.lock();
        // Actually, if we really needed this, *and* could be sure that the shells are strictly
        // nested and not just started in parallel threads (how would we?), we could just stash
        // away the other callback, do our thing and revert it before leaving this function.
        assert!(
            global.is_none(),
            "Simultaneously running shells are not supported"
        );

        let built = self.build_shell_command::<Self>();

        // mutex is maybe not the right abstraction; something different could do this if it
        // had a "put it in there until you give it back when the function returns, and users may
        // take it for some time" semantics.
        *global = Some(SleevedCommandList(self as *mut _ as *mut _));

        // Release mutex so that running shell commands can use it
        drop(global);

        // unsafe: The cast is legitimized by the convention of all Built being constructed to give
        // a null-terminated array
        let result = cb(
            &built as *const _ as *const riot_sys::shell_command_t,
            linebuffer.as_mut_ptr() as _,
            linebuffer.len() as _,
        );

        CURRENT_SHELL_RUNNER.lock().take();

        result
    }

    /// Run your own callback with argc and argv if the called argument is what the implementation
    /// put into its own entry of its Built, or defer to its next.
    fn find_self_and_run(
        &mut self,
        argc: i32,
        argv: *mut *mut libc::c_char,
        command_index: usize,
    ) -> i32;

    #[inline(never)]
    fn find_root_and_run(argc: i32, argv: *mut *mut libc::c_char, command_index: usize) -> i32 {
        let lock = CURRENT_SHELL_RUNNER
            .try_lock()
            .expect("Concurrent shell commands");
        let sleeve = lock
            .as_ref()
            .expect("Callback called while no shell set up as running");
        let result;
        {
            // unsafe: A suitable callback is always configured. We can make a &mut out of it for as
            // long as we hold the lock.
            let root = unsafe { &mut *(sleeve.0 as *mut Self) };
            result = root.find_self_and_run(argc, argv, command_index);
        }
        drop(lock);
        result
    }
}

/// A list of commands that can be presented as a shell prompt
///
/// The BUFSIZE is carried around in the trait because it can have a default there (a trait method
/// can't have a default value for its generic argument), which necessitates that implementers use
/// `with_buffer_size` to change it and carry that size on. (Having a `.run_forever()` /
/// `.run_forever<BUFSIZE = 60>()` would be ideal, but that's currently not possible).
pub trait CommandList<const BUFSIZE: usize = { riot_sys::SHELL_DEFAULT_BUFSIZE as _ }>:
    CommandListInternals
{
    fn run_once_with_buf(&mut self, linebuffer: &mut [u8]) {
        // unsafe: See unsafe in run_any where it's called
        self.run_any(linebuffer, |built, buf, len| unsafe {
            shell_run_once(built, buf, len)
        })
    }

    fn run_forever_with_buf(&mut self, linebuffer: &mut [u8]) -> ! {
        // unsafe: See unsafe in run_any where it's called
        self.run_any(linebuffer, |built, buf, len| unsafe {
            shell_run_forever(built as _, buf, len);
            unreachable!()
        })
    }

    /// Run the shell prompt on stdio
    ///
    /// See [shell_run_forever] for details.
    ///
    /// [shell_run_forever]: https://doc.riot-os.org/group__sys__shell.html#ga3d3d8dea426c6c5fa188479e53286aec
    ///
    /// The line buffer is allocated inside this function with the size configured as part of the
    /// trait type; use `.with_buffer_size::<>()` to alter that.
    #[doc(alias = "shell_run_forever")]
    fn run_forever(&mut self) -> ! {
        let mut linebuffer = [0; BUFSIZE];
        self.run_forever_with_buf(&mut linebuffer)
    }

    /// Run the shell prompt on stdio until EOF is reached
    ///
    /// See [shell_run_once] for details.
    ///
    /// [shell_run_once]: https://doc.riot-os.org/group__sys__shell.html#ga3d3d8dea426c6c5fa188479e53286aec
    ///
    /// The line buffer is allocated inside this function with the size configured as part of the
    /// trait type; use `.with_buffer_size::<>()` to alter that.
    #[doc(alias = "shell_run_once")]
    fn run_once(&mut self) {
        let mut linebuffer = [0; BUFSIZE];
        self.run_once_with_buf(&mut linebuffer)
    }

    #[deprecated(note = "Renamed to run_forever", since = "0.9")]
    fn run_forever_providing_buf(&mut self) -> ! {
        self.run_forever()
    }

    #[deprecated(note = "Renamed to run_once", since = "0.9")]
    fn run_once_providing_buf(&mut self) {
        self.run_once()
    }

    /// Extend the list of commands by an additional one.
    ///
    /// The handler will be called every time the command is entered, and is passed the arguments
    /// including its own name in the form of [Args]. Currently, RIOT ignores the return value of
    /// the function.
    fn and<'a, H, T>(self, name: &'a CStr, desc: &'a CStr, handler: H) -> impl CommandList<BUFSIZE>
    where
        H: FnMut(&mut stdio::Stdio, Args<'_>) -> T,
        T: crate::main::Termination,
    {
        Command {
            name,
            desc,
            handler,
            next: self,
        }
    }

    /// Change the buffer size used for `.run_forever_providing_buf()`.
    ///
    /// Note that no buffer of that size is carried around -- it is merely transported in the trait
    /// to provide a (defaultable) number for that method.
    fn with_buffer_size<const NEWSIZE: usize>(self) -> Self::WithBufferSizeResult<NEWSIZE>;
    type WithBufferSizeResult<const NEWSIZE: usize>: CommandList<NEWSIZE>;
}

// For a bit more safety -- not that anything but someone stealing the module-private
// CURRENT_SHELL_RUNNER and replacing its content in an uncontrolled fashion would disturb the
// peace here --, this could be *almost* made a *mut dyn core::any::Any, and then use
// downcast_mut() in the handlers to get back the right Root, verifying in the process that indeed
// we agere on what it is in there. That currently doesn't work because the Root is not necessarily
// 'static (but typically only lives its 'a).
struct SleevedCommandList(*mut crate::libc::c_void);

// unsafe: The only way we access the pointer in there is through callbacks we only let RIOT from
// the shell function, and this all happens in the same thread.
//
// (The sleeve allows putting the pointer into a global mutex in the first place).
unsafe impl Send for SleevedCommandList {}

static CURRENT_SHELL_RUNNER: mutex::Mutex<Option<SleevedCommandList>> = mutex::Mutex::new(None);

/// Internal helper that is used to create the linear [`riot_sys::shell_command_t`] structure that a
/// command list needs to pass to RIOT
///
/// (Exposed publicly as the [`CommandList::and`] trait method can not return an `impl CommandList`
/// yet)
#[repr(C)]
pub struct BuiltCommand<NextBuilt> {
    car: shell_command_t,
    cdr: NextBuilt,
}

/// Internal helper that holds the data assembled using the [`CommandList::and`] builder
///
/// (Exposed publicly as the [`CommandList::and`] trait method can not return an `impl CommandList`
/// yet)
pub struct Command<'a, Next, H, T = i32>
where
    Next: CommandListInternals,
    H: FnMut(&mut stdio::Stdio, Args<'_>) -> T,
    T: crate::main::Termination,
{
    name: &'a CStr,
    desc: &'a CStr,
    handler: H,
    next: Next,
}

impl<'a, Next, H, T> Command<'a, Next, H, T>
where
    Next: CommandListInternals,
    H: FnMut(&mut stdio::Stdio, Args<'_>) -> T,
    T: crate::main::Termination,
{
    /// This is building a trampoline. As it's static and thus can't have the instance, we pass on
    /// a disambiguator (the command_index) for the globally stored root to pick our own self out of
    /// its tail again.
    ///
    /// As all the commands in the list are serialized into a single CommandListInternals at the
    /// root, they are all nested, and thus have sequential tail sizes. Over using the own TypeId,
    /// this gives the advantage of building shorter trampolines (14 bytes rather than 24 on
    /// Cortex-M3), and also allows the find_self_and_run function to optimize better, as all its
    /// jumps are from a contiguous small range (think `match ... {1 => one(), 2 => two(), _ =>
    /// panic!()}` rather than arbitrary large numbers; the compiler range check once and then pick
    /// the jump address from a table).
    extern "C" fn handle<Root: CommandListInternals>(
        argc: i32,
        argv: *mut *mut libc::c_char,
    ) -> i32 {
        Root::find_root_and_run(argc, argv, Self::tailsize())
    }

    /// Size of the own type's built structs, in multiples of shell_command_t.
    ///
    /// Usef for finding the own instance again, see handle documentation.
    const fn tailsize() -> usize {
        core::mem::size_of::<<Self as CommandListInternals>::Built>()
            / core::mem::size_of::<shell_command_t>()
    }
}

unsafe impl<'a, Next, H, T> CommandListInternals for Command<'a, Next, H, T>
where
    Next: CommandListInternals,
    H: FnMut(&mut stdio::Stdio, Args<'_>) -> T,
    T: crate::main::Termination,
{
    type Built = BuiltCommand<Next::Built>;

    fn build_shell_command<Root: CommandListInternals>(&self) -> Self::Built {
        BuiltCommand {
            car: shell_command_t {
                name: self.name.as_ptr() as _,
                desc: self.desc.as_ptr() as _,
                handler: Some(Self::handle::<Root>),
            },
            cdr: self.next.build_shell_command::<Root>(),
        }
    }

    // This is explicitly marked as inline as the large if / else if tree that it logically builds
    // should really be treated like a match by the optimizer, and not accumulate stack frames for
    // the commands deep down in the tree.
    #[inline]
    fn find_self_and_run(
        &mut self,
        argc: i32,
        argv: *mut *mut libc::c_char,
        command_index: usize,
    ) -> i32 {
        if command_index == Self::tailsize() {
            let marker = ();
            let args = unsafe { Args::new(argc, argv as _, &marker) };
            let handler = &mut self.handler;
            let mut stdio = stdio::Stdio {};
            handler(&mut stdio, args)
                // see https://gitlab.com/etonomy/riot-wrappers/-/issues/3
                .report() as _
        } else {
            self.next.find_self_and_run(argc, argv, command_index)
        }
    }
}

impl<'a, Next, H, T, const BUFSIZE: usize> CommandList<BUFSIZE> for Command<'a, Next, H, T>
where
    Next: CommandListInternals,
    H: FnMut(&mut stdio::Stdio, Args<'_>) -> T,
    T: crate::main::Termination,
{
    fn with_buffer_size<const NEWSIZE: usize>(self) -> Self::WithBufferSizeResult<NEWSIZE> {
        Command { ..self }
    }
    type WithBufferSizeResult<const NEWSIZE: usize> = Command<'a, Next, H, T>;
}

struct CommandListEnd;

unsafe impl CommandListInternals for CommandListEnd {
    type Built = shell_command_t;

    fn build_shell_command<Root: CommandListInternals>(&self) -> Self::Built {
        shell_command_t {
            name: core::ptr::null(),
            desc: core::ptr::null(),
            handler: None,
        }
    }

    #[inline]
    fn find_self_and_run(
        &mut self,
        _argc: i32,
        _argv: *mut *mut libc::c_char,
        _command_index: usize,
    ) -> i32 {
        panic!("No handler claimed the callback");
    }
}

impl<const BUFSIZE: usize> CommandList<BUFSIZE> for CommandListEnd {
    fn with_buffer_size<const NEWSIZE: usize>(self) -> Self::WithBufferSizeResult<NEWSIZE> {
        CommandListEnd
    }
    type WithBufferSizeResult<const NEWSIZE: usize> = CommandListEnd;
}

/// Start a blank list of commands
///
/// This returns an empty command list that can be run as is (to expose RIOT's built-in shell
/// commands), or as a starting point for adding more commands using its [`CommandList::and`]
/// builder.
pub fn new() -> impl CommandList {
    CommandListEnd
}

/// Make a function whose signature is `fn(&mut `[`Stdio`](stdio::Stdio)`, `[`Args`]`<'b>) -> impl `[`Termination`](crate::main::Termination) available through
/// XFA in any RIOT shell, even when called throuch C. (The function's signature may be more
/// generic, eg. accepting an `impl `[`Write`](core::fmt::Write) and an `impl `[`IntoIterator`]`<&str>`).
///
/// Compared to [CommandList], this is limited by only taking functions and not closures -- but
/// that allows using it even in scenarios where [CommandList]'s hacks that reconstruct a full
/// closure from something that's only a plain function call in C are unavailable.
///
/// The modname identifier needs to be provided as a name that can be used for a private module
/// created by the macro. The name literal is the command name as matched by the shell, with the
/// descr literal shown next to it when running `help`. The fun is a local function of static
/// lifetime that gets executed whenever the shell command is invoked.
///
/// Example
/// -------
///
/// ```
/// # #![no_std]
/// # #![feature(start)]
/// fn do_echo(
///         _stdio: &mut riot_wrappers::stdio::Stdio,
///         args: riot_wrappers::shell::Args<'_>,
/// )
/// {
///     use riot_wrappers::println;
///     println!("Running args of run:");
///     for a in args.iter() {
///         println!("{:?}", a);
///     }
/// }
/// # #[start]
/// # fn main(_argc: isize, _argv: *const *const u8) -> isize {
/// riot_wrappers::static_command!(echo, "echo", "Print the arguments in separate lines", do_echo);
/// # 0
/// # }
/// ```
#[macro_export]
macro_rules! static_command {
    ( $modname:ident, $name:literal, $descr:literal, $fun:ident ) => {
        // Note that this winds up in a dedicated compilation unit; the C linker may not use them
        // when running from the staticlib, which is why RIOT is going towards linking all .o
        // files.
        mod $modname {
            use super::$fun;

            // The transparent allows the &StaticCommand to have the right properties to be storable in a
            // static, and still be the same pointer.
            #[repr(transparent)]
            pub struct StaticCommand($crate::riot_sys::shell_command_t);

            // unsafe: OK due to the only construction way (the CStr is created from a literal and
            // thus static, and the_function is static by construction as well)
            unsafe impl Sync for StaticCommand {}

            #[link_section = ".roxfa.shell_commands_xfa_v2.5"]
            #[export_name = concat!("shell_commands_xfa_v2_5_", stringify!($modname))]
            static THE_STRUCT: StaticCommand = StaticCommand($crate::riot_sys::shell_command_t {
                name: $crate::cstr::cstr!($name).as_ptr() as _,
                desc: $crate::cstr::cstr!($descr).as_ptr() as _,
                handler: Some(the_function),
            });

            unsafe extern "C" fn the_function(
                argc: i32,
                argv: *mut *mut $crate::riot_sys::libc::c_char,
            ) -> i32 {
                let marker = ();
                let args = unsafe { $crate::shell::Args::new(argc, argv as _, &marker) };
                let mut stdio = $crate::stdio::Stdio {};
                use $crate::main::Termination;
                $fun(&mut stdio, args).report()
            }
        }
    };
}
