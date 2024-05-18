let motor_speed = 0;
let motor_direction = 0;
let motor_duration = 0;

let speedContainer = document.getElementById("motor-speed");
let speedDisplayContainer = document.getElementById("motor-speed-display");

let leftDirectionContainer = document.getElementById("motor-direction-left");
let rightDirectionContainer = document.getElementById("motor-direction-right");

let enableDurationContainer = document.getElementById("motor-duration-enable");
let durationContainer = document.getElementById("motor-duration");

const updateMotorSpeed = () => {
    speedDisplayContainer.innerText = speedContainer.value;
    motor_speed = speedContainer.value;
}

const updateMotorDirection = () => {
    motor_direction = leftDirectionContainer.checked ? 0 : 1;
}

const toggleDurationInput = () => {
    durationContainer.disabled = !enableDurationContainer.checked;
}

const validateMotorDuration = () => {
    let duration = parseInt(durationContainer.value);
    if (isNaN(duration)) return;
    else if (duration < 0) duration = 0;
    else if (duration > 300) duration = 300;

    motor_duration = duration;
    durationContainer.value = duration.toString();
}

const startMotor = () => {
    let headers = {
        'MOTOR_SPEED': motor_speed.toString(),
        'MOTOR_DIRECTION': motor_direction.toString(),
    }

    if (enableDurationContainer.checked)
        headers.MOTOR_DURATION = motor_duration.toString();

    fetch("/api/rotate", {method: 'POST', headers: headers})
        .then(() => console.log("Started motor in direction ", headers));
}

const stopMotor = () => {
    fetch("/api/stop", {method: 'POST'}).then(() => console.log("Stopped the chair!"));
}

speedContainer.value = 0;
speedContainer.addEventListener('input', updateMotorSpeed, false);
enableDurationContainer.addEventListener('input', toggleDurationInput, false);
durationContainer.addEventListener('input', validateMotorDuration, false);
document.getElementsByName('motor-directions')
    .forEach(el => el.addEventListener('input', updateMotorDirection, false));
