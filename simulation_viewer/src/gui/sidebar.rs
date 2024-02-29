use bevy_egui::EguiContexts;
use eframe::egui;

/// System that updates the sidebar GUI
pub fn update_sidebar(mut contexts: EguiContexts) {
    egui::SidePanel::left("Left panel").show(contexts.ctx_mut(), |ui| {
        ui.label("This is the left panel");
    });
}
