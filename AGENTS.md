# AGENTS.md

3Days — a HolyC compiler / TempleOS port. Two halves that must be understood together:

- **C host** (`main.c`, `runtime.c`, `tos_aot.c`, `vfs.c`, `window.c`/`WIN_window.cpp`, `multic.c`):
  loads and relocates a pre-compiled HolyC image, provides the FFI, the drive-letter VFS, one
  screen, and the cores. Binary is `3d_loader` (installed as `3d`).
- **`T/`** — the `T:` drive, mounted at runtime. Contains the actual compiler, editor, window
  manager and DolDoc **written in HolyC** (`.HC`/`.HH`). Most of it is JIT-compiled at runtime.

`README.MD` is the HolyC API reference (graphics, DolDoc, input, debugger); read it before
writing HolyC. This file only covers what that README does not.

## Build

```sh
make                      # Linux -> ./3d_loader   (CC=clang in the Makefile)
make -f Makefile.WIN      # Windows cross-build via mingw-w64 + nasm -> 3d_loader.exe
```

- Needs: clang, yasm, X11 + Xext + GL, and `pkg-config portaudio-2.0`. FreeBSD uses `gmake`.
  (`README.MD` says gcc + tcsh — outdated; only the `*.deb`/`*.pkg` targets need `tcsh`.)
- `-DUSE_NETWORKING` is always on in both Makefiles, so the networking FFI is live in any build.
- `CFLAGS` suppresses `-Wimplicit-function-declaration`, `-Wint-conversion` and
  `-Wincompatible-function-pointer-types`: broken C can compile and link silently, then fault at run time.
- `make: Circular FFI_SYSV.yasm <- FFI_SYSV.yasm.o dependency dropped.` is **benign** — printed on
  every invocation, and the `.yasm` is still assembled.
- `make clean` is `rm $(OBJS)`: it errors out if any `.o` is missing, and never removes `3d_loader`.
- `BungisFile` + `bungis.tcsh` is a stale hand-rolled build (no `-DUSE_NETWORKING`, no GL). Use the Makefile.
- `pkgWIN.sh` is stale too — it calls `makeWIN.tcsh`, which does not exist in the repo.

## Verifying a change without a display

The working loop is command-line mode; it needs no X server:

```sh
printf '"%%d\\n",6*7;\nExit;\n' > /tmp/t.HC
timeout 60 ./3d_loader -t T -c /tmp/t.HC
```

Traps, all verified:

- **Always end the script with `Exit;`** — without it the process never terminates.
- **Always wrap it in `timeout`** — a HolyC runtime fault prints a backtrace, then `ENTERED FAULT`,
  and hangs in the debugger waiting for input.
- **The exit code is always 0**, even on compile errors. Grep stdout for `ERROR:` instead.
- stdout is block-buffered, so a killed process loses output. Use `stdbuf -oL` when debugging a hang.
- Normal boot noise, do not chase it: `WARN: Fun header args mismatch ...`, `FL:T:/KERNELA.HH,<line>`,
  ` $LK,"..."$ ...` lines, `REMOVE:1024,768`, and ALSA/JACK errors on machines with no sound card.
- `-c` mounts the host's root at `Z:/`, so a script path and its `ERROR:` lines show as `Z:/<host path>`
  (e.g. `ERROR: ... Z:/tmp/t.HC,1`) — the way to map an error back to the host file. Reads via `Z:`
  work; `FileWrite("Z:/...")` **faults** (`VirtFileWrite` via `FileWrite` in `T/BlkDev2/DskFile.HC`)
  and drops into the debugger. Write to `T:` instead.
- `-b` boots the bounds-checking image: resolves `./HCRT_BC.BIN` from the process cwd exactly like
  the default resolves `./HCRT.BIN` (`Option(OPTf_BOUNDS_CHECK,1);`).
- Without `-t`: boot drive is `$HOME/.3DAYS_BOOT`, kept as-is if it exists and the installed template
  `/usr/local/include/3Days/T` is not newer; a newer template backs it up to `.3DAYS_BOOT_BAKCUP.N` and
  re-seeds; `-O` forces a fresh copy. Failing all that it falls back to `./T`, else exits. So in a repo
  checkout **always pass `-t T`** — a stale `~/.3DAYS_BOOT` otherwise silently wins.
- There is **no C test suite**: `tests.c` is dead (references removed `AST_*` enums, does not compile,
  referenced by no Makefile). Do not treat it as a gate.

## The `HCRT.BIN` bootstrap loop — the biggest trap

`HCRT.BIN` at the repo root is a **tracked binary** and is what actually boots
(`main.c` looks for `./HCRT.BIN` in the process cwd, then `/usr/local/include/3Days/HCRT.BIN`).

- Editing a `.HC` under `T/` that is `#include`d by `T/HCRT_TOS.HC` (its 64 includes — `KERNELA.HH`,
  `KERNELB.HH`, `STR.HC`, `WINMGR.HC`, `GR/*`, `FontStd8x12.HC`, …) does **nothing** until the image is
  rebuilt. Everything else in `T/` (docs, games, apps) is read at runtime and takes effect at once.
- `KERNELA.HH`/`KERNELB.HH` are extra-hidden: `T/FULL_PACKAGE.HC` embeds their text into the image with
  `BINFILE` "so end-users don't mess up their KERNEL*.HH files", and boot parses the *embedded* copy,
  not the file on disk.
- Rebuild from `T:/` inside 3Days (GUI or `-c`):
  `Cmp("FULL_PACKAGE.HC","HCRT.DBG.Z","HCRT.BIN");` → writes `T/HCRT.BIN` + `T/HCRT.DBG.Z`
  (the embedded boot in `FULL_PACKAGE.HC` calls `DbgInfoRead` on the `.DBG.Z`, so keep the two together).
  `T/FULL_PACKAGE_BC.HC` is `#define BC_ENABLE 1` + that same file → `HCRT_BC.BIN`.
- The compile lands in `T/`, but the loader reads the **repo root**. Copy `T/HCRT.BIN` → `./HCRT.BIN`,
  or the old kernel keeps booting. History commits the rebuilt `HCRT.BIN` in the same commit as the
  kernel sources it came from — do the same.
- `./3d_loader -t T -c T/ReleasePackage.HC` rebuilds `HCRT_BC.BIN` *and* stages a compressed release
  tree in `T/T` (gitignored). That is what the packaging targets run.

## Adding a function callable from HolyC

Three places, all required:

1. A `STK_<name>(int64_t *stk)` wrapper — arguments arrive as an `I64` array, return value is `I64`
   (`F64` goes through a union bit-cast; see `STK_GetVolume` in `runtime.c`).
2. A `STK_RegisterFunctionPtr(&ffi_blob, "Name", STK_<name>, <argc>)` line in `TOS_RegisterFuncPtrs()`
   (`runtime.c`). Networking wrappers live inside `#ifdef USE_NETWORKING`.
3. A prototype in `T/KERNELA.HH` (plain) or `T/KERNELB.HH` (asm-linked) — then rebuild `HCRT.BIN`.

Mismatched prototypes are what the `WARN: Fun header args mismatch` boot spam is reporting.

## Packaging

`make 3days.deb` (dpkg) and `make 3days-0.3.00.pkg` (FreeBSD `pkg create`) both shell out to `tcsh`
scripts, run the ReleasePackage step first, and install the binary as `/usr/local/bin/3d`. `3Days.nsi`
is the Windows installer; `+MANIFEST` is the FreeBSD manifest. The version lives in `+MANIFEST` and
`debian/DEBIAN/control` (both 0.5.00) — nothing syncs them, and the Makefile target is still literally
named `3days-0.3.00.pkg`, so the `.pkg` filename does not track the real version.

## HolyC and file conventions

- Lvalues can be cast: `x(I64*)++;`. Never declare inside a `for` header — `for(I64 i=0;…)` is invalid.
- Top-level statements execute immediately and still need their `;`. Text handed to the compiler as a
  string needs `;;` to terminate: `ExePrint("#define X 1;;")`.
- `*.Z` files (`T/AMOUSE.HC.Z`, `T/Tmp.DD.Z`) are compressed/encrypted containers, **not text** — never
  edit them with host text tools. Use HolyC `Zip(mask)` / `Unzip(mask)`.
- `T/` paths use `/` even on Windows; `vfs.c` translates (`delim` vs `TOS_delim`) — check it when
  touching path handling.
- Screen size and font are compile-time constants: `SCREEN_WIDTH`/`SCREEN_HEIGHT` in `3d.h`, fonts
  chosen by which `T/FONT*.HC` `T/HCRT_TOS.HC` includes.
- `T/Community-Heiresis/` is third-party imports and is misspelled in-tree; leave the name alone.
- No CI, no linter, no formatter in this repo. `.gitattributes` normalizes to LF.
