{
  description = "C SHELL READ";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      # Package build for c-shell
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "cshread";
        version = "0.1.0";
        src = ./.;

        buildInputs = [ pkgs.clang ];

        buildPhase = ''
          make build
        '';

        installPhase = ''
          mkdir -p $out/include/cshread
          cp -r include/*.h $out/include/cshread
          mkdir -p $out/lib
          cp build/libcshread.a $out/lib/
        '';
      };

      # Default package alias
      defaultPackage.${system} = self.packages.${system}.default;

      # DevShell definition
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = [
          pkgs.clang-tools
          pkgs.clang
          # For generating compile_commands.json
          pkgs.bear
          # debugging
          pkgs.valgrind
          pkgs.lldb
        ];

        buildInputs = [
          pkgs.libcxx
          # Unit testing
          pkgs.cmocka
        ];

        # Set CPATH environment variable
        shellHook = ''
          export CPATH=${pkgs.lib.makeSearchPathOutput "dev" "include" [ pkgs.libcxx ]}:${pkgs.lib.makeSearchPath "resource-root/include" [ pkgs.clang ]}
        '';
      };
    };
}
