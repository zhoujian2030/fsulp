# How to build
---
- Both ulp and dpe run in arm linux
```
# ./buildarm.sh clean
# ./buildarm.sh   #build both ulp and dpe bin
```

- ulp runs in arm linux (little-endian), dpe runs in ppc linux (big-endian)
```
# ./buildarm.sh clean
# ./buildarm.sh --nodpe   # build ulp bin with -DCONVERT_ENDIAN
# ./buildppc.sh           # build dpe bin
```

- run unit test in linux VM
```
Note: make sure gtest lib is already comiled

# cd test/lib/gtest
# ./build.sh
# cd -
# ./build.sh clean
# ./build.sh
```

- run in linux VM
```
./build.sh clean
./build.sh --disabletest
```