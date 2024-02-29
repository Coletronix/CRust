use bevy_egui::EguiContexts;
use eframe::egui;

pub fn update_topbar(mut contexts: EguiContexts) {
    egui::TopBottomPanel::top("Top panel").show(contexts.ctx_mut(), |ui| {
        if ui.button("bruh").clicked() {
            println!("Button clicked!");
        }
    });
}
