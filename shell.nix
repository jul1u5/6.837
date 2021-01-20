{ pkgs }:
let
  inherit (pkgs) lib;
  mkShell = pkgs.mkShell.override { stdenv = pkgs.llvmPackages_11.stdenv; };
in
mkShell {
  buildInputs = lib.attrValues {
    inherit (pkgs)
      libGL
      libGLU
      freeglut

      ccls
      bear
      ;
  };
}
