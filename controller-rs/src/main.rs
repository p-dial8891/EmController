use evdev::InputEventKind::Key;
use evdev::AttributeSetRef;
use evdev::{uinput::VirtualDeviceBuilder, AttributeSet};
use std::io::{self, BufRead};

fn main() {
    println!("Hello, world!");

    //let d = evdev::Device::open( std::path::Path::new("/dev/input/by-id/usb-ZEPHYR_Zephyr_HID_mouse_sample_2735883958566D65-event-mouse") );
    let d = evdev::Device::open( "/dev/input/by-id/usb-ZEPHYR_Zephyr_HID_mouse_sample_2735883958566D65-event-mouse" );
    let mut s = d.unwrap();
    println!("{s}");
    let mut toPlayer = VirtualDeviceBuilder::new().unwrap()
        .name("fake-mouse")
        .with_keys(&AttributeSet::from_iter([
            evdev::Key::BTN_LEFT,
            evdev::Key::BTN_RIGHT])).unwrap()
        .build()
        .unwrap();

    let stdin = io::stdin();
    let mut iter = stdin.lock().lines();
    let line1 = iter.next().unwrap().unwrap();

    let i = &[evdev::InputEvent::new(
                evdev::EventType::KEY, evdev::Key::BTN_LEFT.code(), 1),
              evdev::InputEvent::new(
                evdev::EventType::KEY, evdev::Key::BTN_LEFT.code(), 0)];
    let _ = toPlayer.emit(i);
              
    loop {
        for e in s.fetch_events().unwrap() {
            if let Key(k) = e.kind() {
                if k == evdev::Key::BTN_LEFT {
                    let v = e.value();
                    println!("Value is {v}");
                }
            }
        }
    }
}
