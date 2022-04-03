#![no_std]
#![no_main]

mod rustreus_lib;
use rustreus_lib::{Action, KeyState, Response, Toggled};

use core::panic::PanicInfo;

#[repr(C)]
pub struct EncodedResponse {
  pub action: i8,
  pub key_code: u8,
  pub other: i8,
}

fn encode_response(res: Response) -> EncodedResponse {
  match res.action {
    Action::PressKey(key) => EncodedResponse {
      action: 1,
      key_code: key,
      other: res.other,
    },
    Action::ReleaseKey(key) => EncodedResponse {
      action: 2,
      key_code: key,
      other: res.other,
    },
  }
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct EncodedKeyState {
  pub key_code: u8,
  pub is_pressed: i8,
  pub was_pressed: i8,
  pub toggle: i8,
}

fn decode_key_state(state: EncodedKeyState) -> KeyState {
  KeyState {
    key_code: state.key_code,
    is_pressed: state.is_pressed > 0,
    was_pressed: state.was_pressed > 0,
    toggle: match state.toggle {
      1 => Toggled::Off,
      2 => Toggled::On,
      _ => Toggled::Unknown,
    },
  }
}

#[no_mangle]
pub extern "C" fn rustreus_handle_message(
  key_state_ptr: *mut EncodedKeyState,
  res_ptr: *mut EncodedResponse,
) -> () {
  unsafe {
    let key_state = decode_key_state(*key_state_ptr);
    let res = rustreus_lib::handle_key(key_state);
    (*res_ptr) = encode_response(res);
  }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
  loop {}
}
