use bevy::prelude::*;
mod gui;

pub fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        .add_plugins(gui::GUIPlugin)
        .run();
}
