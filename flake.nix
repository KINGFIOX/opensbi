{
  description = "Clang and LLVM support";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        lib = pkgs.lib;
      in {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            pkg-config
            clang_16
            llvm_16
            libxml2
            cargo-binutils
            (with pkgsCross.riscv64-embedded; [
              buildPackages.gcc
              buildPackages.gdb
            ])
            qemu
          ];
          MAKEFLAGS = "-j$(nproc)";
        };
      });
}
