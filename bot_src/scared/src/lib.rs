#![feature(strict_provenance)]
#![allow(clippy::missing_safety_doc)]

pub use bot_mem::*;

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
