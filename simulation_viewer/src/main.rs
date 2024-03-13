use bevy::prelude::*;
mod gui;
mod packetextraction;
mod packets;
mod serial_interface;

pub fn main() {
    App::new()
        .add_plugins(DefaultPlugins)
        .add_plugins(gui::GUIPlugin)
        .run();
}
