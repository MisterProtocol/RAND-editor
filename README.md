# The RAND editor "e"

The RAND editor is one of the oldest text editors in existence.  It has
features never duplicated by any other editor.

The contents of this
repository consist of the original code from RAND of e19, ported to Linux
and called e19.59, and then ported to the Mac in 2021.

The code in the
repository can build binaries of the editor for both Linux and MacOS
Mojave.  Small tweaks to the Makefiles are necessary to compile the
code on MacOS Catalina
or subsequent versions of MacOS, because of changes in the location
of the `#include` files.  These changes are discussed below.

The code in "e" is very old, and much of the reason for the creation
of this repository is to preserve that history.  Consequently, there
are many files in the distribution which are not involved in the build
process, but which remain to provide guidance in the case that the editor
must be further altered for some new environment: they lend a third
dimension to the active code, and give examples of how certain
changes might be made.

Working with terminals, of course, lies at the heart of the RAND editor.
Therefore, there are a number of auxiliary files that the editor can use.
Among these are terminal description files and keyboard description files.

In its early days at RAND, the editor was intended for use not only by
researchers, but also by support staff, who had no workd processing
experience whatsoever and had, to this point, used typewriters exclusively.
Therefore, RAND special-ordered ASCII terminals with the keyboards rewired
to send control characters whenever function keys or keys on the numeric
pad were typed.  The editor is a "modeless" editor (mostly!) in that any
alphanumeric characters typed will be inserted directly into the file; it
has only a very limited "command mode" for typing commands to the editor.
Almost all commands to the editor are typed as one or two control
characters.  These hotwired terminals were then provided with
specially-made brightly colored keycaps with legends like **OPEN**,
**PICK**, **PUT**,
and so forth.  Using these terminals, administrative assistance (then
called secretaries) could begin doing useful work in "e" in about half an
hour.

All of this is by way of saying that, as "e" spread to different (some very
different) types of terminals, it was necessary to provide "e" with
information on how these terminals worked.  This split into two features:
the terminal descriptor files, which were compiled into the editor, and the
keyboard descriptor files, which could be located outside the editor.
Samples of each are provided in the distribution.  In particular, the
distribution contains a `kb.mac` file, which allows Mac users to use the
arrow keys, Page Up, Page Down and Home keys.  Setting the environment
variable `EKBFILE` to a path to this file will allow the editor to more
fully respond to the keyboard on a Mac in a Terminal app window.

If the TERM environment variable is set to a terminal type for which "e"
has a built-in terminal description file, "e" will use the internal
information.  Otherwise, "e" will use either `terminfo`` or `termcap`
terminal descriptions, depending on how it is compiled (see below).

The editor uses external executable files as filters, which can process the
text being edited.  The editor takes marked text, feeds it to the filter,
and replaces the text with the filter's output.  There are three main
executables which are regarded as part of the editor: `fill/just`,
`center`, and `run`.  `fill/just` are the same executable, linked to by two
named.  The `fill` link will fill in ragged text to a margin given in an
argument, or to a default margin if no argument is given.  `just` does the
same thing, but right-justifies the text.  In today's environment of
variable-width fonts and kerning, `just` is probably of little use, but
`fill` is still quite handy.  `center`, as the name implies, centers text
sent to it.  `run` is a bit different.  It will run an arbitrary program
given in its argument as a filter, replacing the input text with the output
text.  `sort` is one possible argument, for example.

There are two Makefiles of particular interest in this distribution.  One
is the top-level Makefile, which is used to actually build the editor and
its ancillary programs, `fill/just`, `center`, and `run`.  It contains
definitions to specify the installation location for the various pieces of
the editor.

The second Makefile, in `e19/Makefile`, contains the information necessary to
actually compile the editor, including termcap compilation options, the
locations of `#include` files, and so forth.  Both Makefiles should be read
carefully and changed as necessary for the local environment.  "e" missed
out on the whole `config` thing by quite a few decades.  Frankly, I'm
surprised it even uses `make`.  It didn't used to.  In the old days it was
compiled by a shell script.
