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

# Controls

It has two modes, `filter` mode and `list` mode. You press <kbd>Tab</kbd> to switch between them.
<kbd>Ctrl</kbd> + <kbd>q</kbd> quits the program. <kbd>Enter</kbd> also quits the program if you're in list mode.

In filter mode, you just modify the filter string which is used for the filter.
In list mode, you use `j` and `k` to navigate down and up, `G` gets you to the bottom of the list and `H` gets you to the top.

Dead simple, right?
