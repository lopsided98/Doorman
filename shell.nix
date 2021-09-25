{ pkgs }: with pkgs;

mkShell {
  name = "doorman-shell";

  nativeBuildInputs = [
      platformio
  ];
}