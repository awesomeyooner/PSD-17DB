With `firmware` being the root folder, run

```bash
# Options: Debug or Release
# Debug has no compiler optimizations
# Release does
$ cmake -B build --preset Debug
```

```bash
# cd into /firmware/build
$ cd build

# Compile using
$ make -j

# Flash DFU
$ make -j flash_dfu

# Flash ST-Link
$ make -j flash_stlink
```