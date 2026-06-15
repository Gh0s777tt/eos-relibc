<a name="top"></a>
<p align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=0:E50914,100:0B0B0B&height=200&section=header&text=eos-relibc&fontSize=66&fontColor=ffffff&fontAlignY=38&desc=Rust%20C%20standard%20library%20for%20E-OS&descAlignY=60&descSize=18&animation=fadeIn" alt="eos-relibc"/>
</p>

<p align="center">
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=19&pause=900&color=E50914&center=true&vCenter=true&width=800&lines=Portable+C+standard+library+in+Rust;E-OS+downstream+of+Redox+relibc;aarch64+verify%28%29+fix" alt="tagline"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/license-MIT-E50914?style=for-the-badge&labelColor=0B0B0B" alt="license"/>
  <img src="https://img.shields.io/badge/Rust-E50914?style=for-the-badge&logo=rust&logoColor=white&labelColor=0B0B0B" alt="Rust"/>
  <img src="https://img.shields.io/badge/part%20of-E--OS-E50914?style=for-the-badge&labelColor=0B0B0B" alt="part of E-OS"/>
  <img src="https://img.shields.io/badge/upstream-redox--os%2Frelibc-E50914?style=for-the-badge&labelColor=0B0B0B" alt="upstream"/>
</p>

<p align="center"><img src="https://raw.githubusercontent.com/Gh0s777tt/Gh0s777tt/main/assets/divider.svg" width="100%" alt=""/></p>

relibc is a portable C standard library written in Rust and is under heavy development, this library contains the following items:

- C, Linux, BSD functions and extensions
- POSIX compatibility layer
- Interfaces for system components

The motivation for this project is twofold: Reduce issues that the Redox developers were having with [newlib](https://sourceware.org/newlib/), and create a more stable and safe alternative to C standard libraries written in C. It is mainly designed to be used under Redox, as an alternative to newlib, but it also supports Linux via the [sc](https://crates.io/crates/sc) crate.

Currently Redox and Linux are supported.

## `redox-rt`

`redox-rt` is a runtime library that provides much of the code that enables POSIX on Redox, like `fork`, `exec`, signal handling, etc.
Relibc uses it as backend in `src/platform/redox`, and it's intended to eventually be usable independently, without relibc.

## Repository Layout

- `include` - Header files (mostly macros and variadic functions `cbindgen` can't generate)
- `src` - Source files
- `src/c` - C code
- `src/crt0` - Runtime code
- `src/crti` - Runtime code
- `src/crtn` - Runtime code
- `src/header` - Header files implementation
- `src/header/*` - Each folder has a `cbindgen.toml` file, it generates a C-to-Rust interface and header files
- `src/ld_so` - Dynamic loader code
- `src/platform` - Platform-specific and common code
- `src/platform/redox` - Redox-specific code
- `src/platform/linux` - Linux-specific code
- `src/pthread` - pthread implementation
- `src/sync` - Synchronization primitives
- `tests` - C tests (each MR needs to give success in all of them)

## Download the sources

To download the relibc sources run the following command:

```sh
git clone --recursive https://gitlab.redox-os.org/redox-os/relibc
```

## Build Instructions

To build relibc out of the Redox build system, do the following steps:

### Dependencies

- Install `cbindgen`

```sh
cargo install cbindgen
```

#### Install the `expect` tool

- Debian, Ubuntu and PopOS:

```sh
sudo apt install expect
```

- Fedora:

```sh
sudo dnf install expect
```

- Arch Linux:

```sh
sudo pacman -S expect
```

### Build Relibc

To build the relibc library objects, run the following command:

```sh
make all
```

- Clean old library objects and tests

```sh
make clean
```

## Build relibc inside the Redox build system

Inside of your Redox build system, run:

```sh
make prefix
```

If you need to rebuild `relibc` for testing a Cookbook recipe, run:

```sh
touch relibc
make prefix r.recipe-name
```

Touching (changing the "last modified time" of) the `relibc` folder is needed to trigger recompilation for `make prefix`. Replace `recipe-name` with your desired recipe name.

Note: Do not edit `relibc` inside `prefix` folder! Do your work on `relibc` folder directly inside your Redox build system instead.

## Tests

Relibc has a test suite that also runs every time a new commit get pushed. You can see `.gitlab-ci.yml` to see how it's being executed. That being said, `./check.sh` is the recommended way to run tests. Here's few examples:

+ `./check.sh` - Run build, without running the test
+ `./check.sh --test` - Run all tests in x86_64 Redox using Redoxer
+ `./check.sh --test --host` - Run all tests in host (Linux)
+ `./check.sh --test --arch=aarch64` - Run all tests in specified arch
  - Arch can be `x86_64`, `aarch64`, `i586`, or `riscv64gc`
+ `./check.sh --test=stdio/printf` - Run a single test
  - Can be combined with `--host` or `--arch`
  - Will run statically linked test in Linux, dynamically linked in Redox

Couple of notes:

- Relibc and its tests will rebuild if files are changed, however switching between arch or host requires you to run `make clean`
- Redoxer is needed to run tests for Redox without `--host`. You can install it using `cargo install redoxer`
- Tests can hang, the test runner can anticipate this, assuming the kernel doesn't hang too.

## Issues

#### I'm building for my own platform which I run, and am getting `x86_64-linux-gnu-ar: command not found` (or similar)

The Makefile expects GNU compiler tools prefixed with the platform specifier, as would be present when you installed a cross compiler. Since you are building for your own platform, some Linux distributions (like Manjaro) don't install/symlink the prefixed executables.

An easy fix would be to replace the corresponding lines in `config.mk`, e.g.

```diff
ifeq ($(TARGET),x86_64-unknown-linux-gnu)
-	export CC=x86_64-linux-gnu-gcc
-	export LD=x86_64-linux-gnu-ld
-	export AR=x86_64-linux-gnu-ar
-	export NM=x86_64-linux-gnu-nm
+       export CC=gcc
+       export LD=ld
+       export AR=ar
+       export NM=nm
	export OBJCOPY=objcopy
	export CPPFLAGS=
	LD_SO_PATH=lib/ld64.so.1
endif
```

## Contributing

Before starting to contribute, read [this](CONTRIBUTING.md) document.

## Supported OSes

- Redox OS
- Linux

## Supported architectures

- i586 (Intel/AMD)
- x86_64 (Intel/AMD)
- aarch64 (ARM64)
- riscv64gc (RISC-V)

## Funding - _Unix-style Signals and Process Management_

This project is funded through [NGI Zero Core](https://nlnet.nl/core), a fund established by [NLnet](https://nlnet.nl) with financial support from the European Commission's [Next Generation Internet](https://ngi.eu) program. Learn more at the [NLnet project page](https://nlnet.nl/project/RedoxOS-Signals).

[<img src="https://nlnet.nl/logo/banner.png" alt="NLnet foundation logo" width="20%" />](https://nlnet.nl)
[<img src="https://nlnet.nl/image/logos/NGI0_tag.svg" alt="NGI Zero Logo" width="20%" />](https://nlnet.nl/core)

<p align="center"><img src="https://raw.githubusercontent.com/Gh0s777tt/Gh0s777tt/main/assets/divider.svg" width="100%" alt=""/></p>

<div align="center">

### 🩸 Part of the **GHOST EMPIRE** ecosystem

[**E-OS**](https://github.com/Gh0s777tt/E-OS) · Rust microkernel OS &nbsp;·&nbsp; Minecraft infrastructure suite &nbsp;·&nbsp; Discord &amp; streaming platforms — forged under **Empire Forge**.

<a href="https://discord.gg/Egf88V9UdH"><img src="https://img.shields.io/badge/Discord-Join%20the%20Empire-5865F2?style=for-the-badge&logo=discord&logoColor=white&labelColor=0B0B0B" alt="discord"/></a>
<a href="mailto:ghostt77@empire-forge.com"><img src="https://img.shields.io/badge/Email-Empire%20Forge-E50914?style=for-the-badge&logo=maildotru&logoColor=white&labelColor=0B0B0B" alt="email"/></a>
<a href="https://donatr.ee/ghost77/"><img src="https://img.shields.io/badge/%E2%9D%A4%20Support-Donate-E50914?style=for-the-badge&labelColor=0B0B0B" alt="donate"/></a>

<sub><i>Black. Red. Production-grade. — © GHOST EMPIRE · Empire Forge</i></sub>

</div>
