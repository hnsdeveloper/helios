#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[panic_handler]
fn no_panic(_info : &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
extern "C" 
fn kmain() {
    ()
}
