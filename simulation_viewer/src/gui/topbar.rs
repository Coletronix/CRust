use bevy_egui::EguiContexts;
use eframe::egui;

enum DemoEnum {
    A(i32),
    B(f32),
    C([i32; 3]),
}

struct MotorValues {
    left: i32,
    right: i32,
}

enum DataPacket {
    Camera([u16; 128]),
    // motor can have left and right data
    Motor(MotorValues),
}

enum IntFloat {
    Int(i32),
    Float(f32),
}

pub fn update_topbar(mut contexts: EguiContexts) {
    egui::TopBottomPanel::top("Top panel").show(contexts.ctx_mut(), |ui| {
        if ui.button("bruh").clicked() {
            println!("Button clicked!");
        }
    });
}
