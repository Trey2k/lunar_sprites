const Engine = class {
	constructor(base_path) {
		this.options = {
			args : [ "--log-level", "info", "--path", "/user_fs" ],
			base_path : base_path,
			bin_path : base_path + '.wasm',
			side_modules : [ base_path + '.side.wasm' ],
			static_files : [],

			persistent_paths : [ '/user_fs' ],
			dynamic_modules : [],
		};

		this.FS = null;
	}

	async init(options) {
		if (options) {
			if (options.args) {
				this.options.args.push.apply(this.options.args, options.args);
			}
			if (options.base_path) {
				this.options.base_path = options.base_path;
				this.options.bin_path = options.base_path + '.wasm';
				this.options.side_modules = [ options.base_path + '.side.wasm' ];
			}
			if (options.bin_path) {
				this.options.bin_path = options.bin_path;
			}
			if (options.side_modules) {
				this.options.side_modules = options.side_modules;
			}
			if (options.static_files) {
				this.options.static_files.push.apply(this.options.static_files, options.static_files);
			}
			if (options.persistent_paths) {
				this.options.persistent_paths.push.apply(this.options.persistent_paths, options.persistent_paths);
			}
			if (options.dynamic_modules) {
				this.options.dynamic_modules.push.apply(this.options.dynamic_modules, options.dynamic_modules);
			}
		}

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
			'thisProgram' : './' + this.options.bin_path,
			'dynamicLibraries' : [ this.options.base_path + '.side.wasm' ],
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

		this.FS = new LSFS(this.module);

		await this.FS.init(this.options.persistent_paths);
		await Promise.all(this.options.dynamic_modules.map(async (module) => {
			let response = await fetch(module);
			let buffer = await response.arrayBuffer();
			this.FS.copy_to_fs("/user_fs/" + module, buffer);
		}));

		await Promise.all(this.options.static_files.map(async (file) => {
			let response = await fetch(file);
			let buffer = await response.arrayBuffer();
			this.FS.copy_to_fs("/user_fs/" + file, buffer);
		}));
	}

	async start() {
		this.module['callMain'](this.options.args);
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
		let r = fetch(this.options.bin_path);
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
			return this.options.base_path + '.worker.js';
		} else if (path.endsWith('.js')) {
			return this.options.base_path + '.js';
		} else if (path.endsWith('.side.wasm')) {
			return this.options.base_path + '.side.wasm';
		} else if (path.endsWith('.wasm')) {
			return this.options.base_path + '.wasm';
		}
	}
}
