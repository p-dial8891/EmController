use evdev::InputEventKind::Key;

fn main() {
    println!("Hello, world!");

    //let d = evdev::Device::open( std::path::Path::new("/dev/input/by-id/usb-ZEPHYR_Zephyr_HID_mouse_sample_2735883958566D65-event-mouse") );
    let d = evdev::Device::open( "/dev/input/by-id/usb-ZEPHYR_Zephyr_HID_mouse_sample_2735883958566D65-event-mouse" );
    let mut s = d.unwrap();
    println!("{s}");
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
