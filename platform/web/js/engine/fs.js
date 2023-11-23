const ENOENT = 44;

const LSFS = class {
	constructor(module) {
		this.module = module;
		this.mount_points = [];
		this.idbfs = false;
		this.syncing = false;
	}

	is_persistent() {
		return this.idbfs ? 1 : 0;
	}

	init(presistent_paths) {
		this.idbfs = false;
		if (!Array.isArray(presistent_paths)) {
			throw new Error('presistent_paths must be an array');
		}

		if (!presistent_paths.length) {
			return;
		}
		this.mount_points = presistent_paths.slice();

		const createRecursive = (dir) => {
			try {
				this.module.FS.stat(dir);
			} catch (e) {
				if (e.errno !== ENOENT) {
					throw e;
				}
				this.module.FS.mkdirTree(dir);
			}
		};

		this.mount_points.forEach((path) => {
			createRecursive(path);
			this.module.FS.mount(this.module.IDBFS, {}, path);
		});

		return new Promise((resolve, reject) => {
			this.module.FS.syncfs(true, (err) => {
				if (err) {
					this.mount_points = [];
					this.idbfs = false;
					console.error(`IndexDB not available: ${err.message}`);
				} else {
					this.idbfs = true;
				}
				resolve(err);
			});
		});
	}

	deinit() {
		this.mount_points.forEach(function(path) {
			try {
				this.module.FS.unmount(path);
			} catch (e) {
				console.error(`Already unmounted`, e);
			}
			if (this.idbfs && IDBFS.dbs[path]) {
				IDBFS.dbs[path].close();
				delete IDBFS.dbs[path];
			}
		});
		this.mount_points = [];
		this.idbfs = false;
		this.syncing = false;
	}

	sync() {
		if (this.syncing) {
			console.error('sync already in progress');
			return Promise.resolve();
		}
		this.syncing = true;
		return new Promise(function(resolve, reject) {
			this.module.FS.syncfs(false, function(err) {
				if (err) {
					console.error(`syncfs failed: ${err.message}`);
				}
				this.syncing = false;
				resolve(err);
			});
		});
	}

	copy_to_fs(path, buffer) {
		const index = path.lastIndexOf('/');
		let dir = '/';
		if (index > 0) {
			dir = path.slice(0, index);
		}
		try {
			this.module.FS.stat(dir);
		} catch (e) {
			if (e.errno !== ENOENT) {
				throw e;
			}
			this.module.FS.mkdirTree(dir);
		}
		this.module.FS.writeFile(path, new Uint8Array(buffer));
	}
}