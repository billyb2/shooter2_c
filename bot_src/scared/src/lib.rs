#![feature(strict_provenance)]
#![allow(clippy::missing_safety_doc)]

use std::alloc::{alloc, realloc, Layout};
use std::ptr::{self, null_mut};
use std::ffi::CString;
use once_cell::sync::Lazy;

include!(concat!(env!("OUT_DIR"), "/bot_actions.rs"));

impl MinimalPlayerInfo {
    const fn zero() -> Self {
        Self { pos_x: 0, pos_y: 0, direction: 0.0 }
    }
}

#[used]
static mut NUM_PLAYERS: u8 = 0;
#[used]
static mut PLAYERS: [MinimalPlayerInfo; 4] = [MinimalPlayerInfo::zero(); 4];
#[used]
static mut NUM_PROJECTILES: u16 = 0;
#[used]
static mut PROJECTILES: *mut MinimalProjectileInfo = ptr::null_mut();
#[used]
static mut ERROR: Lazy<CString> = Lazy::new(|| CString::new("Good evening, world").unwrap());

const UP:u8 = 0x00;
const DOWN:u8 = 0xFF;
const LEFT:u8 = 0x00;
const RIGHT:u8 = 0xFF;
const NONE:u8 = 0x80;


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


unsafe fn actions() -> BotActions {
    let mut actions = BotActions {
        movement_direction_left_right: NONE,
        movement_direction_up_down: NONE,
        direction: 0.0,
        shooting_and_ability: 0x00,

    };

    if NUM_PROJECTILES > 0 {
        actions.movement_direction_left_right = RIGHT;

    }

    actions

}

#[no_mangle]
pub unsafe fn actions_to_int() -> i64 {
    let mut int_bytes = [0_u8; 8];
    let bot_actions = actions();

    int_bytes[0] = bot_actions.movement_direction_up_down;
    int_bytes[1] = bot_actions.movement_direction_left_right;
    int_bytes[2..6].copy_from_slice(&bot_actions.direction.to_le_bytes());
    int_bytes[6] = bot_actions.shooting_and_ability;

    i64::from_le_bytes(int_bytes)

}
