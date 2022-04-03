use std::fs::File;
use std::fs::OpenOptions;
use std::io::prelude::*;
use std::io::BufReader;
use std::io::BufWriter;

mod lib;
mod state;
// use state::update_state;
use desse::{Desse, DesseSized};
use lib::*;

const KEY_STATE_SIZE: usize = 4;

pub fn main() -> std::io::Result<()> {
  let file = File::open("/dev/ttyACM0").unwrap();
  let mut buf_reader = BufReader::new(file);

  loop {
    let msg = {
      let mut line = String::new();
      let _len = buf_reader.read_line(&mut line).unwrap();
      let line = line.trim();
      let spl: Vec<String> = line.split(" ").map(|s| s.to_string()).collect();
      if spl.len() > 1 {
        println!("Received: {:?}", spl);
      }
      if spl.len() == 2 {
        if spl[0] == "rustreus.key_state" {
          let bytes: Vec<u8> = hex::decode(spl[1].as_bytes()).unwrap();
          if bytes.len() == KEY_STATE_SIZE {
            let mut byte_arr: [u8; KEY_STATE_SIZE] = [0; KEY_STATE_SIZE];
            for (index, &byte) in bytes.iter().enumerate() {
              byte_arr[index] = byte;
            }

            Some(KeyState::deserialize_from(&byte_arr))
          } else {
            None
          }
        } else {
          None
        }
      } else {
        None
      }
    };

    if let Some(key_state) = msg {
      handle_key_state(key_state)
    }
  }

  fn handle_key_state(key_state: KeyState) {
    println!("Got number: {}", key_state.key_code);
    let header: &[u8] = b"rustreus.cmd ";
    let response = Response {
      action: if key_state.is_pressed {
        Action::PressKey(key_state.key_code)
      } else {
        Action::ReleaseKey(key_state.key_code)
      },
      other: 123,
    };

    let serialized = Response::serialize(&response);
    let bytes = &serialized;
    let message = [header, &[bytes.len() as u8], bytes].concat();
    send_msg(message);

    // if key_code == 26 {
    //   let msg = press_key_msg(8);
    //   send_msg(msg);
    //   let msg = release_key_msg(8);
    //   send_msg(msg);
    // }
  }

  fn send_msg(message: Vec<u8>) {
    let f = OpenOptions::new().write(true).open("/dev/ttyACM0").unwrap();
    let mut writer = BufWriter::new(f);
    let bytes = [message.as_slice(), b".\n"].concat();
    let _ignored = writer.write_all(bytes.as_slice());
    writer.flush();
  }

  fn press_key_msg(key: u8) -> Vec<u8> {
    let header: &[u8] = b"rustreus.releaseKey ";
    [header, &[key]].concat()
  }

  fn release_key_msg(key: u8) -> Vec<u8> {
    let header: &[u8] = b"rustreus.releaseKey ";
    [header, &[key]].concat()
  }
}
