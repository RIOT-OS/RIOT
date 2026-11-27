{
  lib,
  buildPythonPackage,
  fetchurl,
  pexpect,
  psutil,
  setuptools,
}:

buildPythonPackage rec {
  pname = "riotctrl";
  version = "0.5.1";
  format = "setuptools";

  src = fetchurl {
    url = "https://files.pythonhosted.org/packages/ee/75/2ca1d57d6f7083da9b96c42a19ce67a72c43a454e7fe8e5c9a904f22cd3a/riotctrl-0.5.1.tar.gz";
    hash = "sha256-PIdMlWwSKg3VUfq2Wtwuj47ZRCLiwHOvxCI2QI9Kiio=";
  };

  propagatedBuildInputs = [
    pexpect
    psutil
  ];

  # The test suite requires a running RIOT node.
  doCheck = false;

  pythonImportsCheck = [ "riotctrl" ];

  meta = with lib; {
    description = "Python abstraction for RIOT nodes";
    homepage = "https://github.com/RIOT-OS/riotctrl";
    license = licenses.mit;
    maintainers = [ ];
  };
}
