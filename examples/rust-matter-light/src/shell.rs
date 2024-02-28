fn cmd_onoff(_stdio: &mut stdio::Stdio, args: shell::Args<'_>) {
    let arg_state = args.get(1);
    match arg_state {
        Some(state_str) => {
            let is_on: bool;
            match state_str {
                "on" => is_on = true,
                "off" => is_on = false,
                _ => {
                    println!("Invalid argument for LED state (must be 'on' or 'off')");
                    return;
                }
            }
            use riot_wrappers::saul::{Phydat, ActuatorClass};
            let rgb_led = saul_reg::find_actuator(ActuatorClass::LedRgb).expect("No registered RGB LED was found!");
            let data: Phydat;
            if is_on {
                data = Phydat::new(&[1, 1, 1], None, 0);
            } else {
                data = Phydat::new(&[0, 0, 0], None, 0);
            }
            let _ = rgb_led.write(data);
        }
        None => {
            println!("Usage: onoff [on|off]");
        }
    }
}

static_command!(cmd_onoff, "onoff", "Turn the Color Light on or off", cmd_onoff);