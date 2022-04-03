#![no_std]
#![no_main]

mod rustreus_lib;
use rustreus_lib::*;

use core::panic::PanicInfo;
use desse::{Desse, DesseSized};
struct Foo {
  u: u8,
}

#[repr(C)]
pub struct EncodedResponse {
  pub action: i8,
  pub key_code: u8,
  pub other: i8,
  pub pass_through: i8,
}

fn encode_response(res: Response) -> EncodedResponse {
  match res.action {
    Action::None => EncodedResponse {
      action: 0,
      key_code: 0,
      other: 0,
      pass_through: 1,
    },
    Action::PressKey(key) => EncodedResponse {
      action: 1,
      key_code: key,
      other: res.other,
      pass_through: 1,
    },
    Action::ReleaseKey(key) => EncodedResponse {
      action: 2,
      key_code: key,
      other: res.other,
      pass_through: 1,
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
  key_state_ptr: *const EncodedKeyState,
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

#[no_mangle]
pub extern "C" fn serialize_test(test: *const Test, max_len: u8, out_bytes_ptr: *mut u8) -> u8 {
  let bytes = unsafe { (*test).serialize() };

  let len = bytes.len();
  if len < max_len.into() {
    for (index, &byte) in bytes.iter().enumerate() {
      unsafe {
        *out_bytes_ptr.add(index) = byte;
      }
    }

    len as u8
  } else {
    0
  }
}

#[no_mangle]
pub extern "C" fn deserialize_test(len: u8, bytes_ptr: *const u8, out_test: *mut Test) {
  let bytes = &mut [0, len];

  for (index, byte) in bytes.iter_mut().enumerate() {
    unsafe {
      *byte = *bytes_ptr.add(index);
    }
  }

  let test = Test::deserialize_from(&bytes);
  unsafe {
    *out_test = test;
  }
}
