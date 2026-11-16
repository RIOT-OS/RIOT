{
  lib,
  buildPythonPackage,
  fetchurl,
}:

buildPythonPackage rec {
  pname = "pyhsslms";
  version = "2.0.0";
  format = "wheel";

  src = fetchurl {
    url = "https://files.pythonhosted.org/packages/82/1a/8361466bb51a88a99e1d63c6ec0d1c611def8351b120c0c30ef2840f6689/pyhsslms-2.0.0-py2.py3-none-any.whl";
    hash = "sha256-HZcYITsue5Fkm8ge4OxcI8nDDXpElMAiOTXKH9J1wIM=";
  };

  doCheck = false;
  pythonImportsCheck = [ "pyhsslms" ];

  meta = with lib; {
    description = "HSS/LMS signature verification library";
    homepage = "https://github.com/russhousley/pyhsslms";
    license = licenses.mit;
    maintainers = [ ];
  };
}
