{ pkgs ? import <nixpkgs> { } }:
let
  mkShell = pkgs.mkShell.override { stdenv = pkgs.llvmPackages_10.stdenv; };
in
mkShell {
  buildInputs = with pkgs; [
    libGL
    libGLU
    freeglut

    bear
  ];
}
