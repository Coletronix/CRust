use serialport::{available_ports, SerialPortType};

// create a function that either returns a list of availible ports and then another functoin that would take results one of the ports that would construct the seiralport

pub fn list_ports() -> Vec<String> {
    let mut ports_v = Vec::new();

    match available_ports() {
        Ok(ports) => {
            match ports.len() {
                0 => println!("No ports found."),
                1 => println!("Found 1 port:"),
                n => println!("Found {} ports:", n),
            };
            for p in ports {
                println!("  {}", p.port_name);
                match p.port_type {
                    SerialPortType::UsbPort(_info) => {
                        ports_v.push(p.port_name);
                    },
                    _ => () // default case
                }
            }
        }
        Err(e) => {
            eprintln!("{:?}", e);
            eprintln!("Error listing serial ports");
        }
    }
    return ports_v;
}