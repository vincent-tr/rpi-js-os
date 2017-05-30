cross compiler :
================

brew install crosstool-ng --with-make --with-grep

https://www.jaredwolff.com/blog/cross-compiling-on-mac-osx-for-raspberry-pi/
https://gist.github.com/h0tw1r3/19e48ae3021122c2a2ebe691d920a9ca

toolchain options/tools suffix : -7 (to use gcc and not clang)
c library: none
c compiler: gcc 5.2
c compiler: uncheck 'Link libstdc++ statically into the gcc binary
config: <git root dir>/.config
