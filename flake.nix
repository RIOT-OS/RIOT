# just run "nix develop" 
{
  description = "riot-c-flake";

  inputs = {
    flake-compat.url = "https://flakehub.com/f/edolstra/flake-compat/*.tar.gz";

    flake-schemas.url = "https://flakehub.com/f/DeterminateSystems/flake-schemas/*.tar.gz";

    nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/*.tar.gz";
  };

  outputs = { self, flake-compat, flake-schemas, nixpkgs }:
    let
      # Helpers for producing system-specific outputs
      supportedSystems = [ "x86_64-linux" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in {
      # Schemas tell Nix about the structure of your flake's outputs
      schemas = flake-schemas.schemas;

      # Development environments
      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell {
          # Pinned packages available in the shell environment
          packages = with pkgs; [
            curl
            git
            jq
            wget
            nixpkgs-fmt
            gcc-arm-embedded
            python3Full
          ];
        };
      });
    };
}
