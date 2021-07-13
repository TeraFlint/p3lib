# Pitri's C++20 library

This is my second attempt of a nicely structured library, which I'm going to fill with (hopefully) useful and generalized C++ classes.

The library will make heavy use of C++ modules and concepts, so C++20 will be an absolute neccessity to use this thing.

My aim is to keep it cross-platform, but depending on the situation with the planned image class (as I've used the WIC library for that in the past), I'm not sure if I can commit to this. Either way, any platform specific code will be wrapped in a preprocessor block, so linux users can still compile it, albeit with limited functionality.
