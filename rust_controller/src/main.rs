#![no_main]
#![no_std]

use cortex_m_rt::entry;

use heapless::String;
use msp432p401r as pac;
use msp432p401r_hal as hal;
use core::fmt::Write;

use hal::{
    clock::{CsExt, DCOFrequency, MPrescaler, SMPrescaler},
    flash::{FlashExt, FlashWaitStates},
    gpio::*,
    pcm::{PcmExt, VCoreSel},
    serial::{uart::uarta::UART_A0, uart::*},
    watchdog::{Disable, WDTExt},
};
use hardware_drivers::terminal::Terminal;

mod hardware_drivers;

// define custom panic handler
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    unsafe {
        // ok to steal since we are panicking
        let p = pac::Peripherals::steal();
        let mut _watchdog = p.WDT_A.constrain().disable().unwrap();
        let gpio = p.DIO.split();
        let mut led1 = gpio.p1_0.into_output();

        // simple blink to show panic
        loop {
            led1.toggle().unwrap();
            delay(200000);
        }
    }
}

fn delay(count: u32) {
    for _ in 0..count {
        // force the compiler to include this loop and do nothing
        let _ = unsafe { core::ptr::read_volatile(&0) };
    }
}

#[entry]
fn main() -> ! {
    // Take the Peripherals once and only once.
    let p = pac::Peripherals::take().unwrap();

    // Watchdog Config.
    let mut _watchdog = p.WDT_A.constrain().disable().unwrap();

    // PCM Config.
    let _pcm = p
        .PCM
        .constrain() // Setup PCM
        .set_vcore(VCoreSel::DcdcVcore1) // Set DCDC Vcore1 -> 48 MHz Clock
        .freeze();

    // Flash Control Config.
    let _flash_control = p
        .FLCTL
        .constrain() // Setup Flash
        .set_waitstates(FlashWaitStates::_2) // Two wait states -> 48 Mhz Clock
        .freeze();

    let clock =
        p.CS.constrain() // Setup CS
            .mclk_dcosource_selection(DCOFrequency::_48MHz, MPrescaler::DIVM_0) // 48 MHz DCO
            .smclk_prescaler(SMPrescaler::DIVS_4) // 3 MHz SMCLK
            .freeze();

    let gpio = p.DIO.split();

    let uart = UART_A0::from(p.EUSCI_A0)
        .with_pins(
            gpio.p1_2.into_alternate_primary(),
            gpio.p1_3.into_alternate_primary(),
        )
        .with_clock_source(ClockSource::SMCLK)
        .with_baud_rate(9600, &clock)
        .with_bit_order(BitOrder::LsbFirst)
        .with_parity(Parity::None)
        .with_stop_bits(StopBits::One)
        .with_data_bits(DataBits::Eight)
        .init();

    let mut term = Terminal::from(uart);

    let mut led1 = gpio.p1_0.into_output();
    let mut led2 = hardware_drivers::rgb::RgbLed::new(
        gpio.p2_0.into_output(),
        gpio.p2_1.into_output(),
        gpio.p2_2.into_output(),
    );
    led1.set_low().unwrap();

    let mut counter = 0;
    let mut color = hardware_drivers::rgb::Color::Red;
    
    let mut num_str = String::<32>::new();

    loop {
        led2.set_color(color);
        color = match color {
            hardware_drivers::rgb::Color::Red => hardware_drivers::rgb::Color::Green,
            hardware_drivers::rgb::Color::Green => hardware_drivers::rgb::Color::Blue,
            hardware_drivers::rgb::Color::Blue => hardware_drivers::rgb::Color::Red,
            _ => hardware_drivers::rgb::Color::Red,
        };

        
        let _ = write!(num_str, "Counter at: {}\r\n", counter);
        term.send_str(num_str.as_str());
        counter += 1;

        delay(20000);
    }
}
