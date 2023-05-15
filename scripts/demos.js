var rt_buffer;
var rt_module;
var rt_instance;
var rt_fov = 90;
var rt_import_object = {
    wasi_snapshot_preview1: {
        fd_close: (fd) => {
            console.log(`fd_close called with fd: ${fd}`);
            return 0; // or some other appropriate default value
        },
        fd_seek: () => { },
        fd_read: () => { },
        fd_write: (fd, iovs, iovsLen, nwritten) => {
            return 0; // ESUCCESS
        },
        proc_exit: (status) => {
            console.log(`Program exited with status ${status}`);
            return 0;
        },
        random_get: (bufferPtr, numBytes) => {
            const memory = importObject.wasi_snapshot_preview1.memory;
            const buffer = new Uint8Array(memory.buffer, bufferPtr, numBytes);
            crypto.getRandomValues(buffer);
            return 0;
        },
        fd_fdstat_get: () => { },
        environ_sizes_get: () => { },
        environ_get: () => { },
        args_sizes_get: () => { },
        args_get: () => { },
    }
};


var as_buffer;
var as_module;
var as_instance;
var as_import_object = {
    wasi_snapshot_preview1: {
        fd_close: (fd) => {
            console.log(`fd_close called with fd: ${fd}`);
            return 0; // or some other appropriate default value
        },
        fd_seek: () => { },
        fd_read: () => { },
        fd_write: (fd, iovs, iovsLen, nwritten) => {
            return 0; // ESUCCESS
        },
        proc_exit: (status) => {
            console.log(`Program exited with status ${status}`);
            return 0;
        },
        random_get: (bufferPtr, numBytes) => {
            const memory = importObject.wasi_snapshot_preview1.memory;
            const buffer = new Uint8Array(memory.buffer, bufferPtr, numBytes);
            crypto.getRandomValues(buffer);
            return 0;
        },
        fd_fdstat_get: () => { },
        environ_sizes_get: () => { },
        environ_get: () => { },
        args_sizes_get: () => { },
        args_get: () => { },
    }
};

var wn_buffer;
var wn_module;
var wn_instance;
var wn_import_object = {
    wasi_snapshot_preview1: {
        fd_close: (fd) => {
            console.log(`fd_close called with fd: ${fd}`);
            return 0; // or some other appropriate default value
        },
        fd_seek: () => { },
        fd_read: () => { },
        fd_write: (fd, iovs, iovsLen, nwritten) => {
            return 0; // ESUCCESS
        },
        proc_exit: (status) => {
            console.log(`Program exited with status ${status}`);
            return 0;
        },
        random_get: (bufferPtr, numBytes) => {
            const memory = wn_import_object.wasi_snapshot_preview1.memory;
            const buffer = new Uint8Array(memory.buffer, bufferPtr, numBytes);
            crypto.getRandomValues(buffer);
            return 0;
        },
        fd_fdstat_get: () => { },
        environ_sizes_get: () => { },
        environ_get: () => { },
        args_sizes_get: () => { },
        args_get: () => { },
    },
};


window.onload = function (event) {
    const fetchPromises = [];

    fetchPromises.push(fetch('wasm/raytracer/raytracer.wasm')
        .then(response => response.arrayBuffer())
        .then(rt_buffer => WebAssembly.compile(rt_buffer))
        .then(rt_module => WebAssembly.instantiate(rt_module, rt_import_object))
        .then(rt_instance => {
            rt_import_object.wasi_snapshot_preview1.memory = rt_instance.exports.memory;
            // function(screen_width, screen_height, fov, height_start, height_end, render_option)
            var rt_entry_point = rt_instance.exports.render_entry_point;

            var rt_intersection = document.getElementById("rt_intersection");
            var rt_depth = document.getElementById("rt_depth");
            var rt_basic = document.getElementById("rt_basic");

            if (rt_intersection) {
                rt_intersection.addEventListener("click", function () {
                    canvas = document.getElementById("RTCanvas");
                    context = canvas.getContext("2d");
                    imgData = context.createImageData(canvas.width, canvas.height);
                    rt_entry_point(canvas.width, canvas.height, document.getElementById("rt_fov").value, 0, canvas.height, 0);
                    img = new Uint8ClampedArray(rt_instance.exports.memory.buffer, rt_instance.exports.img_data, canvas.width * canvas.height * 4);
                    imgData.data.set(img);
                    context.putImageData(imgData, 0, 0);
                    console.log(rt_instance.exports.memory);
                });
            }
            if (rt_depth) {
                rt_depth.addEventListener("click", function () {
                    start = performance.now();
                    canvas = document.getElementById("RTCanvas");
                    context = canvas.getContext("2d");
                    imgData = context.createImageData(canvas.width, canvas.height);
                    rt_entry_point(canvas.width, canvas.height, document.getElementById("rt_fov").value, 0, canvas.height, 1);
                    img = new Uint8ClampedArray(rt_instance.exports.memory.buffer, rt_instance.exports.img_data, canvas.width * canvas.height * 4);
                    imgData.data.set(img);
                    context.putImageData(imgData, 0, 0);
                    console.log("Raytracer: " +(performance.now() - start) + "ms");
                });
            }
            if (rt_basic) {
                rt_basic.addEventListener("click", function () {
                    canvas = document.getElementById("RTCanvas");
                    context = canvas.getContext("2d");
                    imgData = context.createImageData(canvas.width, canvas.height);
                    rt_entry_point(canvas.width, canvas.height, document.getElementById("rt_fov").value, 0, canvas.height, 2);
                    img = new Uint8ClampedArray(rt_instance.exports.memory.buffer, rt_instance.exports.img_data, canvas.width * canvas.height * 4);
                    imgData.data.set(img);
                    context.putImageData(imgData, 0, 0);
                });
            }
        })
        .catch(console.error));

    fetchPromises.push(fetch('wasm/astar/astar.wasm')
        .then(response => response.arrayBuffer())
        .then(as_buffer => WebAssembly.compile(as_buffer))
        .then(as_module => WebAssembly.instantiate(as_module, as_import_object))
        .then(as_instance => {
            as_import_object.wasi_snapshot_preview1.memory = as_instance.exports.memory;
            var as_canvas_offset = as_instance.exports;

            var as_run = document.getElementById("as_run");
            if (as_run) {
                as_run.addEventListener("click", function () {
                    canvas = document.getElementById("ASCanvas");
                    context = canvas.getContext("2d");
                    imgData = context.createImageData(canvas.width, canvas.height);
                    img = new Uint8ClampedArray(as_instance.exports.memory.pixels, as_instance.exports.pixels, canvas.width * canvas.height * 4);
                    imgData.data.set(img);
                    context.putImageData(imgData, 0, 0);
                    console.log("ran");
                    console.info(img);
                });

            }
        })
        .catch(console.error));

    fetchPromises.push(fetch('wasm/worleynoise/worleynoise.wasm')
        .then(response => response.arrayBuffer())
        .then(wn_buffer => WebAssembly.compile(wn_buffer))
        .then(wn_module => WebAssembly.instantiate(wn_module, wn_import_object))
        .then(wn_instance => {
            wn_import_object.wasi_snapshot_preview1.memory = wn_instance.exports.memory;
            var canvas_offset = wn_instance.exports.getCanvas;
            var generate = wn_instance.exports.generate;

            var wn_run = document.getElementById("wn_run");
            if (wn_run) {
                wn_run.addEventListener("click", function () {
                    start = performance.now();
                    canvas = document.getElementById("WNCanvas");
                    context = canvas.getContext("2d");
                    imgData = context.createImageData(canvas.width, canvas.height);
                    generate(document.getElementById("wn_input").value, document.getElementById("wn_range").value);
                    img = new Uint8ClampedArray(wn_instance.exports.memory.buffer, canvas_offset(), 300 * 600 * 4);
                    imgData.data.set(img);
                    context.putImageData(imgData, 0, 0);
                    console.log("Worly Noise: " +(performance.now() - start) + "ms");
                });
            }
        })
        .catch(console.error));

    Promise.all(fetchPromises)
        .then(function () {
            // Last function to be called after all fetch requests are done
            rt_depth.click();
            wn_run.click();

        })
        .catch(console.error);
};