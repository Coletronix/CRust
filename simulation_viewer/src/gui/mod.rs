// GUI plugin for use with bevy

use bevy::prelude::*;
use bevy_egui::EguiPlugin;
mod sidebar;
mod topbar;

pub struct GUIPlugin;

impl Plugin for GUIPlugin {
    fn build(&self, app: &mut App) {
        app.add_plugins(EguiPlugin).add_systems(
            Update,
            (
                topbar::update_topbar,
                sidebar::update_sidebar.after(topbar::update_topbar),
            ),
        );
    }
}
