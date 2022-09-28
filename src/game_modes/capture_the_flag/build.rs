use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-search=c/wrapper.h");
    let bindings = bindgen::Builder::default()
        .clang_arg("-DWASM=1")
        .header("c/wrapper.h")
        .use_core()
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .unwrap();

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");

}
