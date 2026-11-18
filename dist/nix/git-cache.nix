{
  lib,
  rustPlatform,
  fetchFromGitHub,
}:

rustPlatform.buildRustPackage rec {
  pname = "git-cache";
  version = "0.2.8";

  src = fetchFromGitHub {
    owner = "kaspar030";
    repo = "git-cache-rs";
    rev = version;
    hash = "sha256-Z0YJiOX7VYhNwLmn1hkdCXdDiSY6O8KEyOd2j/Z5KJk=";
  };

  cargoHash = "sha256-tq0a8FdAzls1bHTENNZ/YVzrRpc6TKddbK38X0IJxA8=";

  meta = {
    description = "A git clone cache";
    homepage = "https://github.com/kaspar030/git-cache-rs";
    license = lib.licenses.asl20;
    mainProgram = "git-cache";
    platforms = lib.platforms.unix;
  };
}
