use serde::Serialize;
use std::env;
use std::fs;
use std::path::Path;
use std::process::Command;
use std::time::Duration;

#[derive(Serialize)]
#[allow(non_snake_case)]
pub(crate) struct HostRuntime {
    pub(crate) os: String,
    pub(crate) kernel: String,
    pub(crate) initSystem: String,
    pub(crate) sessionType: String,
    pub(crate) desktopSession: String,
    pub(crate) shell: String,
    pub(crate) hostname: String,
    pub(crate) user: String,
    pub(crate) uptime: String,
}

pub(crate) fn detect_host_runtime() -> HostRuntime {
    HostRuntime {
        os: read_os_pretty_name().unwrap_or_else(|| "Linux host".into()),
        kernel: run("uname", &["-r"]).unwrap_or_else(|| "unknown-kernel".into()),
        initSystem: detect_init_system(),
        sessionType: env::var("XDG_SESSION_TYPE").unwrap_or_else(|_| "unknown-session".into()),
        desktopSession: env::var("XDG_CURRENT_DESKTOP")
            .or_else(|_| env::var("DESKTOP_SESSION"))
            .unwrap_or_else(|_| "unknown-desktop".into()),
        shell: env::var("SHELL").unwrap_or_else(|_| "unknown-shell".into()),
        hostname: run("hostname", &[]).unwrap_or_else(|| "unknown-host".into()),
        user: env::var("USER").unwrap_or_else(|_| "unknown-user".into()),
        uptime: detect_uptime(),
    }
}

pub(crate) fn detect_online() -> bool {
    [
        "/sys/class/net/wlan0/operstate",
        "/sys/class/net/eth0/operstate",
    ]
    .iter()
    .any(|path| {
        fs::read_to_string(path)
            .map(|value| value.trim() == "up")
            .unwrap_or(false)
    }) || run("ip", &["route", "show", "default"]).is_some()
}

fn detect_uptime() -> String {
    let raw = fs::read_to_string("/proc/uptime")
        .ok()
        .and_then(|content| content.split_whitespace().next()?.parse::<f64>().ok());

    match raw {
        Some(seconds) => format_duration(Duration::from_secs_f64(seconds)),
        None => "unknown-uptime".into(),
    }
}

fn format_duration(duration: Duration) -> String {
    let total_seconds = duration.as_secs();
    let days = total_seconds / 86_400;
    let hours = (total_seconds % 86_400) / 3_600;
    let minutes = (total_seconds % 3_600) / 60;

    if days > 0 {
        format!("{days}d {hours}h {minutes}m")
    } else if hours > 0 {
        format!("{hours}h {minutes}m")
    } else {
        format!("{minutes}m")
    }
}

fn read_os_pretty_name() -> Option<String> {
    let content = fs::read_to_string("/etc/os-release").ok()?;
    for line in content.lines() {
        if let Some(value) = line.strip_prefix("PRETTY_NAME=") {
            return Some(value.trim_matches('"').to_string());
        }
    }
    None
}

fn detect_init_system() -> String {
    if let Ok(target) = fs::read_link("/proc/1/exe") {
        if let Some(name) = Path::new(&target)
            .file_name()
            .and_then(|name| name.to_str())
        {
            return name.to_string();
        }
    }

    run("ps", &["-p", "1", "-o", "comm="]).unwrap_or_else(|| "unknown-init".into())
}

fn run(command: &str, args: &[&str]) -> Option<String> {
    let output = Command::new(command).args(args).output().ok()?;
    if !output.status.success() {
        return None;
    }

    let text = String::from_utf8(output.stdout).ok()?;
    let trimmed = text.trim();
    if trimmed.is_empty() {
        None
    } else {
        Some(trimmed.to_string())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn duration_format_is_stable() {
        assert_eq!(format_duration(Duration::from_secs(90)), "1m");
        assert_eq!(format_duration(Duration::from_secs(7_500)), "2h 5m");
        assert_eq!(format_duration(Duration::from_secs(93_900)), "1d 2h 5m");
    }
}
