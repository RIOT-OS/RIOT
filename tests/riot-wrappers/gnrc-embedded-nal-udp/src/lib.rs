#![no_std]

use riot_wrappers::{println, riot_main};

use embedded_nal::{UdpClientStack, UdpFullStack};

riot_main!(main);

fn main() {
    let mut stack: riot_wrappers::socket_embedded_nal::Stack<2> =
        riot_wrappers::socket_embedded_nal::Stack::new();

    stack.run(|mut stack| {
        let mut server = stack.socket().unwrap();
        stack.bind(&mut server, 1234).unwrap();

        let mut client = stack.socket().unwrap();
        stack.connect(&mut client, "[::1]:1234".parse().unwrap()).unwrap();

        stack.send(&mut client, b"hello").unwrap();
        let mut buf = [0; 16];
        match stack.receive(&mut server, &mut buf) {
            Ok((5, _)) if &buf[..5] == b"hello" => {
                println!("Tests completed.");
            }
            Ok((5, _)) => {
                println!("Tests failed: Weird, the length is right, but the content of the buffer is wrong");
            }
            x => {
                println!("Tests failed: Received {x:?}.");
            }
        }

        // No clean-up yet, so we just terminate like this:
        loop {
            riot_wrappers::thread::sleep();
        }
    });
}
