use evdev::InputEventKind::Key;
use evdev::AttributeSetRef;
use evdev::{uinput::VirtualDeviceBuilder, AttributeSet};
use std::io::{self, BufRead};
use std::sync::mpsc;
use std::thread;
use std::time::Duration;

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

    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let mut count = 0;
        loop {
            let click = rx.try_recv();
            match click {
                Ok(x) => { count+=1; () }
                Err(_) => () //println!("Error receiving click.")
            }
            if count == 1 {
                thread::sleep(Duration::from_millis(500));
                let click2 = rx.try_recv();
                match click2 {
                    Ok(y) => { let i = &[
                        evdev::InputEvent::new(
                            evdev::EventType::KEY, evdev::Key::BTN_RIGHT.code(), 1),
                        evdev::InputEvent::new(
                            evdev::EventType::KEY, evdev::Key::BTN_RIGHT.code(), 0)];
                        let _ = toPlayer.emit(i);

                        println!("Double click."); }

                    Err(_) => { let i = &[
                        evdev::InputEvent::new(
                            evdev::EventType::KEY, evdev::Key::BTN_LEFT.code(), 1),
                        evdev::InputEvent::new(
                            evdev::EventType::KEY, evdev::Key::BTN_LEFT.code(), 0)];
                        let _ = toPlayer.emit(i);

                        println!("Single click."); }
                }
                count = 0;
            }
            thread::sleep(Duration::from_millis(50));
        }
    });
              
    loop {
        for e in s.fetch_events().unwrap() {
            if let Key(k) = e.kind() {
                if k == evdev::Key::BTN_LEFT {
                    let v = e.value();
                    //println!("Value is {v}");
                    if ( v == 1 ) { tx.send(()).unwrap(); }
                }
            }
        }
    }
}
