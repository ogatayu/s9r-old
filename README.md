s9r
===

This is Virtual Analog S(ynthesize)r for Raspberry Pi (for now).

Building
---

```shell
# install requirements
sudo apt install cmake libx11-dev libasound2-dev libjack-jackd2-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# build s9r
git clone https://github.com/y3o4xx/s9r.git
cd s9r
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make
```


Running
---

```shell
jackd -d alsa -X raw &
./s9r
```


Testing
---

```shell
make test
```