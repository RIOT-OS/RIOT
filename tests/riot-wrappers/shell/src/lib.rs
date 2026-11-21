#![no_std]

use core::fmt::Write;
use riot_wrappers::println;
use riot_wrappers::riot_main;
use riot_wrappers::shell::CommandList;

riot_main!(main);

fn main() -> ! {
    let mut nonglobal_state = 0;

    // Not running anything fancy with run_once (where we could, for example, play around with
    // different buffer sizes) because .

    riot_wrappers::shell::new()
        .and(
            c"closure",
            c"Run a command that holds a mutable reference",
            |stdout, _args| {
                writeln!(stdout, "Previous state was {}", nonglobal_state).unwrap();
                nonglobal_state += 1;
                writeln!(stdout, "New state is {}", nonglobal_state).unwrap();
            },
        )
        .run_forever()
}

fn do_echo(_stdio: &mut riot_wrappers::stdio::Stdio, args: riot_wrappers::shell::Args<'_>) {
    println!("Arguments:");
    for a in args.iter() {
        println!("- {}", a);
    }
}
riot_wrappers::static_command!(
    echo,
    "echo",
    "Print the arguments in separate lines",
    do_echo
);
