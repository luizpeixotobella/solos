use std::io::{self, Read};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Classification only: never writes audit artifacts or executes submitted text.
    let mut input = String::new();
    io::stdin().take(262_145).read_to_string(&mut input)?;
    if input.len() > 262_144 { return Err("input too large".into()); }
    println!("{}", serde_json::to_string(&ghost_core::ghost_audit::classify_input(&input))?);
    Ok(())
}
