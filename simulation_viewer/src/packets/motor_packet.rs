use super::Packet;

struct MotorCommands {
    left: f32,
    right: f32,
}

impl MotorCommands {
    pub fn new(left: f32, right: f32) -> MotorCommands {
        MotorCommands { left, right }
    }
}

impl Packet for MotorCommands {
    fn get_type(&self) -> u8 {
        0x01
    }
    
    fn to_bytes(&self) -> Vec<u8> {
        let mut bytes = vec![];
        bytes.push(self.get_type());
        bytes.extend_from_slice(&self.left.to_be_bytes());
        bytes.extend_from_slice(&self.right.to_be_bytes());
        bytes
    }
    
    fn from_bytes(bytes: Vec<u8>) -> MotorCommands {
        let left = f32::from_be_bytes([bytes[1], bytes[2], bytes[3], bytes[4]]);
        let right = f32::from_be_bytes([bytes[5], bytes[6], bytes[7], bytes[8]]);
        MotorCommands::new(left, right)
    }
}