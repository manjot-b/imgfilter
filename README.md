# About
This project performs GPGPU computations to compute various image filters. It uses
[AMD's ROCm](https://rocm.docs.amd.com/en/latest/rocm.html) stack and
[HIP runtime](https://rocm.docs.amd.com/projects/HIP/en/latest/index.html) to perform the image filters on the GPU.

# Package Dependencies
On Arch Linux you just need to install the `rocm-hip-runtime` as outlined in the
[Arch Wiki](https://wiki.archlinux.org/title/GPGPU#ROCm)

On other Linux distros follow [AMD's guide](https://rocm.docs.amd.com/en/latest/deploy/linux/quick_start.html)
on getting ROCm setup on your platform.

# Building
1. Clone this repo and all submodules recursively.
```bash
git clone --recurse-submodules -j4 https://github.com/manjot-b/imgfilter.git
cd imgfilter
```
2. Create the build directory with the config files with `cmake --config ninja-rocm-release`. Or make a build
directory and `cd` into it.
```bash
mkdir -p build
cd build
```

3. Generate the config files via `cmake`.
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
```

4. Compile the program
```
ninja
```

Don't provide a generator to `cmake` with the `-G` option if you wish to use `Makefiles` to build the project.

For my system with a Polaris graphics cards (Radeon RX 580 gfx803), if I don't build with at least `-O1` optimization,
I get "Memory Fault" errors. You can try building with some other build type
with `-DCMAKE_BUILD_TYPE=<build-type>`.
