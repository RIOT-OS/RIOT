// Copyright (C) 2020 Christian Amsüss
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use riot_wrappers::error::NumericError;
use riot_wrappers::riot_main;
use riot_wrappers::println;

extern crate rust_riotmodules;

use riot_wrappers::vfs::{Dir, File, FileMode, Mount, SeekFrom};

riot_main!(main);

const BUF_SIZE: usize = 100;

fn main() {
    println!("Hello Rust!");

    let mut mount_points = Mount::all();
    let mut mp = mount_points.next().expect("no mount point found");
    let dir = Dir::open("/nvm0");
    match dir {
        Ok(dir) => { println!("Opened dir: {:?}", dir.count()) }
        Err(_) => { println!("error opening dir"); }
    }
    println!("Mount point: {:?}, {} files", mp.mount_point(), mp.root_dir().count());

    //let mut rw_file = File::open_with_mode("/nvm0/test", FileMode::ReadWrite(false));
    let mut rw_file = File::open_with_mode("/nvm0/test", FileMode::ReadWrite(false));
    //let mut rw_file = File::open_rw("/nvm0/test123");
    match rw_file {
        Ok(mut f) => {
            println!("File opened with R/W");
            let data = "hello".as_bytes();
            f.write(&data).expect("Error writing to file");
            println!("Write success!");

            f.seek(SeekFrom::Start(0)).expect("seek failed");
            let mut content: [u8; BUF_SIZE] = [0; BUF_SIZE];
            f.read(&mut content).expect("Error reading from file");
            println!("Contents: {:?}", content);
            drop(f);
        }
        Err(err) => {
            println!("Error opening file: {:?}", err);
        }
    }
    return;
    
    let mut f = File::open_rw("/nvm0/write_only");
    match f {
        Ok(mut f) => {
            println!("File opened!");
            let data = "hello".as_bytes();
            f.write(&data).expect("Error writing to file");
            println!("Contents written!");

            f.seek(SeekFrom::Start(0)).expect("seek failed");
            let mut content: [u8; BUF_SIZE] = [0; BUF_SIZE];
            f.read(&mut content).expect("Error reading from file");
            println!("Contents: {:?}", content);
            drop(f);
        }
        Err(err) => {
            println!("Error opening file: {:?}", err);
        }
    }
}
