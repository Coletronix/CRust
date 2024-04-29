use std::error::Error;
use std::io::{stdin, Read};
use std::ptr::read;
use std::sync::Arc;
use std::thread;
use tokio::sync::mpsc::{self, Receiver};
use tokio::sync::Mutex;

use bluest::{Adapter, Uuid};
use futures_lite::StreamExt;
use tracing::info;
use tracing::metadata::LevelFilter;
use tracing_subscriber::prelude::*;
use tracing_subscriber::{fmt, EnvFilter};
use tty_read::{ReaderOptions, TermReader};

const READWRITE_SERVICE: Uuid = Uuid::from_u128(0x0000ffe0_0000_1000_8000_00805f9b34fb);
const READWRITE_CHARACTERISTIC: Uuid = Uuid::from_u128(0x0000ffe1_0000_1000_8000_00805f9b34fb);

const DEVICE_NAME: &str = "CAR11";
// const DEVICE_NAME: &str = "CAR_@21";

async fn spawn_stdin_channel() -> Receiver<String> {

    let (tx, rx) = mpsc::channel::<String>(32);
    tokio::spawn(async move {
        // Configure reader options
        let options = ReaderOptions::default();

        // Open a reader
        let reader = TermReader::open_stdin(&options)
            .expect("failed to open stdin reader");

        // Read 5 bytes
        loop {
            if let Ok(input) = reader.read_bytes(1) {
                tx.send(input.iter().map(|&x| x as char).collect()).await.unwrap();
            }

            // let mut buffer = String::new();
            // let mut buffer = [0; 1];
            // stdin().read(&mut buffer).unwrap();
            // // if not newline
            // if buffer[0] != 10 {
            //     tx.send(buffer.iter().map(|&x| x as char).collect()).await.unwrap();
            // }
        }
    });
    rx
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    
    tracing_subscriber::registry()
        .with(fmt::layer())
        .with(
            EnvFilter::builder()
                .with_default_directive(LevelFilter::INFO.into())
                .from_env_lossy(),
        )
        .init();

    let adapter = Adapter::default()
        .await
        .ok_or("Bluetooth adapter not found")?;
    adapter.wait_available().await?;

    adapter.connected_devices().await.iter().for_each(|device| {
        info!("Connected device: {:?}", device);
    });

    info!("starting scan");
    let mut scan = adapter.scan(&[]).await?;
    info!("scan started");
    while let Some(discovered_device) = scan.next().await {
        let device = discovered_device.device;
        let device_name = device.name();
        let device_name = device_name.as_deref().unwrap_or("(unknown)");
        if device_name == DEVICE_NAME {
            info!("found device: {} with ID: {:?}", device_name, device.id());
            adapter.connect_device(&device).await?;
            info!("connected to device: {}", device_name);

            // enable readwrite service and characteristic
            // let rw_services = device.discover_services_with_uuid(READWRITE_SERVICE).await?;
            let rw_service = match device
                .discover_services_with_uuid(READWRITE_SERVICE)
                .await?
                .get(0)
            {
                Some(service) => service.clone(),
                None => return Err("service not found".into()),
            };

            let rw_chars = rw_service.characteristics().await?;
            
            let rw_char = rw_chars
                .iter()
                .find(|x| x.uuid() == READWRITE_CHARACTERISTIC)
                .ok_or("readwrite characteristic not found")?;
            
            info!("discovered readwrite char: {:?}", rw_char);

            info!("char properties: {:?}", rw_char.properties().await?);

            
            // start a read async task and a write async task

            // Assuming `rw_char` is wrapped in an Arc and Mutex for shared ownership
            let rw_char_read = Arc::new(Mutex::new(rw_char.clone()));

            let read_handle = rw_char_read.clone();
            tokio::spawn(async move {
                let read_stream = read_handle.lock().await;
                let mut read_stream = read_stream.notify().await.unwrap();
                while let Some(Ok(data)) = read_stream.next().await {
                    print!("{}", data.iter().map(|&x| x as char).collect::<String>());
                    // better way
                    // print!("{}", String::from_utf8_lossy(&data));
                }
            });
            
            let mut stdin_channel = spawn_stdin_channel().await;
            
            let rw_char_write = Arc::new(Mutex::new(rw_char.clone()));

            let write_handle = rw_char_write.clone();
            tokio::spawn(async move {
                let mut stdin = stdin();
                let mut buffer = [0; 1024]; // You can adjust the buffer size as needed
                loop {
                    match stdin_channel.try_recv() {
                        Ok(mut data) => {
                            data.make_ascii_lowercase();
                            let mut write_stream = write_handle.lock().await;
                            write_stream.write_without_response(data.as_bytes()).await.unwrap();
                        }
                        Err(_) => {}
                    }
                    // match stdin.read(&mut buffer) {
                    //     Ok(n) => {
                    //         if n > 0 {
                    //             let mut write_stream = write_handle.lock().await;
                    //             write_stream.write_without_response(&buffer[..n]).await.unwrap();
                    //         }
                    //     }
                    //     Err(ref e) if e.kind() == std::io::ErrorKind::WouldBlock => {
                    //         println!("Nothing available to read from stdin.");
                    //     }
                    //     Err(e) => {
                    //         eprintln!("Error reading from stdin: {}", e);
                    //     }
                    // }
                }
            });
            
            
            // let mut rw_char_write = rw_char.clone();


            // Forever print out whatever is received
            // loop {
            //     let data = read_stream.next().await.unwrap()?;
            //     print!("{}", data.iter().map(|&x| x as char).collect::<String>());
                
                // check if stdin has any data, and if so send it
            /*     let mut buffer = [0; 1024]; // You can adjust the buffer size as needed
                match stdin().read(&mut buffer) {
                    Ok(n) => {
                        if n > 0 {
                            rw_char.write_without_response(&buffer[..n]).await?;
                        }
                    }
                    Err(ref e) if e.kind() == std::io::ErrorKind::WouldBlock => {
                        println!("Nothing available to read from stdin.");
                    }
                    Err(e) => {
                        eprintln!("Error reading from stdin: {}", e);
                    }
                } */
            // }
            
        }
        // info!(
        //     "{}{}: {:?}",
        //     discovered_device.device.name().as_deref().unwrap_or("(unknown)"),
        //     discovered_device
        //         .rssi
        //         .map(|x| format!(" ({}dBm)", x))
        //         .unwrap_or_default(),
        //     discovered_device.adv_data.services
        // );
    }

    Ok(())
}
