#![allow(clippy::missing_safety_doc)]
#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]

mod mem;
mod math;

use nanorand::Rng;
use std::mem::MaybeUninit;

use math::*;
pub use mem::*;

const GAME_MODE_NAME: &str = "Capture the Flag";

struct Flag {
    pos_x: f32,
    pos_y: f32,
    default_pos_x: f32,
    default_pos_y: f32,
    held_by: Option<&'static MinimalPlayerInfo>,
    team: &'static mut MinimalTeamInfo,

}

impl AsAABB for Flag {
    fn as_aabb(&self) -> AxisAlignedBoundingBox {
        AxisAlignedBoundingBox {
            pos_x: self.pos_x,
            pos_y: self.pos_y,
            width: 50.0,
            height: 30.0,

        }
        
    }

}

impl AsAABB for MinimalPlayerInfo {
    fn as_aabb(&self) -> AxisAlignedBoundingBox {
        AxisAlignedBoundingBox {
            pos_x: self.pos_x,
            pos_y: self.pos_y,
            width: PLAYER_SIZE as f32,
            height: PLAYER_SIZE as f32,

        }
    }

}

static mut FLAGS: [MaybeUninit<Flag>; 2] = [MaybeUninit::uninit(), MaybeUninit::uninit()];

static mut NUM_TEAMS: u32 = 2;

#[no_mangle]
pub unsafe fn init_game_mode() {
    for (i, (flag, team)) in FLAGS.iter_mut().zip(TEAMS.iter_mut()).enumerate() {
        //team.id = 100 + i as u64;
        team.id = RNG.generate();
        team.color = match i == 0 {
            true => 0xFF0000FF,
            false => 0xFFFF0000,
        };

        let pos_x = (MAP.width - 50.0) * i as f32;
        let pos_y = MAP.height / 2.0;

        flag.write(Flag {
            pos_x,
            pos_y,
            default_pos_x: pos_x,
            default_pos_y: pos_y,
            team,
            held_by: None,

        });

    }

}

#[no_mangle]
pub unsafe fn winning_score() -> u64 {
    5

}

#[no_mangle]
pub unsafe fn add_player_to_team() -> u32 {
    // Sort by whichever team has the lowest number of players
    let team = match TEAMS[0].num_players < TEAMS[1].num_players {
        true => &mut TEAMS[0],
        false => &mut TEAMS[1],

    };
    
    team.players[team.num_players as usize] = PLAYER_TO_BE_ADDED;
    team.num_players += 1;

    PLAYER_TO_BE_ADDED.team_id = team.id;

    true as u32

}

unsafe fn draw_flags() {
    for (i, (flag, obj)) in FLAGS.iter().zip(DRAWABLE_OBJECTS.iter_mut()).enumerate() {
        let flag = flag.assume_init_ref();

        obj.pos_x = flag.pos_x;
        obj.pos_y = flag.pos_y;
        obj.width = 50.0;
        obj.height = 30.0;
        obj.shape = Shape_DrawableRectangle;

        if i == 0 {
            obj.color = 0xFF0000CC;

        } else {
            obj.color = 0xFFCC0000;

        }

    }

}

#[no_mangle]
pub unsafe fn calculate_scores() -> u32 {
    for flag in FLAGS.iter_mut() {
        let flag = flag.assume_init_mut();

        if let Some(player) = flag.held_by {
            if (flag.default_pos_x == 0.0 && player.pos_x >= MAP.width - PLAYER_SIZE as f32) || (flag.default_pos_x == MAP.width - 50.0 && player.pos_x <= PLAYER_SIZE as f32) {
                flag.team.score += 1;
                
                flag.pos_x = flag.default_pos_x;
                flag.pos_y = flag.default_pos_y;
                flag.held_by = None;

            }

        }

    }

    // Check if a team has won    
    let winning_team = TEAMS.iter().find(|t| t.score >= winning_score());

    if let Some(winning_team) = winning_team {
        WINNING_TEAM = *winning_team;
        true as u32

    } else {
        false as u32

    }


}

#[no_mangle]
pub unsafe fn set_player_stats() {
    draw_flags();

    for flag in FLAGS.iter_mut() {
        let flag = flag.assume_init_mut();
        // If the flag is being held by a player, just set the flag's position to the player's
        // position, UNLESS the player is using their ability or they die
        if let Some(player) = flag.held_by {
            if player.using_ability == false as u16 && player.health > 0 {
                flag.pos_x = player.pos_x;
                flag.pos_y = player.pos_y;

            } else {
                flag.held_by = None;

            }

        } else {
            // Find the first player in collision with the flag that's not using their ability and
            // is alive
            let player = TEAMS.iter().flat_map(|t| t.players[0..(t.num_players as usize)].iter()).find(|p| (p.using_ability == false as u16) && p.health > 0 && aabb_collision(&**p, flag));

            if let Some(player) = player {
                // If a player is colliding with the flag, and the flag is of the opposite team,
                // have them start holding it
                if player.team_id != flag.team.id {
                    flag.pos_x = player.pos_x;
                    flag.pos_y = player.pos_y;

                    flag.held_by = Some(player);

                // If it's your team's flag, return it to the default position
                } else {
                    flag.pos_x = flag.default_pos_x;
                    flag.pos_y = flag.default_pos_y;

                }

            }

        }
    }

}

#[no_mangle]
/// Returns whether or not a player was sucessfully spawned
pub unsafe fn spawn_player() -> u32 {
    // Next, find a map position on whatever side the player is on
    let is_left_team = FLAGS[0].assume_init_ref().team.id == PLAYER_TO_BE_ADDED.team_id;

    let good_spawn_fn = match is_left_team {
        true => |spawn_point: &&MinimalMapObject| spawn_point.pos_x <= MAP.width / 2.0,
        false => |spawn_point: &&MinimalMapObject| spawn_point.pos_x >= MAP.width / 2.0,

    };

    // Find a good spawn point, or just pick a random one
    let good_spawn_points: Vec<&MinimalMapObject> = MAP.spawn_points[0..MAP.num_spawn_points as usize].iter().filter(good_spawn_fn).collect();
    let spawn = good_spawn_points[RNG.generate_range(0..good_spawn_points.len())];

    PLAYER_TO_BE_ADDED.pos_x = spawn.pos_x + spawn.width * 0.5;
    PLAYER_TO_BE_ADDED.pos_y = spawn.pos_y + spawn.height * 0.5;

    true as u32
    
}

