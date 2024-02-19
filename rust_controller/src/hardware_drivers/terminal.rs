#[allow(unused)]
#[allow(dead_code)]
use heapless::String;
use msp432p401r_hal::serial::uart::{
    uarta::{UART_A0, UART_A1, UART_A2, UART_A3},
    Enabled,
};

trait WritableReadable {
    fn write(&mut self, data: u8);
    fn read(&mut self) -> u8;
    fn data_available(&mut self) -> bool;
}

macro_rules! impl_writable_readable {
    ($eusci_ax:ty) => {
        impl WritableReadable for $eusci_ax {
            fn write(&mut self, data: u8) {
                <$eusci_ax>::write(self, data);
            }

            fn read(&mut self) -> u8 {
                <$eusci_ax>::read(self)
            }

            fn data_available(&mut self) -> bool {
                <$eusci_ax>::data_available(self)
            }
        }
    };
}

impl_writable_readable!(UART_A0<Enabled>);
impl_writable_readable!(UART_A1<Enabled>);
impl_writable_readable!(UART_A2<Enabled>);
impl_writable_readable!(UART_A3<Enabled>);

#[derive(Debug)]
pub struct InvalidUnicodeChar;

pub enum ReturnReason {
    GotCharSuccess,
    Newline,
    MaxLengthReached,
}

// terminal struct that holds an instance of anything that implements WritableReadable
pub struct Terminal<T: WritableReadable> {
    uart: T,
}

impl From<UART_A0<Enabled>> for Terminal<UART_A0<Enabled>> {
    fn from(uart: UART_A0<Enabled>) -> Self {
        Terminal { uart }
    }
}

impl From<UART_A1<Enabled>> for Terminal<UART_A1<Enabled>> {
    fn from(uart: UART_A1<Enabled>) -> Self {
        Terminal { uart }
    }
}

impl From<UART_A2<Enabled>> for Terminal<UART_A2<Enabled>> {
    fn from(uart: UART_A2<Enabled>) -> Self {
        Terminal { uart }
    }
}

impl From<UART_A3<Enabled>> for Terminal<UART_A3<Enabled>> {
    fn from(uart: UART_A3<Enabled>) -> Self {
        Terminal { uart }
    }
}

// use macro to generate the same impl for UART_A1, UART_A2, and UART_A3
macro_rules! impl_terminal {
    ($uart:ty) => {
        impl Terminal<$uart> {
            pub fn send_char(&mut self, codepoint: char) {
                match codepoint.len_utf8() {
                    1 => self.uart.write(codepoint as u8),
                    2 => {
                        let mut bytes = [0, 0];
                        codepoint.encode_utf8(&mut bytes);
                        self.uart.write(bytes[0]);
                        self.uart.write(bytes[1]);
                    }
                    3 => {
                        let mut bytes = [0, 0, 0];
                        codepoint.encode_utf8(&mut bytes);
                        self.uart.write(bytes[0]);
                        self.uart.write(bytes[1]);
                        self.uart.write(bytes[2]);
                    }
                    4 => {
                        let mut bytes = [0, 0, 0, 0];
                        codepoint.encode_utf8(&mut bytes);
                        self.uart.write(bytes[0]);
                        self.uart.write(bytes[1]);
                        self.uart.write(bytes[2]);
                        self.uart.write(bytes[3]);
                    }
                    _ => (),
                };
            }

            pub fn send_str(&mut self, string: &str) {
                for codepoint in string.chars() {
                    self.send_char(codepoint);
                }
            }

            pub fn data_available(&mut self) -> bool {
                self.uart.data_available()
            }

            pub fn get_char(&mut self) -> Result<char, InvalidUnicodeChar> {
                let first_byte = self.uart.read();

                let mut bytes = [first_byte, 0, 0, 0];
                let additional_bytes = match first_byte {
                    0x00..=0x7F => 0,
                    0xC2..=0xDF => 1,
                    0xE0..=0xEF => 2,
                    0xF0..=0xF4 => 3,
                    _ => return Err(InvalidUnicodeChar),
                };

                for i in 1..=additional_bytes {
                    let byte = self.uart.read();
                    if byte & 0xC0 != 0x80 {
                        return Err(InvalidUnicodeChar);
                    }
                    bytes[i] = byte;
                }

                let code_point = match additional_bytes {
                    0 => bytes[0] as u32,
                    1 => (((bytes[0] & 0x1F) as u32) << 6) | ((bytes[1] & 0x3F) as u32),
                    2 => {
                        (((bytes[0] & 0x0F) as u32) << 12)
                            | (((bytes[1] & 0x3F) as u32) << 6)
                            | ((bytes[2] & 0x3F) as u32)
                    }
                    3 => {
                        (((bytes[0] & 0x07) as u32) << 18)
                            | (((bytes[1] & 0x3F) as u32) << 12)
                            | (((bytes[2] & 0x3F) as u32) << 6)
                            | ((bytes[3] & 0x3F) as u32)
                    }
                    _ => return Err(InvalidUnicodeChar),
                };

                if (code_point >= 0xD800 && code_point <= 0xDFFF) || code_point > 0x10FFFF {
                    return Err(InvalidUnicodeChar);
                }

                // SAFETY: code_point is guaranteed to be a valid unicode code point at this point
                Ok(unsafe { core::mem::transmute(code_point) })
            }

            pub fn read_strn<const N: usize>(&mut self) -> Result<String<N>, InvalidUnicodeChar> {
                let mut buf: String<N> = String::new();
                while let Ok(ReturnReason::GotCharSuccess) = self.read_strn_char(&mut buf) {}
                Ok(buf)
            }

            pub fn read_strn_char<const N: usize>(
                &mut self,
                buf: &mut String<N>,
            ) -> Result<ReturnReason, InvalidUnicodeChar> {
                let codepoint = self.get_char()?;
                self.send_char(codepoint);

                if codepoint == '\n' || codepoint == '\r' {
                    return Ok(ReturnReason::Newline);
                }

                // check backspace
                if codepoint == '\x7f' {
                    buf.pop();
                } else {
                    let _ = buf.push(codepoint);
                }

                if buf.len() == N {
                    Ok(ReturnReason::MaxLengthReached)
                } else {
                    Ok(ReturnReason::GotCharSuccess)
                }
            }
        }
    };
}

impl_terminal!(UART_A0<Enabled>);
impl_terminal!(UART_A1<Enabled>);
impl_terminal!(UART_A2<Enabled>);
impl_terminal!(UART_A3<Enabled>);
