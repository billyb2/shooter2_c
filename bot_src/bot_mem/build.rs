use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=../c/player.h");
    println!("cargo:rerun-if-changed=../c/bot_actions_def.h");
    println!("cargo:rerun-if-changed=../c/bot_actions.h");
    println!("cargo:rerun-if-changed=src/c/player.c");
    println!("cargo:rerun-if-changed=src/c/player.h");
    println!("cargo:rerun-if-changed=src/c/projectile.c");
    println!("cargo:rerun-if-changed=src/c/projectile.h");

    let bot_actions_bindings = bindgen::Builder::default()
        .header("../c/player.h")
        .header("src/c/projectile.h")
        .header("../c/bot_actions.h")
        .header("../c/bot_actions_def.h")
        .header("../c/bot_mem.h")
        .header("../c/minimal_player_info.h")
        .use_core()
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .unwrap();
    
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());

    bot_actions_bindings
        .write_to_file(out_path.join("bot_actions.rs"))
        .unwrap();

}
