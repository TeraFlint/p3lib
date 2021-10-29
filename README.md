# Pitri's C++20 library

This is my second attempt of a nicely structured library, which I'm going to fill with (hopefully) useful and generalized C++ classes. This time with a test driven approach, there's going to be really high test coverage!

The library will make heavy use of C++ modules and concepts, so C++20 will be an absolute neccessity to use this thing.

My aim is to keep it cross-platform, but depending on the situation with the planned image class (as I've used the WIC library for that in the past), I'm not sure if I can commit to this. Either way, any platform specific code will be wrapped in a preprocessor block, so linux users can still compile it, albeit with limited functionality.

If you're wondering where this weird namespace name is coming from: "P3" is my abbreviation of "Pitri".

## My plans moving forward
### modules
I love the C++20 modules. They:
* Largely remove the need to separate code into `.hpp` and `.cpp`, which makes them one cohesive unit.
* Don't leak macros (this is especially frustrating with `Windows.h`'s `min` and `max` macros meddling with the STL).
* Don't break the project if the module file path changes
* In general just seem to be plug and play, something C++ libraries desperately need.

However, the currently (2021-10) unfinished implementation of modules on visual studios is really fighting against me on my way towards progress:
* `import <stl header>;` quickly brings the IDE to the point of not working anymore: Syntax highlighting and mouseover texts just stop working, which makes it a lot harder to progress and debug.
* Especially working with type aliases and concepts from imported modules seem to be problematic in this regard.
* Once a critical mass of "problematic" features in modules is reached, small (and to my knowledge otherwise inconspicuous changes, like adding unrelated functions) create internal compiler errors (which just show up as mentioned, which reveals no further information about the actual reasons).

It is quite frustrating to continue at this point. I might have to go back to headers (mostly without source, since most/all of my stuff ist required to be placed in headers (due to my rather liberal use of `template` or `constexpr`), at least until the module implementations are more refined than the current mess of instability.

I'm still trying to cling to modules as long as possible, just because of their potential and the future they'll probably have.

### grid
* Split up `p3::grid` into `p3::fixed_grid`, `p3::dynamic_grid` and `p3::sparse_grid`.
* Make a `p3::grid` class which initially holds a `p3::sparse_grid` and converts it into a `p3::dynamic_grid`, once `p3::sparse_grid`'s space requirement grows beyond `p3::dynamic_grid`'s.

### image
* Inherit from `dynamic_grid<color, 2>` for storage and especially constructors.
* Inherit from `filestream_access` for basic I/O functionality.
* Only focus on encoding/decoding image formats.
* Proper marking of the unfinished implementation state for non-windows systems, if it happens to be using the WIC library.

### meta type manipulation
Currently both the most interesting thing to me and the most affected by the module problems.
* Algorithms operating on the type lists: concatenation, push/pop front/back, index of type, type of index, filter, sort, etc.
* It's the one I'm interested in the most, but this type stuff combined with c++20 modules is relatively demanding on the visual studio IDE.
* `constexpr` string manipulations on the basis of `values<char>`, allowing the creation of a usable string buffer (`std::array<char, n>`) at compile time.
* Static compile time type name generator (which will need a lot of template specializations for the most used container types)
* Mathematical type based formula abstraction, able to "collapse" to a single constant (or variable dependent types) during compilation.

### unit tests
* Specify a test result file format which is easily readable and compact.
* Move all existing tests into their own modules (which I've tried, but unfortunately brought the modules to a critical mass again).
* Of course: keep working in a test driven manner to keep the code coverage as high as possible.

### automatic commit integrity tests
* Setup pipeline to compile and run the tests.
* Generating summaries (with the help of `unit_test`'s planned save file format).
* Ensuring healthy code across compilers by taking (at least) "the big three" compilers into account. This should bring me out of my MSVC view and give me chances to grow and achieve awareness about compiler quirks, bugs and traps.
* Find out how to set something like this up, since I have basically no idea how.
