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

### Branches

It is intended that there be two branches in this repository, which will
not be merged.  One branch (`original`)contains the code for the RAND editor as
forward-ported to Linux and MacOS, but with no further additions
made, except for bug fixes.  The second branch (`main`) contains code developed
from the first branch,
which supports use of the mouse, and colored text highlighting on terminal
emulators that support it.  It is expected that most people will be interested
in the second branch.  The first branch exists for historical interest, and
for people who have problems getting the second branch to compile on their
systems.  The first branch, with the original code, reads terminal input
in an idiosyncratic way, as it dates from a "do-it-yourself" era when
third-party libraries were nonexistent.  The second branch handles mouse
support by means of a completely rewritten input section, which uses
`ncurses` to multiplex terminal and mouse input.

### Extra files

There are a number of extra files and directories included in the repository
that are not directly involved in building the editor or its helper
executables.  These are present for historical interest, and because
they have been useful in the past in debugging problems with the editor.
"pres.c", for example, will print the editor "state files" in a
human-readable form.  If you're not interested in them they may simply
be removed.

### History

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
researchers, but also by support staff, who had no word processing
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

### Terminal Handling

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
information.  Otherwise, "e" will use either `terminfo` or `termcap`
terminal descriptions, depending on how it is compiled (see below).
#### "ncurses"
The original version of the editor does its own input and terminal
handling, without reference to external libraries, although it can
make use of `curses` to write to terminals that do not have built-in
support.
However, the newer version, with mouse support, uses the `ncurses`
library to multiplex input from the mouse.

### Filter Programs

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

### Editor-created files

The editor creates several files beginning with "." in the normal course
of use.  The first is the "state file", whose name begins with ".es1".
This file contains everything needed to restore the previous edit session
to its state when the editor exited, including the name(s) of file(s) being
edited, the location and size of windows, etc.  The other file, the
"keystroke file", has a name beginning with ".ek1".  It contains all of
the keystrokes from the current editing session.  If the session exits
normally, this file is deleted on exit.  If the editor (or the system)
crashes, or if the editing session is aborted via "bye abort", this
file is preserved, and may be used to restore the work done in the
aborted session.  If a ".ek1" file is found when the editor is started,
with or without arguments, the editor will present the user with several
options, including replaying the previous edit session, silently
recovering the previous edit session, ignoring the previous session
(and proceeding with the current command as given by the user), or
exiting without doing anything.

### Other Programs

There are two other programs which may be of use, both in the directory
"e19".  The first is "pres.c", may be used to present the contents of
a ".es1" editor state file in human-readable form.  It should be noted
that historically, the version number of the editor has been increased
whenever the format of the state file has been changed.

The other program is "e.showkeys.c", which is of recent vintage, and may
be used to print the contents of a ".ek1" keystroke file from a crashed
or aborted session.  In particular, it allows the user to determine
how far they want the recovery to continue, that is, the user may want
to skip the last few keystrokes if they caused the editor to exit or
crash.  This may be done by giving an optional argument to the editor
prompt which asks the user whether to recover the session or not.
Currently, option "2" recovers the previous session, so answering
"2 5" would recover all but the last five keystrokes of the previous
session.  If those keystrokes were "<cmd>b q<ret>", this would prevent
the editor from exiting at the end of the recovery.

### Makefiles

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

### Highlighting

This version of the editor supports highlighting of **MARK**ed
text using colors,
on terminals which support colored text.  The default highlighting
uses dark text on a light gray background.  Arbitrary colors may be chosen
instead using command-line arguments; see the man page for details.

To assist in choosing pleasing colors, a directory `t_xcolor` is included
with programs to display possible color combinations.  N.B.: These programs
have not been extensively tested on a variety of systems; some work
may be needed to compile them.

### What to Do First

Read the `README` file.  Read the top level `Makefile`, and `e19/Makefile`.
Do what those Makefiles say.  The most common way to build the editor,
after everything has been set up, is to say `make clean bsd` or
`make clean s5` at the top level. 
Before installing, you should test the editor.  The editor executable
is left in `e19/le`.  The origin of the name `le` as opposed to `e`
is lost to history.
Note that this will not build the
filter executables or the `man` files.  Look in the subdirectories
such as `fill` and `man` for those.  The top-level Makefile has 
additional targets "etc", "help", "clean", "bsd.man" and 
"s5.man" which will be useful. Finally, `make bsd.install` or
`make s5.install` will install the editor on your system.
