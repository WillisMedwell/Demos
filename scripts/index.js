var yaw = 0;   // x axis
var pitch = 0; // y axis
var roll = 0;  // z axis 

let raytracer_worker = new Worker('scripts/worker.js');

let has_gyro_support = true;

window.onload = function (event) {
    var child = document.querySelector('.Scale'); // Replace with your child class or ID
    var parent = child.parentNode;
    var scaleFactor = 0.8; // Replace with your scale factor

    var childHeight = child.offsetHeight;
    var newParentHeight = childHeight * scaleFactor;
    parent.style.height = newParentHeight + 'px';


    // Request permission for gyro.
    if (typeof DeviceMotionEvent.requestPermission === 'function') {
        DeviceMotionEvent.requestPermission()
            .then(permissionState => {
                if (permissionState === 'granted') {
                    has_gyro_support = true;
                    window.addEventListener('devicemotion', handleMotion, true);
                } else {
                    // Permission denied
                    console.log('Device motion permission denied.');
                    has_gyro_support = false;
                }
            })
            .catch(error => {
                console.error('Error requesting device motion permission:', error);
                has_gyro_support = false;
            });
    } else {
        console.log('Device motion events not supported.');
        has_gyro_support = false;
    }

    // Init workers and pass canvas to respective worker thread.
    let raytracer_canvas = document.getElementById("raytracer_canvas");
    //let noise_canvas = document.getElementById("noise_canvas");
    let raytracer_canvas_offscreen = raytracer_canvas.transferControlToOffscreen();
    //let noise_canvas_offscreen = noise_canvas.transferControlToOffscreen();

    raytracer_worker.postMessage(
        {
            topic: "init",
            wasm_file: "../wasm/raytracer.wasm",
            canvas: raytracer_canvas_offscreen,
            args: [
                Number(yaw), Number(pitch), Number(roll),
                Number(document.getElementById("raytracer_fov").value),
            ]
        },
        [raytracer_canvas_offscreen]
    );


    createActionTrigger("raytracer_fov", "input", postRaytracerData);
    createActionTrigger("raytracer_yaw", "input", postRaytracerData);
    createActionTrigger("raytracer_pitch", "input", postRaytracerData);

    createActionTrigger("gyro_enable", "input", getGyroPermission);
}

function postRaytracerData() {
    if (document.getElementById("gyro_enable").checked == false) {
        yaw = document.getElementById("raytracer_yaw").value;
        pitch = document.getElementById("raytracer_pitch").value;
        roll = 0;
    }

    raytracer_worker.postMessage(
        {
            topic: "render_play",
            args: [
                Number(yaw), Number(pitch), Number(roll),
                Number(document.getElementById("raytracer_fov").value),
            ]
        }
    );

}

function handleOrientation(event) {
    yaw = event.beta - 45;
    pitch = event.gamma;
    roll = 0;

    document.getElementById("raytracer_yaw").value = yaw;
    document.getElementById("raytracer_pitch").value = pitch;

    // as we have manually changed the value, we need to artifically create an 'onchange' event.
    let e = new Event('change', {
        bubbles: true,
        cancelable: true,
    });
    document.getElementById("raytracer_yaw").dispatchEvent(e);
    document.getElementById("raytracer_pitch").dispatchEvent(e);

    postRaytracerData();
}

function getGyroPermission() {
    if (document.getElementById("gyro_enable").checked == true) {
        if (typeof DeviceOrientationEvent.requestPermission === 'function') {
            DeviceOrientationEvent.requestPermission()
                .then(permissionState => {
                    if (permissionState === 'granted') {
                        window.addEventListener('deviceorientation', handleOrientation, true);
                    }
                })
                .catch(console.error);
        } else {
            window.addEventListener('deviceorientation', handleOrientation, true);
        }
    }
    else {
        window.removeEventListener('deviceorientation', handleOrientation, true);
    }
}

function createActionTrigger(id, type, action) {
    let document_trigger = document.getElementById(id);

    if (document_trigger) {
        document_trigger.addEventListener(type, async () => {
            action();
        });
    }
}







