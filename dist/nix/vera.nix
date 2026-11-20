# vera++ is not packaged in nixpkgs.  RIOT only uses its Tcl rules, so build
# it without the optional Python and Lua bindings (which pull in a large,
# outdated dependency stack).
{
  lib,
  stdenv,
  fetchurl,
  cmake,
  boost186,
  tcl,
}:

stdenv.mkDerivation rec {
  pname = "vera++";
  version = "1.3.0";

  src = fetchurl {
    url = "https://github.com/verateam/vera/archive/refs/tags/v${version}.tar.gz";
    hash = "sha256-MtHSm+jsllVvoJNdkI0mJ9r/vxF6vRqmOfWhxkrhDOs=";
  };

  nativeBuildInputs = [ cmake ];
  buildInputs = [
    boost186
    tcl
  ];

  # Boost 1.70+ removed boost::filesystem::path::normalize().
  postPatch = ''
    sed -i 's/\.normalize()/.lexically_normal()/' src/executable_path.cpp
  '';

  cmakeFlags = [
    "-DVERA_PYTHON=OFF"
    "-DVERA_LUA=OFF"
    "-DVERA_USE_SYSTEM_BOOST=ON"
    # The project still declares CMake 2.8 compatibility.
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
  ];

  meta = with lib; {
    description = "Static analysis and style checker for C++";
    homepage = "https://github.com/verateam/vera";
    license = licenses.boost;
    mainProgram = "vera++";
    platforms = platforms.unix;
  };
}
