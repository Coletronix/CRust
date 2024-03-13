
pub(crate) trait SerialInterface {
    fn send(&self, data: Vec<u8>);
    fn receive(&self) -> Vec<u8>;
}
