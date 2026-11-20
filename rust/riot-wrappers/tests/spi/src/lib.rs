//! This is a primitive SPI test.
//!
//! It performs different reads and writes in different modes. The precise results depend on
//! the voltage on the MISO pin, which is usually indeterminate.
#![no_std]

use embedded_hal::spi::SpiDevice;

use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    // SPI device and CS pin that the test may drive.
    //
    // Boards should only be added if the attached peripherals are safe to use, no matter what gets
    // written there.
    let (spi_num, cs_num) = match riot_wrappers::BOARD {
        "particle-xenon" => (0, (0, 31)),
        _ => panic!("No "),
    };

    let cs = riot_wrappers::gpio::GPIO::from_port_and_pin(cs_num.0, cs_num.1).unwrap();
    let spi = riot_wrappers::spi::for_embedded_hal_1::SpiBus::from_number(spi_num)
        // arbitrary parameters
        .with_speed_1mhz()
        .with_mode(embedded_hal::spi::MODE_2);

    println!("Testing with software CS");
    // Writing a test for the SpiBus would be annoyingly repetitive compared to the one for
    // SpiDevice; using the SpiBus through the embedded-hal-bus mechanism instead.
    //
    // Also, there's not really anything about this test where the use of the CS pin makes any
    // difference in the outcome, it's more to cover the full API surface.
    let Ok(mut spi_with_soft_cs) = embedded_hal_bus::spi::ExclusiveDevice::new(
        spi,
        cs.configure_as_output(riot_wrappers::gpio::OutputMode::Out)
            .unwrap(),
        riot_wrappers::ztimer::Clock::usec(),
    );
    test_on_device(&mut spi_with_soft_cs);

    // ExclusiveDevice has no destructuring finalizer, so we just rebuild the pieces, which RIOT
    // lets us do -- and it won't cause any sort of practical trouble because the exclusive device
    // is not used any more, probably even dropped already.
    let cs = riot_wrappers::gpio::GPIO::from_port_and_pin(cs_num.0, cs_num.1).unwrap();
    let spi = riot_wrappers::spi::for_embedded_hal_1::SpiBus::from_number(spi_num)
        // arbitrary parameters
        .with_speed_1mhz()
        .with_mode(embedded_hal::spi::MODE_2);

    println!("Testing with hardware CS");
    // It is not guaranteed that this is really hardware CS; could just as well be performed by
    // RIOT internally.
    let mut spi_with_hard_cs = spi.into_device(cs).unwrap();
    test_on_device(&mut spi_with_hard_cs);

    println!("Both tests done.");
}

// This is a bit .unwrap()py even though all our devices have infallible SPI (and CS) operations at
// runtime; can't use `let Ok(()) = …;` because the ExclusiveDevice has an Either<Infallible,
// Infallible> type, and I haven't found an easy way to require that the associated error of D is
// uninhabited.
fn test_on_device<D: SpiDevice>(spi: &mut D) {
    println!("Plain transfer in place:");
    let mut buf = [0, 0, 0x12, 0x34];
    println!("Writing {:?}, …", buf);
    spi.transfer_in_place(&mut buf).unwrap();
    println!("read {:?}.", buf);

    println!("Write from flash:");
    // Writing from flash makes a difference eg. on nrf52: That peripheral is DMA only and can not
    // read flash.
    let buf = [0, 0, 0x12, 0x34];
    println!("Writing {:?}.", buf);
    spi.write(&buf).unwrap();

    println!("Performing complex sequence:");
    let writebuf = [0; 300];
    let mut readbuf1 = [0x12, 0x34, 0x56, 0x78];
    let mut readbuf2 = [0x12, 0x34, 0x56, 0x78];
    use embedded_hal::spi::Operation;
    let mut operations = [
        Operation::Write(&writebuf),
        Operation::DelayNs(123),
        Operation::Read(&mut readbuf1),
        Operation::Read(&mut readbuf2),
    ];
    spi.transaction(&mut operations).unwrap();
    println!(
        "Wrote [0; 300], read into {:?} and {:?}",
        readbuf1, readbuf2
    );

    println!("Plain transfer in place:");
    let writebuf = [0, 0];
    let mut readbuf = [0xff; 10];
    spi.transfer(&mut readbuf, &writebuf).unwrap();
    println!(
        "In mixed transfer, wrote [0; 2], and continued reading into {:?}.",
        readbuf
    );
}
