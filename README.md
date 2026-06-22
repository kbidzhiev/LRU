# cpp_proj

How to run

```bash
./build/main/main <<< "2 6 3 3 1 2 1 2"
```
output is 
```bash
Cache type   N hits
LRU          3
LFU          1
```



# Tips

## CMAKE


```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14

cmake --build build
ctest --test-dir build --output-on-failure
```
