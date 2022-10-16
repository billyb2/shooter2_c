include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
use crate::NUM_TEAMS;
use nanorand::rand::wyrand::WyRand;

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

pub static mut TEAMS: [MinimalTeamInfo; 2] = [ZERO_TEAM; 2];
pub static mut RNG: WyRand = WyRand::new_seed(0);
pub static mut MAP: MinimalMapInfo = ZERO_MAP;
pub static mut DRAWABLE_OBJECTS: [DrawableObject; 255] = [ZERO_DRAWABLE_OBJECT; 255];
pub static mut PLAYER_TO_BE_ADDED: MinimalPlayerInfo = ZERO_PLAYER;
pub static mut WINNING_TEAM: MinimalTeamInfo = ZERO_TEAM;

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

static mut GAME_MODE_NAME: [u8; 17] = [0; 17];

#[no_mangle]
pub unsafe fn name_ptr() -> u64 {
    GAME_MODE_NAME[0..16].as_mut().copy_from_slice(crate::GAME_MODE_NAME.as_bytes());
    GAME_MODE_NAME.as_ptr() as u64

}


