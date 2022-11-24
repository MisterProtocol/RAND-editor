# The RAND editor "e"

The RAND editor is one of the oldest text editors in existence.  It has
features never duplicated by any other editor.

The contents of this
repository consist of the original code from RAND of e19, ported to Linux
and called e19.59, and then ported to the Mac in 2021.
The "main" branch has additional features, such as mouse support.

The code in the
repository can build binaries of the editor for both Linux and MacOS.

Make sure to see the section labeled "IMPORTANT" in the "Makefiles"
segment below before building the editor.

Small tweaks to the Makefiles are necessary to compile the
code on MacOS Catalina
or subsequent versions of MacOS, because of changes in the location
of the `#include` files.  These changes are discussed in the
Makefiles, particularly e20/Makefile.
If Apple changes the architecture of `XCode` again this is likely to
become an adventure.
The main problem is not with the MacOS compilers finding the `#include`
files.  The compilers are configured to find them wherever they live,
as long as your MacOS developer environment is set up correctly.
The problem is with the dependencies listed in the Makefiles.

The code can also be compiled and run under Windows using WSL, the Windows
Subsystem for Linux, in which case it builds and runs just as it would
on a native Linux system.

### Note for Newcomers

The editor has a command-line "-showbuttons" option which will put
clickable buttons in an area at the bottom of the edit window.  You
should make sure your terminal is at least 92 characters wide for this.
Start typing into the edit window; your text will be entered directly.
After you have some text entered, start clicking the buttons at the
bottom of the screen with the mouse to see what various editor commands
do.

You can start the editor in two ways:

1) `e filename` will edit the named file, or, if it does not exist,
will ask you if you want to create it.

2) `e` with no arguments will start up editing the last file you
edited in the current directory, at the place in the file where you exited
the editor.  If you have not previously run the editor in the current
directory, it will open a new file called `scratch`.

The files in the "man" directory will give you much more extensive
documentation on how the editor works.
We earnestly suggest you read the manual page.
There are several reports listed there, published by The RAND Corporation,
which provide extensive documentation.
These reports may be downloaded for free from http://rand.org.

### Branches

It is intended that there be two branches in this repository, which will
not be merged.  One branch (`original`) contains the code for
the RAND editor as forward-ported to Linux and MacOS, but with no further
additions made, except for bug fixes.  This version is called e19.
It should be noted that the 'original' branch, e19, will have problems
editing files with extremely long lines, due to some problems with
"short" integers in the underlying Line Access library.  This has
been fixed in the newer branch.

The second branch (`main`) contains code developed from the first branch,
which supports use of the mouse, and colored text highlighting on terminal
emulators that support it.  It is expected that most people will be interested
in the second branch.  The first branch exists for historical interest, and
for people who have problems getting the second branch to compile on their
systems.  The first branch, with the original code, reads terminal input
in an idiosyncratic way, as it dates from a "do-it-yourself" era when
third-party libraries were nonexistent.  The second branch handles mouse
support by means of a completely rewritten input section, which uses
`ncurses` to multiplex terminal and mouse input. This version is
called e20.

### Extra files

There are a number of extra files and directories included in the repository
that are not directly involved in building the editor or its helper
executables.  These are present for historical interest, and because
they have been useful in the past in debugging problems with the editor.
"pres.c", for example, will print the editor "state files" in a
human-readable form.  If you're not interested in them they may simply
be removed.

The `t_xcolor` directory contains a complete distribution of the `ncurses`
library, together with other small programs such as `test_keys` which may
prove useful if you choose to redefine the mapping of terminal keys
to editor commands via a keyboard definitions file.

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
and so forth.  Using these terminals, administrative assistants (then
called secretaries) could begin doing useful work in "e" in about half an
hour.

### **OPEN**, **CLOSE**, **PICK**, and **PUT**

Modern editors typically have only one buffer, which is shared by the
"copy" and "paste" operations.  "e" handles things differently.

The terms "copy" and "paste" did not exist when "e" was written.
Instead, "e" uses the terminology **PICK** and **PUT**.  As mentioned
above under "History", RAND had terminals created with special keycaps
labeled **OPEN**, **CLOSE**, **PICK** and **PUT** in a vertical row.
**OPEN** inserted blank lines at the cursor, **CLOSE** deleted text,
**PICK** selected text and put it in a special PICK buffer, and **PUT**
inserted the contents of the PICK buffer at the cursor position.
**CLOSE** also put the deleted text into a buffer, but it was (and is)
a separate buffer from that used by **PICK**.  This buffer could be
re-inserted into the text by pressing the **CMD** key, followed by **PUT**.

As the years went on, and the editor underwent further development, the
**PUT** key fell into disfavor.  It still exists in the editor code, but is
not bound to a key.  Instead, the **CMD** key is used with **PICK** and
**CLOSE** to insert text from the PICK and CLOSE buffers respectively.

Whether the **PUT** key should be re-enabled is currently a matter of
philosophical discussion.

It should be noted that there are several other buffers used by "e" for more
esoteric purposes, such as the "adjust" buffer.  The introduction of these
extra buffers may have influenced the fate of the **PUT** key.

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
#### NCURSES versions
Currently, the editor appears to compile and run without problems
with the ncurses library versions 5.4 and 5.7 using the
terminfo files distributed with those library versions.

The editor also compiles and runs with the 6.2 and 6.3 ncurses libraries.
We did run into a problem initializing mouse position reporting
using the 6.2 distributed terminfo xterm-256color file with an old
xterm terminal emulator that did not support the SGR xterm mouse
protocol.  The SGR protocol was implemented around 2015 and is now
supported by most modern terminal emulators.

(The issue, for those interested, is in the mouse initialization and
parsing definitions used in the terminfo entries "XM" and "kmous".)

An alternative, if using the terminfo files distributed with ncurses-6.2
with a xterm emulator lacking SGR support is to compile and use the
terminal definition "xterm-256color-a" in the "terminfo" subdirectory.
If this is compiled into "~/.terminfo", the editor will run without
problems.  Other alternatives are to use a TERM setting of xterm-color,
xterm-1002, xterm-1003, or xterm-x11mouse.  We also added a
-mouseinit=value option to specify a custom mouse initialization.

On the Mac, we recommend installing and using the third-party program
"iTerm2", available at http://iterm2.com.  Using "TERM=xterm-256color", the
editor runs fine when compiled against ncurses-6.2.
However, if the editor is compiled against ncurses-5.4, it is necessary
to "unsetenv TERMINFO\_DIRS" for iTerm2 to work correctly.
(Recent versions of iTerm2 include a private TERMINFO description file
for "xterm-256color" which does not work well with versions of ncurses
prior to 6.2.  Unsetting the TERMINFO\_DIRS environment variable, which
is set by iTerm2, causes the system definition to be used.  Unsetting
this environment variable in .cshrc or .profile is feasible because
the shell executes these after iTerm2 has set up the environment.)

It should be noted that the 'original' branch, e19, doesn't use any of this,
and in fact uses the older \<sgtty.h> terminal interface.  The newer
interface, \<term.h>, is used by the mouse support code in branch 'main'.
However, the code still includes <sgtty.h> for some of its code.  These
two include files are not supposed to be used together, but our
mandate was not to rewrite the entire editor.  Therefore, there is
a collection of '#undef' directives in the code to allow these
two include files to coexist without causing multiple definition
errors.  Yes, this is ugly.  No, we're not fixing it.  If you want
to see ugly, take a look at the code in "GetLine()".


### Filter Programs

The editor uses external executable files as filters, which can process the
text being edited.  The editor takes marked text, feeds it to the filter,
and replaces the text with the filter's output.  There are three main
executables which are regarded as part of the editor: `fill/just`,
`center`, and `run`.  `fill/just` are the same executable, linked to by two
names.  The `fill` link will fill in ragged text to a margin given in an
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
crashes, or if the editing session is aborted via "exit abort", this
file is preserved, and may be used to restore the work done in the
aborted session.  If a ".ek1" file is found when the editor is started,
with or without arguments, the editor will present the user with several
options, including replaying the previous edit session, silently
recovering the previous edit session, ignoring the previous session
(and proceeding with the current command as given by the user), or
exiting without doing anything.

### Other Programs

There are two other programs which may be of use, both in the directory
"e20".  The first is "pres.c", may be used to present the contents of
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

### Ancillary Files ###

At times, the editor must display messages longer than a single line to
inform the user of certain events.  These files are installed by some
of the targets in the top-level Makefile, and are placed into the same
directory (usually `/usr/local/lib/e`) as the filter programs.  Among
them are `Crashdoc`, `dummy`, `errmsg`, `helpkey` and `recovermsg`.

### Keyboard Files ###

The editor's keyboard may be remapped.  This is not done by entries in
a profile file, but by specially-constructed keyboard mapping files.
Examples are included with the editor, notably `kb.mac`, which solves
certain problems when using "e" on a Mac.  See also etc/kb_ncurses
which includes examples to map various Fn keys.

### Makefiles

There are two Makefiles of particular interest in this distribution.  One
is the top-level Makefile, which is used to actually build the editor and
its ancillary programs, `fill/just`, `center`, and `run`.  It contains
definitions to specify the installation location for the various pieces of
the editor.

The second Makefile, in `e20/Makefile`, contains the information necessary to
actually compile the editor, including termcap compilation options, the
locations of `#include` files, and so forth.  Both Makefiles should be read
carefully and changed as necessary for the local environment.  "e" missed
out on the whole `config` thing by quite a few decades.  (Personal note:
Frankly, I'm surprised it even uses `make`.  It didn't used to.  In the
old days it was compiled by a shell script. --MOB)

IMPORTANT: In building "e", the first thing you should do is change
to the `e20` directory and run either `make sys5.depend` or `make bsd.depend`,
depending on whether you're building the editor for Linux or MacOS.
You only need to do this once, unless you change the code in such a
way as to change which `#include` files are included via a `#define`.

A note about man/Makefile: The "e" manpage uses nroff/troff diversions
(the .di command).  However, the current "man" command, in a brilliant
example of reinventing the wheel, uses a rewrite of nroff/troff called
"mandoc", and it does not support diversions.  These are (mostly) harmless
in normal "man" output, but building the PostScript version of the man
page has problems with the blizzard of "mandoc" errors.  Hence there
is a bit of fast footwork in man/Makefile to get rid of the mandoc errors
before the PostScript generator can see them.

Alternatively, `man -t e` will prevent the use of "mandoc" entirely,
and produce PostScript output with diversions handled properly by
`groff`.
Capture the output in "e.ps", and read that with an appropriate reader
for a much more legible man page.
For a third alternative, go to /etc/man.conf and change the "-mandoc" references
to "-man", which prevents use of "mandoc" in favor of forcing the use
of the full `groff` program.  Test thoroughly on this one!

Finally, using Homebrew or MacPorts to install "man-db" will give you a "gman"
command, which, unlike Apple's installed "man" command, does not hardwire
a call to "mandoc" into the binary.  (Apple claims to have done this because
Darwin OS does not have "roff".  It has "nroff" and "troff" both, so
this is jejune and stupid.)  The "gman" command renders regular man pages
as well as the "e" man page without muss, fuss or bother.

### Extensions - Window resizing

The editor now supports resizing of both the containing terminal
window, and the editor's own internal windows.
Either type may be resized by the usual method of dragging a
border.  This is a very new feature, as originally the editor
ran on hardware terminals, where resizing the screen was
not possible.

### Extensions - `ctags` and `tags` files

Editors such as `vi/vim` and `emacs` have the ability to use
`tags` files, created by external programs such as `ctags`.
When an editor supporting the use of tags first starts, it looks
in the current directory for a file named `tags`.
If present, this file contains an index of the definition of
every function defined in every source code file in the
directory.
This allows the programmer to quickly find the definition
of any function referred to in the code.

The RAND editor now supports the use of `tags` files.
A **tag** command has been introduced which will use the
`tags` file to find the definition of the function whose
name is under the cursor.

Because of the age of the editor's code, it uses comment blocks
which confuse the `ctags` utility.
Therefore, to generate a `tags` file for use with the
editor's source code, a helper shell script, `ctags.sh`,
is used by the top-level `Makefile` to create the `tags`
file for the editor source.
Because most editing is expected to be done in the
subdirectory containing the bulk of the editor's
source code, the `tags` file is not built at the top
level, but is placed in the `e20/` directory.

The helper script `ctags.sh` will probably need to be edited for your
system depending on whether your system runs Linux or MacOS,
and where your `ctags` program is located.  There is an alternative
version of the `ctags.sh` script, `ctags_trw.sh`, which you
may wish to run instead.  It does the same thing as `ctags.sh`,
except that instead of including all source files in the entire
directory tree in the `tags` file, it indexes only a preselected
subset of source files.  Which files are included are listed in
the file `tagfiles` in each subdirectory.  Depending on whether or
not you are editing test programs in the "e" source directory, you
may wish to run this version instead, to avoid extraneous
entries in the `tags` file.  Initial versions of the `tagfiles`
files have been included in each subdirectory.

N.B.: If you choose to run `ctags_trw.sh`, and if you then add
a new source file to any of the directories, you will have to add
that file to the local `tagfiles` file.  These tag files are
constructed by hand, not automatically.

### Shell files for Macs

Modern versions of MacOS hate like crazy to leave core dump files.  To get
an "e" core dump file for debugging purposes, you have to
issue "limit core unl" to the top-level shell, then run a couple of
shell files in the top-level distribution directory.  The file
"entitlements.sh" creates the file "tmp.entitlements", which contains
magic code to allow a core dump to happen.  Then, the shell
file "coredump.sh" uses this to sign a freshly-created "e20/le"
binary executable.  "tmp.entitlements" only has to be created once.
"coredump.sh" has to be re-run on every newly-compiled editor binary
that you want to be able to dump core.  Typically you want a core
dump so you can examine what went wrong using the "lldb" debugger.
Unlike the early days of UNIX, core dump files are not left in the
current directory; they're left in "/cores" in a file named with the
process ID they represent.  Cleaning out "/cores" is therefore not
only to reclaim space (MacOS core dumps are unbelievably huge) but
to avoid confusion.

### What to Do First

Read the `README` file.  Read the top level `Makefile`, and `e20/Makefile`.
Do what those Makefiles say.  The most common way to build the editor,
after everything has been set up, is to say `make clean bsd` or
`make clean s5` at the top level.
Before installing, you should test the editor.  The editor executable
is left in `e20/le`.  The origin of the name `le` as opposed to `e`
is lost to history.
Note that this will not build the
filter executables or the `man` files.  Look in the subdirectories
such as `fill` and `man` for those. The top-level Makefile has
additional targets "etc", "help", "clean", "bsd.man" and
"s5.man" which will be useful. Finally, `make bsd.install` or
`make s5.install` will install the editor on your system.

### Final Secrets

Adding a new source file to the editor is a complex process, due to
the age of the editor source tree.

For example, to  a file to the `e20/` directory, follow these
instructions:

* If it's a header file, add it to the HFILES definition in `e20/Makefile`.
* If it's a C source file, add it to the OFILES (in .o form), CFILES (in .c form), and the E compilation target (in .o form).
* Add a segment similar to the others in the section which gives build rules for each `.o` file individually.
* Run `make depend` to pick up dependencies.

The process is identical for adding source files to the `ff3` and `la1`
directories, although this should not be necessary unless you are Very
Far Into the Weeds.

Adding a source file to the `lib` directory is simpler, but again,
this should not be necessary.

If you are using `ctags` with the `ctags_trw.sh` script, be sure to
add the new file to the `tagfiles`file in the corresponding
directory.