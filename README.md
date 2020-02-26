s9r
===

This is software s(ynthesize)r.

Building
---

```shell
# install requirements
sudo apt install cmake libasound2-dev libjack-jackd2-dev

# build & install libsoundio
git clone https://github.com/andrewrk/libsoundio.git
cd libsoundio
mkdir build
cd build
cmake ..
make
sudo make install

# (interlude)

# build s9r
git clone https://github.com/y3o4xx/s9r.git
cd s9r
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