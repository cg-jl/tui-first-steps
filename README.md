# tui-first-steps

A basic TUI with input handling and listing. No `ncurses` used.

# Build (linux)

I'm using [premake](https://premake.github.io/) to generate the makefiles.
Make sure you've installed it on your system to continue (usually by the name `premake5` or `premake`).

If you're using GNU Make (which probably you will), then the steps to reproduce are:

- At the root of the project, enter `premake5 gmake`
- `cd` into `./makefiles`
- `make config=release`

The binary will be located (from the project root) in:
`./build/bin/app/release/app`

If you built it in debug mode (defaults to debug mode if no arguments are passed), then just
change `release` for `debug` in the above path.

I really hope it doesn't break....
