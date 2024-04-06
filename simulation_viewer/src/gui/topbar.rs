use bevy::ecs::system::Local;
use bevy_egui::EguiContexts;
use eframe::egui;
use bluetooth_serial_port::{scan_devices, BtDevice};
use std::time::Duration;

pub fn update_topbar(mut contexts: EguiContexts, mut devices: Local<Vec<BtDevice>>) {
    egui::TopBottomPanel::top("Top panel").show(contexts.ctx_mut(), |ui| {
        if ui.button("Scan").clicked() {
            println!("Button clicked!");
            *devices = scan_devices(Duration::from_secs(10)).unwrap();
        }
        
        // show all deivces in a dropdown
        let devices_names = devices.iter().map(|device| format!("{:?}", device)).collect::<Vec<String>>();
    
        let mut selected_device = 0;
        ui.horizontal(|ui| {
            ui.label("Select device:");
            ui.selectable_value(&mut selected_device, 0, "None");
            for (i, device) in devices_names.iter().enumerate() {
                ui.selectable_value(&mut selected_device, i + 1, device);
            }
        });
    });
}
