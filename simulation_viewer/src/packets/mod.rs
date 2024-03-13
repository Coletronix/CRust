pub mod motor_packet;
use crate::serial_interface::SerialInterface;

pub trait Packet {
    fn get_type(&self) -> u8;
    fn to_bytes(&self) -> Vec<u8>;
    fn from_bytes(bytes: Vec<u8>) -> Self;
}

pub trait Transmittable {
    fn send(&self, ser: &dyn SerialInterface);
    fn receive(&self, ser: &dyn SerialInterface) -> Vec<u8>;
}

// blanket implementation to send on all packets
impl<T: Packet> SerialInterface for T {
    fn send(&self, ser: &dyn SerialInterface) {
        ser.send(self.to_bytes());
    }
    fn receive(&self, ser: &dyn SerialInterface) -> Vec<u8> {
        ser.receive()
    }
    
}