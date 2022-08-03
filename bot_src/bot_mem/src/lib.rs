#![feature(strict_provenance)]
#![allow(non_upper_case_globals)]
#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(clippy::missing_safety_doc)]

use core::ptr;
use std::alloc::{alloc, realloc, Layout};
use std::ffi::CString;
use once_cell::unsync::Lazy;

pub const UP: u8 = 0x00;
pub const DOWN: u8 = 0xFF;
pub const LEFT: u8 = 0x00;
pub const RIGHT: u8 = 0xFF;
pub const NONE: u8 = 0x80;

include!(concat!(env!("OUT_DIR"), "/bot_actions.rs"));

impl MinimalPlayerInfo {
    const fn zero() -> Self {
        Self { pos_x: 0, pos_y: 0, direction: 0.0 }
    }
}

#[used]
pub static mut NUM_PLAYERS: u8 = 0;
#[used]
pub static mut PLAYERS: [MinimalPlayerInfo; 4] = [MinimalPlayerInfo::zero(); 4];
#[used]
pub static mut NUM_PROJECTILES: u16 = 0;
#[used]
pub static mut PROJECTILES: *mut MinimalProjectileInfo = ptr::null_mut();
#[used]
pub static mut ERROR: Lazy<CString> = Lazy::new(|| CString::new("Good evening, world").unwrap());

#[no_mangle]
pub unsafe fn get_num_projectiles_ptr() -> i32 {
    (&mut NUM_PROJECTILES) as *mut u16 as i32

}

#[no_mangle]
pub unsafe fn get_projectiles_ptr() -> i32 {
    PROJECTILES.addr() as i32

}

#[no_mangle]
pub unsafe fn get_projectiles_ptr_ptr() -> i32 {
    let ptr: *mut *mut MinimalProjectileInfo = &mut PROJECTILES;
    ptr.addr() as i32

}

#[no_mangle]
pub unsafe fn get_num_players_ptr() -> i32 {
    (&mut NUM_PLAYERS) as *mut u8 as i32

}


#[no_mangle]
pub unsafe fn get_players_ptr() -> i32 {
    (&mut PLAYERS) as *mut MinimalPlayerInfo as i32

}


#[no_mangle]
pub unsafe fn my_realloc(addr_of_ptr: i32, old_size: i32, size: i32) {
    let ptr_to_list = addr_of_ptr as *mut *mut u8;

    *ptr_to_list = match (*ptr_to_list).is_null() {
        true => alloc(Layout::array::<u8>(size as usize).unwrap()),
        false => realloc(*ptr_to_list, Layout::array::<u8>(old_size as usize).unwrap(), size as usize),
    };

}

