#![no_std]
#![no_main]

mod rustreus_lib;
use core::panic::PanicInfo;

#[no_mangle]

pub extern "C" fn rustreus_handle_message(key_code: u8, is_pressed: i8, was_pressed: i8, toggle_state: i8) -> i32 {
    rustreus_lib::handle_key(key_code, is_pressed, was_pressed, toggle_state)
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
