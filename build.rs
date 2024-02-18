// build.rs

fn main() {
    println!("cargo:rustc-link-search=native=../C/target/debug");
    println!("cargo:rustc-link-lib=static=C");
}