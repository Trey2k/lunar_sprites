const Engine = class {
	constructor(base_path) {
		this.base_path = base_path;
		this.bin_path = base_path + '.wasm';
	}

	async init() {
		const nodes = document.getElementsByTagName('canvas');
		if (nodes.length && nodes[0] instanceof HTMLCanvasElement) {
			const first = nodes[0];
			this.canvas = (first);
		}
		if (!this.canvas) {
			throw new Error('No canvas found in page');
		}

		this.module = await LunarSprites({
			'print' : this.print,
			'printErr' : this.printErr,
			'thisProgram' : './' + this.bin_path,
			'noExitRuntime' : false,
			'instantiateWasm' : this.instantiateWasm.bind(this),
			'locateFile' : this.locateFile.bind(this),
			'canvas' : this.canvas,
		});

		this.module.new_canvas = this.new_canvas;
		this.module.delete_canvas = this.delete_canvas;
		this.module.set_canvas_focused = this.set_canvas_focused;
		this.module.set_canvas_size = this.set_canvas_size;
		this.module.set_canvas_position = this.set_canvas_position;
		this.module.set_canvas_visibility = this.set_canvas_visibility;
		this.module.set_canvas_fullscreen = this.set_canvas_fullscreen;
	}

	async start(args) {
		this.module['callMain'](args);
	}

	print(text) {
		console.log(text);
	}

	printErr(text) {
		console.error(text);
	}

	new_canvas(canvas_id, width, height, x, y) {
		var canvas = document.createElement('canvas');
		canvas.id = canvas_id;
		canvas.width = width;
		canvas.height = height;
		canvas.style.position = 'absolute';
		canvas.style.left = x + 'px';
		canvas.style.top = y + 'px';
		canvas.tabIndex = -1;
		document.body.appendChild(canvas);
	}

	delete_canvas(canvas_id) {
		var canvas = document.getElementById(canvas_id.replace('#', ''));
		if (canvas) {
			canvas.remove();
		}
	}

	set_canvas_focused(canvas_id, focused) {
		var canvas = document.getElementById(canvas_id.replace('#', ''));
		if (canvas) {
			if (focused) {
				canvas.focus();
			} else {
				canvas.blur();
			}
		}
	}

	set_canvas_size(canvas_id, width, height) {
		var canvas = document.getElementById(canvas_id.replace('#', ''));
		if (canvas) {
			canvas.width = width;
			canvas.height = height;
		}
	}

	set_canvas_position(canvas_id, x, y) {
		var canvas = document.getElementById(canvas_id.replace('#', ''));
		if (canvas) {
			canvas.style.left = x + 'px';
			canvas.style.top = y + 'px';
		}
	}

	set_canvas_visibility(canvas_id, visible) {
		var canvas = document.getElementById(canvas_id.replace('#', ''));
		if (canvas) {
			canvas.style.visibility = visible ? 'visible' : 'hidden';
		}
	}

	set_canvas_fullscreen(canvas_id, fullscreen) {
		var canvas = document.getElementById(canvas_id.replace('#', ''));
		if (canvas) {
			if (fullscreen) {
				canvas.style.position = 'fixed';
				canvas.style.left = '0px';
				canvas.style.top = '0px';
				canvas.width = window.innerWidth;
				canvas.height = window.innerHeight;
				canvas.last_width = canvas.width;
				canvas.last_height = canvas.height;
			} else {
				canvas.style.position = 'absolute';
				canvas.style.left = '0px';
				canvas.style.top = '0px';
				canvas.width = canvas.last_width;
				canvas.height = canvas.last_height;
			}
		}
	}

	instantiateWasm(imports, onSuccess) {
		let r = fetch(this.bin_path);
		function done(result) {
			onSuccess(result['instance'], result['module']);
		}
		if (typeof (WebAssembly.instantiateStreaming) !== 'undefined') {
			WebAssembly.instantiateStreaming(Promise.resolve(r), imports).then(done);
		} else {
			r.arrayBuffer().then(function(buffer) {
				WebAssembly.instantiate(buffer, imports).then(done);
			});
		}
		r = null;
		return {};
	}

	locateFile(path) {
		if (!path.startsWith('lunar_sprites.')) {
			return path;
		} else if (path.endsWith('.worker.js')) {
			return this.base_path + '.worker.js';
		} else if (path.endsWith('.js')) {
			return this.base_path + '.js';
		} else if (path.endsWith('.wasm')) {
			return this.base_path + '.wasm';
		}
	}
}
