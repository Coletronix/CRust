use msp432p401r_hal as hal;

use hal::gpio::{
    porta::{P2_0, P2_1, P2_2},
    *,
};

#[derive(Clone, Copy, Debug)]
pub enum Color {
    Red,
    Green,
    Blue,
    Yellow,
    Cyan,
    Magenta,
    White,
    Other(bool, bool, bool),
    Off,
}

pub struct RgbLed {
    red: P2_0<Output>,
    green: P2_1<Output>,
    blue: P2_2<Output>,
}

impl RgbLed {
    pub fn new(red: P2_0<Output>, green: P2_1<Output>, blue: P2_2<Output>) -> Self {
        RgbLed { red, green, blue }
    }

    pub fn set_color_parts(&mut self, red: bool, green: bool, blue: bool) {
        self.red.set_state(red.into()).unwrap();
        self.green.set_state(green.into()).unwrap();
        self.blue.set_state(blue.into()).unwrap();
    }

    pub fn set_color(&mut self, color: Color) {
        match color {
            Color::Red => self.set_color_parts(true, false, false),
            Color::Green => self.set_color_parts(false, true, false),
            Color::Blue => self.set_color_parts(false, false, true),
            Color::Yellow => self.set_color_parts(true, true, false),
            Color::Cyan => self.set_color_parts(false, true, true),
            Color::Magenta => self.set_color_parts(true, false, true),
            Color::White => self.set_color_parts(true, true, true),
            Color::Other(r, g, b) => self.set_color_parts(r, g, b),
            Color::Off => self.set_color_parts(false, false, false),
        }
    }
}
