#![allow(non_camel_case_types)]

use std::intrinsics::copy_nonoverlapping;
use std::{fs, slice};
use std::mem::size_of;
use std::sync::{Arc, Mutex};
use std::error::Error;
use std::ffi::{CStr, c_char};
use once_cell::sync::Lazy;
use wasmer::{Module, NativeFunc, Store, Instance, imports, ExportError};

include!(concat!(env!("OUT_DIR"), "/bot_actions.rs"));

const UP:u8 = 0x00;
const DOWN:u8 = 0xFF; 
const LEFT:u8 = 0x00; 
const RIGHT:u8 = 0xFF;
const NONE:u8 = 0x80;

impl Into<BotActions> for i64 {
    fn into(self) -> BotActions {
        let bytes = self.to_le_bytes();

        BotActions {
            movement_direction_up_down: bytes[0],
            movement_direction_left_right: bytes[1],
            direction: f32::from_le_bytes(bytes[2..6].try_into().unwrap()),
            shooting_and_ability: bytes[6],
        }
    }

}

struct BotData {
    instance: Instance,
    my_realloc: NativeFunc<(i32, i32, i32), ()>,
    get_num_players_ptr: NativeFunc<(), i32>,
    get_players_ptr: NativeFunc<(), i32>,
    get_num_projectiles_ptr: NativeFunc<(), i32>,
    get_projectiles_ptr: NativeFunc<(), i32>,
    get_projectiles_ptr_ptr: NativeFunc<(), i32>,
    actions: NativeFunc<(), i64>,
    player_index: u8,

}

struct WasmData {
    bots: Vec<BotData>,

}

static WASM_DATA: Lazy<Arc<Mutex<WasmData>>> = Lazy::new(|| {
    Arc::new(Mutex::new(WasmData { bots: Vec::new() }))

});

fn get_ptr_function(fn_name: &'static str, instance: &Instance) -> Result<NativeFunc<(), i32>, ExportError> {
    instance.exports.get_native_function::<(), i32>(fn_name)

}

fn setup_bot_inner(file_name_ptr: *const c_char, player_index: u8) -> Result<(), Box<dyn Error>> {
    let wasm_file_name = unsafe { CStr::from_ptr(file_name_ptr).to_str()? };

    println!("File: {}", wasm_file_name);

    let wasm_bytes = fs::read(wasm_file_name)?;

    let store = Store::default();

    let module = Module::new(&store, wasm_bytes)?;
    let import_object = imports! {};

    let instance = Instance::new(&module, &import_object)?;

    WASM_DATA.lock()?.bots.push(BotData {
        my_realloc: instance.exports.get_native_function::<(i32, i32, i32), ()>("my_realloc")?,
        get_num_players_ptr: get_ptr_function("get_num_players_ptr", &instance)?,
        get_players_ptr: get_ptr_function("get_players_ptr", &instance)?,
        get_num_projectiles_ptr: get_ptr_function("get_num_projectiles_ptr", &instance)?,
        get_projectiles_ptr_ptr: get_ptr_function("get_projectiles_ptr_ptr", &instance)?,
        get_projectiles_ptr: get_ptr_function("get_projectiles_ptr", &instance)?,
        actions: instance.exports.get_native_function::<(), i64>("actions_to_int")?,
        player_index,
        instance,

    });


    Ok(())

}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub fn setup_bot(file_name_ptr: *const c_char, player_index: u8) -> u8 {
    match setup_bot_inner(file_name_ptr, player_index) {
        Ok(()) => 0,
        Err(err) => {
            println!("{:?}", err);
            1

        },

    }
    

}

#[no_mangle]
#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub fn update_bot_info(players: *mut Player, num_players: u8, projectiles: *mut *mut Projectile, num_projectiles: *mut u16, map: *const Map) {
    let wasm_data = &mut WASM_DATA.lock().unwrap();
    let players = unsafe { slice::from_raw_parts_mut(players, num_players.into()) };
    let projectiles_ptr = unsafe {
        match (*projectiles).is_null() {
            false => Some(slice::from_raw_parts_mut(*projectiles, (*num_projectiles).into())),
            true => None,

        }

    };

    let minimal_player_list: Vec<MinimalPlayerInfo> = players.iter().map(|player| unsafe { get_minimal_player_info(player) }).collect();
    let minimal_projectile_list: Option<Vec<MinimalProjectileInfo>> = projectiles_ptr.map(|proj_ptr| proj_ptr.iter().map(|projectile| {
        MinimalProjectileInfo {
            pos_x: projectile.pos_x,
            pos_y: projectile.pos_y,

        }

    }).collect());

    
    for bot in wasm_data.bots.iter_mut() {
        let memory = bot.instance.exports.get_memory("memory").unwrap();
        let mem_buffer = unsafe { memory.data_unchecked_mut() };


        let num_players_ptr = bot.get_num_players_ptr.call().unwrap() as usize;
        let players_ptr = bot.get_players_ptr.call().unwrap() as usize;

        unsafe {
            // Just memcpy to the bots memory, basically
            copy_nonoverlapping(minimal_player_list.as_ptr().cast::<u8>(), &mut mem_buffer[players_ptr], size_of::<MinimalPlayerInfo>() * minimal_player_list.len());

        };

        mem_buffer[num_players_ptr] = num_players;

        let projectiles_ptr = bot.get_projectiles_ptr.call().unwrap() as usize;
        let projectiles_ptr_ptr = bot.get_projectiles_ptr_ptr.call().unwrap();
        let num_projectiles_ptr = bot.get_num_projectiles_ptr.call().unwrap() as usize;

        bot.my_realloc.call(
            projectiles_ptr_ptr, 
            u16::from_le_bytes(mem_buffer[num_projectiles_ptr..num_projectiles_ptr + 2].try_into().unwrap()).into(), 
            (size_of::<MinimalProjectileInfo>() * unsafe { *num_projectiles } as usize) as i32
        ).unwrap();

        if let Some(minimal_projectile_list) = &minimal_projectile_list {
            unsafe {
                copy_nonoverlapping(minimal_projectile_list.as_ptr().cast::<u8>(), &mut mem_buffer[projectiles_ptr], size_of::<MinimalProjectileInfo>() * (*num_projectiles as usize));

            };
        }

        unsafe { mem_buffer[num_projectiles_ptr..num_projectiles_ptr + 2].copy_from_slice(&(*num_projectiles).to_le_bytes()) };

        let bot_actions: BotActions = bot.actions.call().unwrap().into();
        let player = &mut players[bot.player_index as usize];

        let movement_info = PlayerMovementInfo { x_axis: bot_actions.movement_direction_left_right.into(), y_axis: bot_actions.movement_direction_up_down.into() };

        player.direction = bot_actions.direction;

        unsafe { move_player(player, movement_info, map) };

        if (bot_actions.shooting_and_ability & 0x0F) == 0x0F {
            unsafe { use_ability(player, map) };

        }

        if (bot_actions.shooting_and_ability & 0xF0) == 0xF0 {
            unsafe { shoot(projectiles, num_projectiles, player, player.direction) };

        }


    }

}
