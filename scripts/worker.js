let thrown = false;
function assert(result) {
    if (result == 0 && !thrown) {
        console.error((new Error("Assertion failed")).stack);
        thrown = true;
    }
}
let count = 0;
function printf(thing) {
    if (count < 50) {
        console.log(thing.toString());
        count++;
    }
}

class WasmHandler {
    constructor(file_name, canvas, args = []) {
        this.canvas = canvas;
        this.context = canvas.getContext('2d');
        this.buffer = null;
        this.is_playing = false;
        this.import_object = {
            wasi_snapshot_preview1: {
                clock_res_get: (clockId, resolutionPtr) => {
                    console.log(`clock_res_get called with clockId: ${clockId} and resolutionPtr: ${resolutionPtr}`);
                    return 0;
                },
                fd_close: (fd) => {
                    console.log(`fd_close called with fd: ${fd}`);
                    return 0;
                },
                fd_seek: () => { },
                fd_read: () => { },
                fd_write: (fd, iovs, iovsLen, nwritten) => {
                    return 0;
                },
                proc_exit: (status) => {
                    console.log(`Program exited with status ${status}`);
                    return 0;
                },
                fd_fdstat_get: () => { },
                environ_sizes_get: () => { },
                environ_get: () => { },
                args_sizes_get: () => { },
                args_get: () => { },
                random_get: (bufferPtr, numBytes) => {
                    const memory = this.import_object.wasi_snapshot_preview1.memory;
                    const buffer = new Uint8Array(memory.buffer, bufferPtr, numBytes);
                    crypto.getRandomValues(buffer);
                    return 0;
                },
            },
            env: {
                assert: assert,
                printf: printf,
            },
        };
        this.args = args;

        this.play = this.play.bind(this);
        this.execute = this.execute.bind(this);

        fetch(file_name)
            .then(response => response.arrayBuffer())
            .then(buffer => {
                this.buffer = buffer;
                return WebAssembly.compile(buffer);
            })
            .then(module => {
                this.module = module;
                return WebAssembly.instantiate(this.module, {
                    ...this.import_object,
                    env: {
                        ...this.import_object.env,
                        memory: new WebAssembly.Memory({ initial: 16384, maximum: 16384, shared: true }),
                    },
                });
            })
            .then(instance => {
                this.instance = instance;
                this.memory = instance.exports.memory;
                this.import_object.wasi_snapshot_preview1.memory = this.memory;
                this.update = this.instance.exports.update;
                this.draw = this.instance.exports.draw;
                
                this.is_playing = true;
                this.play();
            })
            .catch(console.error);
    }
    async execute() {
        let img_data = this.context.createImageData(this.canvas.width, this.canvas.height);

        let instantiated_args = [
            this.canvas.width,
            this.canvas.height,
        ];

        for (let arg of Object.values(this.args)) {
            if (typeof arg === "function") {
                instantiated_args.push(Number(arg()));
            }
            else {
                instantiated_args.push(arg);
            }
        }
        this.draw(...instantiated_args);

        let img = new Uint8ClampedArray(
            this.memory.buffer,
            this.instance.exports.img_data,
            this.canvas.width * this.canvas.height * 4
        );
        img_data.data.set(img);
        await new Promise(r => setTimeout(r, 0.01)); // Add a slight delay to allow UI updates
        this.context.putImageData(img_data, 0, 0);
    }

    play(timestamp) {
        if (!this.is_playing) {
            return;
        }
        this.is_playing = true;
        let intervalId = setInterval(() => {
            this.execute();
            let now = performance.now();
            let deltaTime = this.lastUpdateTime ? (now - this.lastUpdateTime) / 1000 : 0;
            this.update(deltaTime);
            this.lastUpdateTime = performance.now();
            console.log(deltaTime * 1000 + "ms");
        }, 1000 / 60);  // 60 fps

    }
}

let init = false;
let handler = null;

self.onmessage = function (event) {
    let msg = event.data;
    switch (msg.topic) {
        case "init":
            handler = new WasmHandler(msg.wasm_file, msg.canvas, msg.args);
            break;
        case "render_frame":
            handler.is_playing = false;
            let start = performance.now();
            handler.args = msg.args;
            handler.execute();
            console.log((performance.now() - start).toFixed(1) + "ms");
            break;
        case "render_play":
            handler.args = msg.args;
            if (!handler.is_playing) {
                handler.is_playing = true;
                handler.play();
            }
            break;
        default:
            console.warn("Unknown worker message topic");
            break;
    }
};
