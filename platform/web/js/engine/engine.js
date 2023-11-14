const Engine = class {
	constructor(base_path) {
		this.base_path = base_path;
		this.bin_path = base_path + '.wasm';
	}

	async init() {
		this.module = await LunarSprites({
			'print' : this.print,
			'printErr' : this.printErr,
			'thisProgram' : './' + this.bin_path,
			'noExitRuntime' : false,
			'instantiateWasm' : this.instantiateWasm.bind(this),
			'locateFile' : this.locateFile.bind(this),

		});
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
