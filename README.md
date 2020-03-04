s9r
===

This is Virtual Analog S(ynthesize)r for Raspberry Pi (for now).

Building
---

1. Install the required packages.(Linux only)

    ### Raspbian and Ubuntu

    ```shell
    sudo apt install cmake
    sudo apt install libx11-dev libasound2-dev libjack-jackd2-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
    ```

2. Clone and initialize the s9r repository.

    ```shell
    git clone https://github.com/y3o4xx/s9r.git
    cd s9r
    git submodule init
    git submodule update
    ```

3. Configure the build using cmake.

    ### Raspbian and Ubuntu

    ```shell
    mkdir build
    cd build
    cmake ..
    cd ..
    ```

    ### Windows

    MinGW is required when building on Windows.

    ```shell
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    cd ..
    ```

4. Run the build.

    ```shell
    cmake --build build
    ```

Running
---

### Raspbian and Ubuntu

```shell
jackd -d alsa -X raw &
build/bin/s9r
```

### Windows

```shell
build/bin/s9r
```

Testing
---

```shell
cmake --build build --target test
```
