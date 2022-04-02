static mut STATE: State = State::new();
struct State {
  counter: i32,
}

impl State {
  const fn new() -> State {
    State { counter: 0 }
  }
}

pub unsafe fn update_state() -> i32 {
  STATE.counter += 1;
  STATE.counter
}
