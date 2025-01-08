# Third party libraries


## glad
Used to generate WGL, EGL and GLES3 extension loaders.

- Upstream: https://github.com/Dav1dde/glad
- Version: 2.0.4 (2348b07c1ab4504d60398713781d8a57880234fa)
- License: CC0 1.0 and Apache 2.0
- Required: Yes

Files extracted from upstream source:
- `LICENSE`

Files generated from [upstream web instance](https://gen.glad.sh/):
- `EGL/eglplatform.h`
- `KHR/khrplatform.h`
- `egl.c`
- `glad/egl.h`
- `gl.c`
- `glad/gl.h`
- `wgl.c`
- `glad/wgl.h`

Perma links:
- [GL/GLES](https://gen.glad.sh/#generator=c&api=gl%3D3.3%2Cgles2%3D3.0&profile=gl%3Dcore%2Cgles1%3Dcommon&extensions=GL_ARB_debug_output%2CGL_ARB_ES3_compatibility&options=LOADER%2CMERGE%2CMERGE)
- [EGL/WGL](https://gen.glad.sh/#generator=c&api=egl%3D1.5%2Cwgl%3D1.0&profile=gl%3Dcore%2Cgles1%3Dcommon&extensions=EGL_KHR_create_context%2CEGL_KHR_platform_android%2CEGL_KHR_platform_wayland%2CEGL_KHR_platform_x11%2CWGL_ARB_create_context%2CWGL_ARB_create_context_profile%2CWGL_ARB_extensions_string%2CWGL_ARB_pixel_format%2CWGL_EXT_create_context_es2_profile&options=LOADER%2CMERGE)


## libspng
ibspng (simple png) is a library for reading and writing png files. used by the png modules.

- Upstream: https://github.com/randy408/libspng
- Version: 0.7.4 (fb768002d4288590083a476af628e51c3f1d47cd)
- License: BSD 2-Clause
- Required: No

Files extracted from upstream source:
- `LICENSE`
- `spng/spng.c`
- `spng/spng.h`


## miniz
A lightweight replacment for zlib. Used by libspng.

- Upstream: https://github.com/richgel999/miniz
- Version: 3.0.2 (293d4db1b7d0ffee9756d035b9ac6f7431ef8492)
- License: MIT license
- Required: No

Files extracted from upstream source:
- `LICENSE`
- `miniz.c`
- `miniz.h`

## lua
The Lua programming language. Used by the lua module.

- Upstream: https://github.com/lua/lua
- Version: 5.4.6 (6443185167c77adcc8552a3fee7edab7895db1a9)
- License: MIT license
- Required: No

Files extracted from upstream source:
- `LICENSE` license text copied from https://www.lua.org/license.html
- `*.c`
- `*.h`

## RFont
A lightweight TrueType font rendering library. Used by the font modules.

- Upstream: https://github.com/ColleagueRiley/RFont
- Version: b3bbf4571b1855ae21adf2e85b391766481d775b
- License: Zlib License
- Required: No

Files extracted from upstream source:
- `LICENSE`
- `RFont.h`

## miniaudio
An audio playback and capture library. Used by the audio module.

- Upstream: https://github.com/ColleagueRiley/RFont
- Version: 0.11.21 (4a5b74bef029b3592c54b6048650ee5f972c1a48)
- License: Public Domain or MIT No Attribution
- Required: No

Files extracted from upstream source:
- `LICENSE`
- `miniaudio.h`

## dr_libs
Public domain, single file audio decoding libraries. Used by the audio module.

- Upstream: https://github.com/mackron/dr_libs
- Version: 660795b2834aebb2217c9849d668b6e4bd4ef810
- License: Public Domain or MIT No Attribution
- Required: No

Files extracted from upstream source:
- `LICENSE`
- `dr_flac.h`
- `dr_mp3.h`
- `dr_wav.h`