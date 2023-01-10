const fs = require('fs');
const os = require('os');
const path = require('path');
const { execSync } = require('child_process');
const crypto = require('crypto');

if (!fs.existsSync('.build')) {
	fs.mkdirSync('.build');
	if (os.platform() === 'win32') execSync('attrib +h .build');
}
const build = path.join(__dirname, '.build');

process.chdir('src');

const engine = './Engine';
const app = './App';
const shader = './Shader';

if (!fs.existsSync(path.join(build, 'hashes.json'))) {
	fs.writeFileSync(path.join(build, 'hashes.json'), '{}');
}
if (!fs.existsSync(path.join(build, engine)))
	fs.mkdirSync(path.join(build, engine));
if (!fs.existsSync(path.join(build, app))) fs.mkdirSync(path.join(build, app));

const hashes = JSON.parse(fs.readFileSync(path.join(build, 'hashes.json')));

function compileToObj(src, args, out = src, nested = 0) {
	process.chdir(src);
	const files = fs.readdirSync('.');
	console.log('Building:', process.cwd());
	for (let file of files) {
		const stat = fs.lstatSync(file);
		if (stat.isDirectory()) compileToObj(file, args, out, nested + 1);
		else if (path.extname(file) === '.cpp') {
			const hash = crypto.createHash('sha1');
			hash.update(fs.readFileSync(file));
			const digest = hash.digest('hex');
			if (hashes[file] && hashes[file] == digest) {
				console.log('No changes done to file:', file, '\tSkipping.');
				continue;
			} else hashes[file] = digest;

			execSync(
				[
					'g++',
					...args,
					'-c',
					file,
					`-o ${path.join(build, out, path.parse(file).name + '.o')}`,
				].join(' ')
			);
			console.log('Finished building:', path.parse(file).name + '.o');
		}
	}
	process.chdir('..');
}

function link(src, args, out) {
	console.log('Creating', out);
	const dir = process.cwd();
	process.chdir(src);
	const objs = [];
	const files = fs.readdirSync('.');
	for (let file of files) {
		if (path.extname(file) === '.o') {
			objs.push(file);
		}
	}

	execSync(
		['g++', ...objs, ...args, '-o', path.join(build, '../bin', out)].join(
			' '
		)
	);

	process.chdir(dir);
}

function buildShaders() {
	execSync(
		'%VULKAN_SDK%/Bin/glslc.exe ' +
			path.join(process.cwd(), 'Shader/shader.vert') +
			' -o ' +
			path.join(process.cwd(), '../bin/vertex.spv')
	);
	execSync(
		'%VULKAN_SDK%/Bin/glslc.exe ' +
			path.join(process.cwd(), 'Shader/shader.frag') +
			' -o ' +
			path.join(process.cwd(), '../bin/fragment.spv')
	);
}

const std = '-std=c++17';
const flags = '-Wall -Werror -Wextra -pedantic';
console.log(os.platform())
switch (os.platform()) {
	case 'win32':
		// Build engine.dll
		(() => {
			console.log('Building engine.dll');
			const includes =
				'-I%VULKAN_SDK%/Include -I' +
				process.cwd() +
				' -I' +
				path.join(process.cwd(), engine);
			const defines =
				'-D_DEBUG -DVK_DEBUG -DVK_USE_PLATFORM_WIN32_KHR -DDB_EXPORT -D_CRT_SECURE_NO_WARNINGS';
			const links =
				'-luser32 -L%VULKAN_SDK%/Lib -lvulkan-1 -lopengl32 -lglu32 -lgdi32';
			compileToObj(engine, [std, '-g', flags, includes, defines]);
			link(
				path.join(build, engine),
				['-shared', links, includes],
				'engine.dll'
			);
		})();

		// Build DeskBrew.exe
		(() => {
			console.log('Building DeskBrew.exe');
			const includes =
				'-I%VULKAN_SDK%/Include -I' +
				process.cwd() +
				' -I' +
				path.join(process.cwd(), engine);
			const defines = '-D_DEBUG -DDB_IMPORT';
			compileToObj(app, [std, flags, includes, defines]);

			const links =
				'-L' + path.join(process.cwd(), '../bin') + ' -lengine';
			link(path.join(build, app), [links], 'DeskBrew.exe');
		})();

		// Build shader files
		if (!process.argv.find((val) => val === '--no-shaders')) {
			console.log('Building shaders');
			buildShaders();
		}
		break;

	case 'linux':
		// Build libengine.so
		(() => {
			console.log('Building libengine.so');
			const includes =
				'-I%VULKAN_SDK%/Include -I' +
				process.cwd() +
				' -I' +
				path.join(process.cwd(), engine);
			const defines =
				'-D_DEBUG -DVK_DEBUG -DVK_USE_PLATFORM_XCB_KHR -DDB_EXPORT -D_CRT_SECURE_NO_WARNINGS';
			compileToObj(engine, [
				std,
				'-ggdb',
				flags,
				'-fPIC',
				includes,
				defines,
			]);

			const links = '-L%VULKAN_SDK%/lib -lvulkan -lX11-xcb -lxcb -lX11';
			link(path.join(build, engine), ['-shared', links], 'libengine.so');
		})();

		// Build DeskBrew executable
		(() => {
			console.log('Building DeskBrew executable');
			const includes =
				'-I%VULKAN_SDK%/Include -I' +
				process.cwd() +
				' -I' +
				path.join(process.cwd(), engine);
			const defines = '-D_DEBUG -DDB_IMPORT';
			compileToObj(app, [std, '-ggdb', flags, includes, defines]);

			const links =
				'-L' +
				path.join(process.cwd(), '../bin') +
				' -Wl,-rpath,' +
				path.join(process.cwd(), '../bin') +
				' -lengine';
			link(path.join(build, app), [links], 'DeskBrew');
		})();

		// TODO: Implement shader building for Linux platforms
		// Build shader files
		/* if (!process.argv.find((val) => val === '--no-shaders')) {
			console.log('Building shaders');
			buildShaders();
		} */
		break;

	default:
		console.error(
			'Unsupported platform...\nSupported platforms:\n\t- Win32\n\t- Linux'
		);
}

fs.writeFileSync(
	path.join(build, 'hashes.json'),
	JSON.stringify(hashes, null, 2)
);
