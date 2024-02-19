use std::io::{self, Write};
use std::time::Duration;

fn main() {
    let port_name = "/dev/ttyACM0";
    let baud_rate = 9600;

    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(10))
        .open();

    match port {
        Ok(mut port) => {
            let mut serial_buf: Vec<u8> = vec![0; 1];
            println!("Receiving data on {} at {} baud:", &port_name, &baud_rate);
            loop {
                // read in a byte
                if let Ok(_) = port.read_exact(&mut serial_buf) {
                    print!("{}", serial_buf[0] as char);
                    io::stdout().flush().unwrap();
                }
            }
        }
        Err(e) => {
            eprintln!("Failed to open \"{}\". Error: {}", port_name, e);
            ::std::process::exit(1);
        }
    }
}