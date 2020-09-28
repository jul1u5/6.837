# vecmath

`vecmath` is a vector library. It provides classes such as `Vector3f`, `Vector4f` and `Matrix4f`, and all sorts of helpful functions that operate on vectors and matrices.

## Compile

```bash
# To generate compile_commands.json for clangd
$ bear make
# Otherwise
$ make
```

Make will generate the following files in `../lib/vecmath`:

- `libvecmath.a` - a static library
- `libvecmath.so` - a shared library
