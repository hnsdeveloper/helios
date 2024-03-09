use std::path::PathBuf;

// It doesn't handle errors properly!!
fn find_files(dir: &String, ext: &String, recurse: bool) -> Vec<PathBuf> {
    let mut modules: Vec<PathBuf> = vec![];

    if let Ok(directory) = std::fs::read_dir(dir) {
        for value in directory {
            let node = value.expect("Can't read file/directory node");
            let node_path = node.path();
            let node_path_str = node.path().to_str().unwrap().to_string();
            let file_type = node.file_type().unwrap();

            if file_type.is_dir() && recurse {
                modules.append(&mut find_files(&node_path_str, &ext, true));
            } else if file_type.is_file() {
                let extension = match node_path.extension() {
                    Some(str) => str.to_str().unwrap(),
                    _ => " ",
                };
                if extension == ext {
                    modules.push(node_path);
                }
            }
        }
    }

    modules.clone()
}

fn main() {
    let cpp_files = find_files(&"./src".to_string(), &"cpp".to_string(), true);
    let assembly_files = find_files(&"./src".to_string(), &"S".to_string(), true);

    println!("Building cpp");
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
        .std("c++20")
        .shared_flag(true)
        .include("./src/cpp/")
        .files(cpp_files.into_iter())
        .files(assembly_files.into_iter())
        .compile("implementations");
}
