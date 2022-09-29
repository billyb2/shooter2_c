# Shooter2_C

shooter2_c is a proejct i builtha allows users towrite bots in ay language that cna compile down to WASM, and have those bots fight to the death!

## Build instructions
### Linux
You'll need the following packages
- clang, gcc, lld, mold, cmake, ninja, rust (rustc + cargo), just (build runner)
To install Rust: https://www.rust-lang.org/tools/install
To install the remaining tools for:
- Gentoo: `emerge gcc lld mold cmake ninja && cargo install -f just` 
- Arch: `pacman -Syu build-tools lld mold just cmake ninja`

### Windows
I don't own a Windows machine, but I assume you'll need:
- rust: https://www.rust-lang.org/tools/install
- mingw
- lld
- clang
- cmake 
- ninja

### MacOS
lol

### Actually building
Once all the platform build tools are installed, you'll need to run the following prebuild steps
- `rustup update nightly`
- `rustup target add wasm32-unknown-unknown`
- `rustup target add x86_64-pc-windows-gnu`
- `rustup component add rust-src`

Then, you'll want to build your bots and move their .wasm file over to the `bots/` directory

In order to configure the project, run:
`just configure`

For Windows users, run:
`just configure_windows`

Finally, to actually run the project, run:
`just run[_windows]`	

or: 
`just run[_windows]_release`

To just build and not run the project, replace run with build for the commands above

### Docker
If you want to just compile a release build for Linux: run `sudo docker build -t shooter2_c . && sudo docker run -v (pwd):/out shooter2_c`

