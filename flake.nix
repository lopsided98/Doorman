{
  description = "Doorman Arduino firmware";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, }:
    with flake-utils.lib;
    eachSystem defaultSystems (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in rec {
      devShell = pkgs.callPackage ./shell.nix { };
    });
}