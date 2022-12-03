# RTL: RunTime Library

RTL is a very small (partial) substitution of the C++ standard runtime library with some additional features. 

## About the Project

RTL is useful for making Windows native applications with extremely small size (from several kilobytes) without any dependencies except WinAPI system libraries. It is implemented in header-only style. 

## TODO

```
- [ ] Add self-test application
- [ ] Add demo application: null with minimal size
- [x] Add demo application: noising window
- [ ] Support for other C++ compilers (clang, gcc)
- [ ] Support for x64 platform
- [ ] Resolve TODOs from source files
- [ ] Implement profiler
- [ ] Add optional "Retry, Ignore, Abort" loop to RTL_OPENCL_CHECK, RTL_WINAPI_CHECK, RTL_ASSERT, RTL_MM_WAVEOUT_CHECK
- [ ] Implement lightweight release checks with the hash code of __FILE__ and __LINE__
- [ ] Implement STL-like API for rtl::random
