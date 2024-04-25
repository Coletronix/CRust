use std::error::Error;
use std::ptr::read;

use bluest::{Adapter, Uuid};
use futures_lite::StreamExt;
use tracing::info;
use tracing::metadata::LevelFilter;
use tracing_subscriber::prelude::*;
use tracing_subscriber::{fmt, EnvFilter};

const READWRITE_SERVICE: Uuid = Uuid::from_u128(0x0000ffe0_0000_1000_8000_00805f9b34fb);
const READWRITE_CHARACTERISTIC: Uuid = Uuid::from_u128(0x0000ffe1_0000_1000_8000_00805f9b34fb);

const DEVICE_NAME: &str = "CAR11";

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

    let adapter = Adapter::default().await.ok_or("Bluetooth adapter not found")?;
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
            info!("found device: {}", device_name);
            adapter.connect_device(&device).await?;
            info!("connected to device: {}", device_name);
            
            // enable readwrite service and characteristic
            let rw_services = device.discover_services_with_uuid(READWRITE_SERVICE).await?;
            let rw_s = rw_services.first().unwrap();
            let rw_chars = rw_s.discover_characteristics_with_uuid(READWRITE_CHARACTERISTIC).await?;
            let rw_char = rw_chars.first().unwrap();
            info!("discovered readwrite char: {:?}", rw_char);
            
            info!("char properties: {:?}", rw_char.properties().await?);
            
            // Forever print out whatever is received
            loop {
                // let rw_data = rw_char.read().await?;
                // info!("read data: {:?}", rw_data);
                rw_char.write_without_response(b"Hello").await?;
                // delay for 1 second
                tokio::time::sleep(tokio::time::Duration::from_secs(1)).await;
            }
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
