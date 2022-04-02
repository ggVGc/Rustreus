use std::fs::File;
use std::fs::OpenOptions;
use std::io::prelude::*;
use std::io::BufReader;
use std::io::BufWriter;

mod state;
// use state::update_state;

pub fn main() -> std::io::Result<()> {
  let file = File::open("/dev/ttyACM0").unwrap();
  let mut buf_reader = BufReader::new(file);

  loop {
    let msg = {
      let mut line = String::new();
      let _len = buf_reader.read_line(&mut line);
      let line = line.trim();
      if !line.is_empty() {
        Some(line.split(" ").next().unwrap().to_string())
      } else {
        None
      }
    };

    if let Some(msg) = msg {
      match msg.trim().parse() {
        Ok(num) => {
          handle_key_code(num);
        }
        _ => {
          if msg != "." {
            println!("Message: {}", msg);
          }
        }
      }
    }
  }

  fn handle_key_code(key_code: u8) {
    println!("Got number: {}", key_code);
    if key_code == 26 {
      let msg = press_key_msg(8);
      send_msg(msg);
      let msg = release_key_msg(8);
      send_msg(msg);
    }
  }

  fn send_msg(message: Vec<u8>) {
    let f = OpenOptions::new().write(true).open("/dev/ttyACM0").unwrap();
    let mut writer = BufWriter::new(f);
    let bytes = [message.as_slice(), &['\n' as u8]].concat();
    let _ignored = writer.write_all(bytes.as_slice());
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
