{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  buildInputs = [
    gcc

    libGL
    libGLU
    freeglut

    cpplint
    cppcheck
  ];
}
