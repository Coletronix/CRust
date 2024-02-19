// simple analog struct for one specific pin.
// TODO: make generic over all valid pins

use msp432p401r_hal as hal;

use core::marker::PhantomData;
use embedded_hal::adc::nb::{Channel, OneShot};
use hal::gpio::porta::P1_1;

struct MyAdc; // 10-bit ADC, with 5 channels

impl<WORD, PIN> OneShot<MyAdc, WORD, PIN> for MyAdc
where
   WORD: From<u16>,
   PIN: Channel<MyAdc, ID=u8>,
{
   type Error = ();

   fn read(&mut self, pin: &mut PIN) -> nb::Result<WORD, Self::Error> {
       let chan = 1 << pin.channel();
       self.power_up();
       let result = self.do_conversion(chan);
       self.power_down();
       Ok(result.into())
   }
}