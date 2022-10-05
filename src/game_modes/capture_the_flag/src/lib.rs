#![allow(clippy::missing_safety_doc)]
#![allow(non_snake_case)]
#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
mod math;

use std::mem::MaybeUninit;

use nanorand::{rand::wyrand::WyRand, Rng};
use math::*;

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

const ZERO_DRAWABLE_OBJECT: DrawableObject = DrawableObject {
    pos_x: 0.0,
    pos_y: 0.0,
    width: 0.0,
    height: 0.0,
    color: 0,
    shape: 0,

};

const ZERO_PLAYER: MinimalPlayerInfo = MinimalPlayerInfo {
    id: 0,
    team_id: 0,
    num_kills: 0,
    using_ability: 0,
    health: 0,
    ammo: 0,
    ability: 0,
    cloaking: false,
    direction: 0.0,
    pos_x: 0.0,
    pos_y: 0.0,
    weapon: 0,
    throwing_grenade: false as u16,
};

const ZERO_TEAM: MinimalTeamInfo = MinimalTeamInfo {
    id: 0,
    score: 0,
    players: [ZERO_PLAYER; 255],
    num_players: 0,
    color: 0,
};

const ZERO_MAP_OBJ: MinimalMapObject = MinimalMapObject {
    pos_x: 0.0,
    pos_y: 0.0,
    width: 0.0,
    height: 0.0,

};

const ZERO_MAP: MinimalMapInfo = MinimalMapInfo {
    width: 0.0,
    height: 0.0,
    num_spawn_points: 0,
    spawn_points: [ZERO_MAP_OBJ; 256],

};

static mut NUM_TEAMS: u32 = 2;
static mut TEAMS: [MinimalTeamInfo; 2] = [ZERO_TEAM; 2];
static mut FLAGS: [MaybeUninit<Flag>; 2] = [MaybeUninit::uninit(), MaybeUninit::uninit()];
static mut RNG: WyRand = WyRand::new_seed(0);

static mut MAP: MinimalMapInfo = ZERO_MAP;

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

static mut PLAYER_TO_BE_ADDED: MinimalPlayerInfo = ZERO_PLAYER;

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

static mut DRAWABLE_OBJECTS: [DrawableObject; 255] = [ZERO_DRAWABLE_OBJECT; 255];

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

static mut WINNING_TEAM: MinimalTeamInfo = ZERO_TEAM;

#[no_mangle]
pub unsafe fn calculate_scores() -> u32 {
    for flag in FLAGS.iter_mut() {
        let flag = flag.assume_init_mut();
        if (flag.default_pos_x == 0.0 && flag.pos_x >= MAP.width - 50.0) || (flag.default_pos_x == MAP.width && flag.pos_x <= 50.0) {
            // Find the flag's team and add to their score
            let team = &mut flag.team;
             
            team.score += 1;

            // Reset the flag to its default position
            flag.pos_x = flag.default_pos_x;
            flag.pos_y = flag.default_pos_y;
            flag.held_by = None;

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
            if true {//player.using_ability == false as u16 player.health > 0 {
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

static mut GAME_MODE_NAME: [u8; 17] = [0; 17];

#[no_mangle]
pub unsafe fn name_ptr() -> u64 {
    GAME_MODE_NAME[0..16].as_mut().copy_from_slice("Capture the Flag".as_bytes());
    GAME_MODE_NAME.as_ptr() as u64

}


#[no_mangle]
pub unsafe fn drawable_objects_ptr() -> u64 {
    DRAWABLE_OBJECTS.as_mut_ptr() as u64

}

#[no_mangle]
pub unsafe fn num_drawable_objects() -> u64 {
    2

}

#[no_mangle]
pub unsafe fn player_to_be_added_ptr() -> u64 {
    (&mut PLAYER_TO_BE_ADDED) as *mut MinimalPlayerInfo as u64

}

#[no_mangle]
pub unsafe fn winning_team_ptr() -> u64 {
    (&mut WINNING_TEAM) as *mut MinimalTeamInfo as u64

}

#[no_mangle]
pub unsafe fn teams_ptr() -> u64 {
    TEAMS.as_mut_ptr() as u64

}

#[no_mangle]
pub unsafe fn num_teams() -> u64 {
    NUM_TEAMS as u64

}

#[no_mangle]
pub unsafe fn map_ptr() -> u64 {
    (&mut MAP) as *mut MinimalMapInfo as u64

}

#[no_mangle]
pub unsafe fn set_rng_seed(seed: u64) {
    RNG = WyRand::new_seed(seed);

}
