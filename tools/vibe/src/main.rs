use anyhow::{bail, Context, Result};
use clap::{Parser, Subcommand};
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

const MANIFEST: &str = "vibe.project.json";

#[derive(Parser)]
#[command(name = "vibe", about = "Vibe Engine — build and run games", version)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Create a new game project from a template
    Init {
        /// Project directory (created if missing)
        path: PathBuf,
        /// Project display name
        #[arg(long, default_value = "MyGame")]
        name: String,
    },
    /// Build the engine runtime (from the engine repository checkout)
    BuildEngine {
        /// CMake build type
        #[arg(long, default_value = "Debug")]
        profile: String,
    },
    /// Run the game using vibe-runtime
    Run {
        /// Path to project root or any file inside it
        #[arg(default_value = ".")]
        path: PathBuf,
        /// Path to vibe-runtime binary (auto-detected when possible)
        #[arg(long)]
        runtime: Option<PathBuf>,
    },
    /// Check project and environment
    Doctor {
        #[arg(default_value = ".")]
        path: PathBuf,
    },
}

fn main() -> Result<()> {
    let cli = Cli::parse();
    match cli.command {
        Commands::Init { path, name } => cmd_init(&path, &name),
        Commands::BuildEngine { profile } => cmd_build_engine(&profile),
        Commands::Run { path, runtime } => cmd_run(&path, runtime.as_deref()),
        Commands::Doctor { path } => cmd_doctor(&path),
    }
}

fn cmd_init(path: &Path, name: &str) -> Result<()> {
    if path.exists() {
        let mut empty = true;
        if path.is_dir() {
            empty = fs::read_dir(path)?.next().is_none();
        }
        if !empty {
            bail!("'{}' already exists and is not empty", path.display());
        }
    } else {
        fs::create_dir_all(path)?;
    }

    let template = find_engine_root()?.join("templates/blank");
    if !template.is_dir() {
        bail!(
            "Template not found at {}. Run from a Vibe Engine checkout.",
            template.display()
        );
    }

    copy_dir_recursive(&template, path)?;

    let manifest_path = path.join(MANIFEST);
    let mut manifest: serde_json::Value =
        serde_json::from_str(&fs::read_to_string(&manifest_path)?)?;
    if let Some(obj) = manifest.as_object_mut() {
        obj.insert("name".into(), serde_json::Value::String(name.to_string()));
    }
    fs::write(&manifest_path, serde_json::to_string_pretty(&manifest)? + "\n")?;

    println!("Created Vibe project '{}' at {}", name, path.display());
    println!("Next: vibe run {}", path.display());
    Ok(())
}

fn cmd_build_engine(profile: &str) -> Result<()> {
    let root = find_engine_root()?;
    let build_dir = root.join("build");
    fs::create_dir_all(&build_dir)?;

    let status = Command::new("cmake")
        .arg("-S")
        .arg(&root)
        .arg("-B")
        .arg(&build_dir)
        .arg(format!("-DCMAKE_BUILD_TYPE={profile}"))
        .arg("-DCMAKE_CXX_COMPILER=g++")
        .status()
        .context("failed to launch cmake")?;
    if !status.success() {
        bail!("cmake configure failed");
    }

    let status = Command::new("cmake")
        .arg("--build")
        .arg(&build_dir)
        .status()
        .context("failed to launch cmake --build")?;
    if !status.success() {
        bail!("engine build failed");
    }

    let runtime = build_dir.join("vibe-runtime");
    println!("Built {}", runtime.display());
    Ok(())
}

fn cmd_run(path: &Path, runtime: Option<&Path>) -> Result<()> {
    let project_root = find_project_root(path)?;
    let runtime_bin = match runtime {
        Some(p) => p.to_path_buf(),
        None => detect_runtime()?,
    };

    if !runtime_bin.is_file() {
        bail!(
            "vibe-runtime not found at {}. Run: vibe build-engine",
            runtime_bin.display()
        );
    }

    let status = Command::new(&runtime_bin)
        .arg(&project_root)
        .status()
        .with_context(|| format!("failed to run {}", runtime_bin.display()))?;
    if !status.success() {
        bail!("runtime exited with {}", status);
    }
    Ok(())
}

fn cmd_doctor(path: &Path) -> Result<()> {
    let project_root = find_project_root(path).ok();
    if let Some(root) = &project_root {
        println!("OK  project root: {}", root.display());
        println!("OK  manifest: {}", root.join(MANIFEST).display());
    } else {
        println!("WARN  no {} found under {}", MANIFEST, path.display());
    }

    match detect_runtime() {
        Ok(bin) if bin.is_file() => println!("OK  runtime: {}", bin.display()),
        Ok(bin) => println!("WARN  runtime missing at {} (run vibe build-engine)", bin.display()),
        Err(e) => println!("WARN  runtime: {e}"),
    }

    if which("cmake") {
        println!("OK  cmake available");
    } else {
        println!("WARN  cmake not found on PATH");
    }

    if which("g++") || which("clang++") {
        println!("OK  C++ compiler available");
    } else {
        println!("WARN  no C++ compiler on PATH");
    }

    Ok(())
}

fn find_project_root(start: &Path) -> Result<PathBuf> {
    let start = if start.is_file() {
        start.parent().unwrap_or(Path::new("."))
    } else {
        start
    };
    let mut current = fs::canonicalize(start).unwrap_or_else(|_| start.to_path_buf());
    loop {
        if current.join(MANIFEST).is_file() {
            return Ok(current);
        }
        if !current.pop() {
            bail!("no {} found from {}", MANIFEST, start.display());
        }
    }
}

fn find_engine_root() -> Result<PathBuf> {
    let mut dir = std::env::current_dir()?;
    loop {
        if dir.join("engine").is_dir() && dir.join("CMakeLists.txt").is_file() {
            return Ok(dir);
        }
        if !dir.pop() {
            bail!("could not find Vibe Engine repository root (engine/ + CMakeLists.txt)");
        }
    }
}

fn detect_runtime() -> Result<PathBuf> {
    let root = find_engine_root()?;
    let candidate = root.join("build/vibe-runtime");
    Ok(candidate)
}

fn copy_dir_recursive(from: &Path, to: &Path) -> Result<()> {
    for entry in walkdir::WalkDir::new(from) {
        let entry = entry?;
        let rel = entry.path().strip_prefix(from)?;
        let dest = to.join(rel);
        if entry.file_type().is_dir() {
            fs::create_dir_all(&dest)?;
        } else {
            if let Some(parent) = dest.parent() {
                fs::create_dir_all(parent)?;
            }
            fs::copy(entry.path(), &dest)?;
        }
    }
    Ok(())
}

fn which(cmd: &str) -> bool {
    Command::new("which")
        .arg(cmd)
        .output()
        .map(|o| o.status.success())
        .unwrap_or(false)
}
