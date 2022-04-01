#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[no_mangle]

pub extern "C" fn rustreus_handle_message(is_pressed: i8, was_pressed: i8, toggle_state: i8) -> i32{
    if is_pressed == 0{
        123
    }else if is_pressed == 1{
        668
    }else{
        99
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
