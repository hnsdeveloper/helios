use std::path::PathBuf;
use std::process::Command;

// It doesn't handle errors properly!!
fn find_files(dir: &str, recurse: bool) -> Vec<PathBuf> {
    let mut modules: Vec<PathBuf> = vec![];

    if let Ok(directory) = std::fs::read_dir(dir) {
        for value in directory {
            let node = value.expect("Can't read file/directory node");
            let node_path = node.path();
            let node_path_str = node.path().to_str().unwrap().to_string();
            let file_type = node.file_type().unwrap();

            if file_type.is_dir() && recurse {
                modules.append(&mut find_files(&node_path_str, true));
            } else if file_type.is_file() {
                let extension = match node_path.extension() {
                    Some(str) => str.to_str().unwrap(),
                    _ => " ",
                };

                match extension {
                    "cpp" | "c" | "S" => modules.push(node_path),
                    _ => (),
                };
            }
        }
    }

    modules.clone()
}

fn main() {
    let output = Command::new("git")
        .args(&["rev-parse", "--short", "HEAD"])
        .output()
        .unwrap();
    let git_hash = String::from_utf8(output.stdout).unwrap();
    let mut git_hash_arg = "-DG_HASH=".to_owned();
    git_hash_arg.push_str(git_hash.as_str());

    let ddebug = match std::env::var("PROFILE").unwrap_or(String::new()).as_str() {
        "debug" => "-DDEBUG",
        _ => "",
    };

    let bootfiles = find_files("./src/arch/riscv64gc/", true);
    let memfiles = find_files("./src/mem/", true);
    let sysfiles = find_files("./src/sys/", true);
    println!("Building kernel.");
    // Builds cpp files
    cc::Build::new()
        .cpp(true)
        .cpp_link_stdlib(None)
        .flag("-ffreestanding")
        .flag("-fmodules-ts")
        .flag("-nostdlib")
        .flag("-fno-exceptions")
        .flag("-fno-rtti")
        .flag("-mabi=lp64d")
        .flag("-mcmodel=medany")
        .flag("-fno-use-cxa-atexit")
        .flag(&git_hash_arg)
        .flag(&ddebug)
        .std("c++20")
        .shared_flag(true)
        .include("./src/")
        .files(bootfiles)
        .files(memfiles)
        .files(sysfiles)
        .compile("kernel");
}
