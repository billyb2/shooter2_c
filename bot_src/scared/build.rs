use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=src/c/bot_actions.h");

    let bot_actions_bindings = bindgen::Builder::default()
        .header("src/c/bot_actions.h")
        .header("src/c/bot_actions_def.h")
        .header("src/c/bot_mem.h")
        .header("src/c/minimal_player_info.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .unwrap();
    
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());

    bot_actions_bindings
        .write_to_file(out_path.join("bot_actions.rs"))
        .unwrap();

}
