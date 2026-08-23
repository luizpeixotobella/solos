use solos_runtime_core::ghost_audit::verify_artifact;
use std::env;
use std::path::PathBuf;
use std::process::ExitCode;

fn main() -> ExitCode {
    let mut arguments = env::args_os().skip(1);
    let Some(path) = arguments.next() else {
        eprintln!("usage: ghost-audit-verify <artifact.json>");
        return ExitCode::from(64);
    };
    if arguments.next().is_some() {
        eprintln!("ghost-audit-verify accepts exactly one artifact path");
        return ExitCode::from(64);
    }

    match verify_artifact(&PathBuf::from(path)) {
        Ok(receipt) => {
            match serde_json::to_string_pretty(&receipt) {
                Ok(payload) => println!("{payload}"),
                Err(error) => {
                    eprintln!("could not serialize Ghost audit receipt: {error}");
                    return ExitCode::from(1);
                }
            }
            if receipt.status == "passed" {
                ExitCode::SUCCESS
            } else {
                ExitCode::from(2)
            }
        }
        Err(error) => {
            eprintln!("{error}");
            ExitCode::from(1)
        }
    }
}
