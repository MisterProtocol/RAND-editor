


          E(1)                      USER COMMANDS                      E(1)



          NAME
               e - the Rand editor, E

          SYNOPSIS
               e [ options ] [ mainfile [ altfile ] ]

          DESCRIPTION
               The Rand editor _E is a full screen  editor  which  treats  a
               text  file  as  a  two-dimensional  surface with text on it.
               Among its more unusual features are manipulating rectangular
               areas of text, editing several files at once, and displaying
               text in one or more windows.  While it is not as powerful as
               _E_m_a_c_s,  or  as widely used as _V_i, most people at Rand prefer
               its easy to learn commands and use it to edit both  programs
               and text.  _E is a complete rewrite of the Rand _N_E_D editor of
               several years back.  The  current  version  (E19)  has  been
               improved  to take advantage of terminals that have the abil-
               ity to do local insert/delete  line,  and  it  can  now  use
               _t_e_r_m_c_a_p so that most terminals with direct cursor addressing
               can be used.

               Please note that this manual entry does not try to be either
               a  tutorial or a full reference manual for the editor.  Rand
               users should initially learn _E by reading the  CSD  document
               _S_e_l_f  _T_e_a_c_h_i_n_g  _G_u_i_d_e _t_o _T_e_x_t _P_r_o_c_e_s_s_i_n_g, available from the
               CSD documentation center.  There is  currently  no  tutorial
               available  for  non-Rand  users.  There is a draft reference
               manual available, _T_h_e _R_a_n_d _E_d_i_t_o_r, _E,  _A_p_r_i_l  _1_9_8_4  that  is
               included in the E/MH distribution for non-Rand users.

               The _m_a_i_n_f_i_l_e on the command line is the file which  is  ini-
               tially  displayed in the editor.  If the file doesn't exist,
               the user will be asked if it should be created.  If in addi-
               tion,  an  _a_l_t_f_i_l_e is specified on the command line, it will
               be read in as the initial alternate file.  If  no  filenames
               are  present on the command line, _e will attempt to continue
               from your previous editing session.  To  do  this  it  saves
               "state"  in  a  file  called  '.es1' in your current working
               directory.  Before invoking the editor, be sure and  specify
               your terminal type by setting the TERM environment variable.

               Refer to section RECENT CHANGES for a list of new changes to
               E.


          OPTIONS
               The currently available command line options are:

               -help
                    This  causes  a  display  of  the  currently  available
                    options  and their values along with the version number
                    of the editor.



          Sun Release 1.1         Last change: Rand                       1






          E(1)                      USER COMMANDS                      E(1)



               -bullets or -nobullets
                    Options -bullets and -nobullets forces  border  bullets
                    on  or  off  regardless of line speed or terminal type.
                    Line speeds of 1200 baud and slower and  some  terminal
                    types  set  border  bullets off in the absence of these
                    options.

               -inplace
                    Option  -inplace  preserves  file  links   except   for
                    singly-linked  files.   From  within  the  editor, link
                    preservation  can  be  turned  on  and  off,  even  for
                    singly-linked files, by the update command.

               -terminal=termtype
                    Option -terminal=termtype identifies the terminal  type
                    explicitly.  In the absence of the -terminal option the
                    terminal type is retrieved from  the  TERM  environment
                    variable.

                    If there is no compiled-in code for the  terminal,  the
                    termcap  entry  for  the terminal type will be used and
                    the "standard" keyboard will  be  assigned.   Refer  to
                    TERMINALS SUPPORTED section of this document for termi-
                    nals that can be supported by compiled-in code  (varies
                    with installation).

               -keyboard=termtype
               -dkeyboard=termtype
                    Option -keyboard=termtype sets  the  keyboard  type  to
                    "termtype".  It overrides any "-terminal=" argument and
                    also any EKBD environment variable.

               -dtermcap
                    Option -dtermcap forces _E to use the termcap entry  for
                    the terminal identified by the TERM variable or -termi-
                    nal option,  disregarding  any  applicable  compiled-in
                    code.

               -notracks
                    Option -notracks will allow you to edit a file  without
                    using  or  disturbing  the work files (keys, state, and
                    change files) from your previous session.

               -replay=filename
                    Option -replay=filename allows you  to  replay  with  a
                    specified  keys  file, where "filename" represents that
                    keys file.  Use -silent when the replay display is  not
                    desired.    The   message   "***replay   completed"  or
                    "***replay aborted" signals the end of the replay.  The
                    <INT>  key can be used to stop the replay short of com-
                    pletion.  Files .ec1* and .ek1* must not be present  if
                    the replay is to work.



          Sun Release 1.1         Last change: Rand                       2






          E(1)                      USER COMMANDS                      E(1)



               -norecover
                    Option -norecover will open the edit session as defined
                    by  the .es1 state file, ignoring any recovery process-
                    ing of a prior crashed or aborted session.

               -regexp
                    Option  -regexp  enables  regular  expression   pattern
                    matching  for  searching  and  replacing.  This mode is
                    also enabled by the  "re[gexp]"  command.   In  regular
                    expression mode, "RE" is displayed at the bottom of the
                    screen.  The command "-re[gexp]" returns to non-regular
                    expression  mode.   The  regular expressions recognized
                    are essentially the same as that for _e_d.

               -state=filename
                    Option -state=filename causes the initial state of  the
                    editor to be read from the indicated "state" file.  The
                    state file from a previous _E  session  (with  alternate
                    files,  windows, cursor positioning, etc.) may be saved
                    by exiting, and renaming the state file (".es1").

               -[no]stick
                    Option -stick sets the behavior at the right margin  to
                    stop  in response to text or a <RIGHT ARROW>.  -nostick
                    is the default, and will automatically shift the window
                    right in response to text or the <RIGHT ARROW> key.

               -readonly
                    Option -readonly  will  prohibit  any  changes  to  the
                    file(s).

               -noreadonly
                    Option -noreadonly will disable the (new e19) mechanism
                    preventing  changes  to files lacking write permission,
                    that you could previously change because you had  write
                    permission in the directory.

               -noprofile
                    Option  -noprofile  will   prevent   reading   of   the
                    ".e_profile".  (See _c_a_v_e_a_t under STARTUP FILE.

               -profile=file
                    Specifies the profile to be used in lieu of the default
                    "./.e_profile"  or "~/.e_profile".  See STARTUP FILE in
                    a later section.

          ENVIRONMENT VARIABLES
               The TERM environment variable  defines  the  terminal  type.
               Refer  to TERMINALS SUPPORTED for those terminal which _e has
               terminal specific code compiled in.  If there is no compiled
               in code for the terminal _e will look in "/etc/termcap" for a
               definition of the terminal.  The -dtermcap option will force



          Sun Release 1.1         Last change: Rand                       3






          E(1)                      USER COMMANDS                      E(1)



               _e to use the "termcap" definition.

               The environment variable EKBD defines the  keyboad  type  if
               different from the TERM type.

               The EDITALT environment variable will be used as the name of
               the  alternate  file.   If an alternate filename is given on
               both the command line and with  the  EDITALT  variable,  the
               command  line  file is used.  The EPROFILE environment vari-
               able will be used as the name of the "startup" file, in lieu
               of  "./.e_profile"  or  "~/.e_profile".   The  command  line
               option -profile=file wins if both are used.

          FILES
               _E creates and maintains several ".e" files.  They  are  used
               to  keep  track  of the current editing session and can nor-
               mally be ignored by the user.  The following information  is
               for the curious.

               File .es1 contains state information on the last  edit  ses-
               sion.   It  is  created  or updated at the end of a normally
               terminated edit session.

               File .ec1 indicates the last edit session in this  directory
               crashed.

               File .ek1 contains a history of the last edit sessions  key-
               board inputs.  The .ek1 file is updated at regular intervals
               so that a mimimum of work is lost on edit or system crashes.

               File .ek1b is the .ek1 file prior to a recovery replay.

               The presence of any .ek1* file indicates that the last  ses-
               sion  in  this  directory abnormally terminated and that the
               next edit session will do recovery processing unless  option
               -norecover  is  used.   File  .ek1  means  abort, .ec1 means
               crash.

               Note that if the login user is not the owner of the  current
               directory  the  above  files will be suffixed with the login
               users name.  If the user has  no  write  permission  in  the
               current  directory, these files are created in the directory
               /tmp/etmp.

               There are also a few utility files in the directory /etc/e.

          RECENT CHANGES
               E19 changes include:
               o+  a word processing mode (WP) that currently features "word
                  wrap" and support for a "left margin";
               o+  a "startup" file mechanism (e.g., ~/.e_profile) to  indi-
                  vidually   tailor  options;  also  provides  support  for



          Sun Release 1.1         Last change: Rand                       4






          E(1)                      USER COMMANDS                      E(1)



                  "editor scripts"
               o+  a delete word function;
               o+  options to control the behavior when text is  typed  past
                  the right edge of the window;
               o+  an alternate syntax for the "goto" command;
               o+  display of tab settings on the top border, and  left  and
                  right margin (width) settings on the bottom border;
               o+  an option for invoking _E with a previous state file;
               o+  a new help facility;
               o+  improved support for the "track" command;
               o+  a new command "caps" to convert lower case to upper case;
               o+  a new command "ccase" to reverse the case of the affected
                  text;
               o+  changes to _f_i_l_l/_j_u_s_t to no longer filter out CTRL  chars,
                  and (by default) not split hyphenated words at the end of
                  a line;
               o+  a  new  "-readonly"  option  to   prevent   unintentional
                  changes; files that are not writable cannot be modified;
               o+  improved VT100 terminal support; and terminal drivers for
                  the Beehive MicroBee, Televideo 910, Liberty Freedom 100,
                  and Wyse 50.
               o+  the sequence "<CMD><WINRIGHT>" now shifts the window  far
                  enough right to display the end of the current line (pre-
                  viously, this sequence would set the  left  edge  of  the
                  window at the cursor position).
               o+  bug fix: if an _a_l_t_f_i_l_e was specified on the command line,
                  and the environment variable EDITALT was set, the _a_l_t_f_i_l_e
                  is now used

               E18 changes include:
               o+  support for regular expression searching and replacing;
               o+  implementation of the +WORD and -WORD functions;
               o+  a new set command to individually tailor various  parame-
                  ters (e.g., the number of cols a <WIN RIGHT> will shift;
               o+  support for symbolic links (the  link  is  read  and  the
                  referenced file is used);
               o+  now runs on System III, System V, and 68000s;
               o+  commands that specify filenames  (e.g.,  "  CMD:  e  _f_i_l_e
                  expands a leading "~" to the appropriate home directory;
               o+  compiled in code for Ann Arbor Ambassadors now  downloads
                  the standard keyboard into the function keys;
               o+  support added for Sun2 workstation;

          COMMANDS
               The following commands are available to the user by  opening
               the command line via the <CMD> key and entering the command.
               The entered command is parsed and executed when the <RETURN>
               key is typed.

               In the following discussion, pound sign ("#")  introduces  a
               comment,  brackets  ("[","]") indicate options, and vertical
               bar ("|") indicates optional choices.



          Sun Release 1.1         Last change: Rand                       5






          E(1)                      USER COMMANDS                      E(1)



               There are two types of areas: a range of lines (indicated by
               <range>)  and  a marked retangle of text (indicated by <rec-
               tangle>).  Some commands will take either type of area, this
               is  indicated  by <area>.  A <range> can be specified either
               by marking or by a number followed by "l" for lines  or  "p"
               for  paragraphs;  rectangular  areas  must  be  marked.  The
               default range is one line for most  commands  (fill,  adjust
               and center default to one paragraph).

               b | bye | ex | exit [ nosave | quit | abort | dump ]
                    Exit the editor.  The  different  exit  options  affect
                    whether  or  not the editor updates the edited files or
                    not, and whether the ".e" state files are changed.  The
                    following table give the complete exit story.

                                 Saves  Update  Remove     Remove
                                 Files  State   Keys file  Change file

                    exit          X      X       X          X
                    exit nosave   -      X       X          X
                    exit quit     -      -       X          X
                    exit abort    -      -       -          X
                    exit dump     -      -       -          -

               blot        adjust | close | erase | pick | run | box
               -blot       adjust | close | erase | pick | run | box
               cover       adjust | close | erase | pick | run | box
               insert      adjust | close | erase | pick | run | box
               overlay     adjust | close | erase | pick | run | box
               underlay    adjust | close | erase | pick | run | box

                    All of these commands take the  named  buffer  (one  of
                    adjust,  close,  erase,  pick,  run, or box) and add or
                    merge it into the current file.

                    Insert takes the named buffer, moves the existing  text
                    to make room for it, and then inserts it at the current
                    text location.  It is identical to the  -pick  command,
                    except  that  now  the  other named buffers can also be
                    inserted.

                    Cover places the named buffer  over  top  of  the  text
                    without moving anything.

                    Overlay is like cover, except that  only  the  printing
                    characters in the buffer clobber the original text.

                    Underlay is the reverse of  overlay,  i.e.  the  buffer
                    covers only the non-printing characters of the original
                    text.





          Sun Release 1.1         Last change: Rand                       6






          E(1)                      USER COMMANDS                      E(1)



                    Blot is like overlay except that positions in the  file
                    corresponding  to printing characters in the buffer are
                    erased.

                    -blot is like blot except that positions  in  the  file
                    corresponding to blanks in the buffer are erased.

               box <rectangle>
                    Draws a box around the rectangle with "+", "|", and "-"
                    characters.

               caps <rectangle>
                    Caps changes the "target text" to all upper case  char-
                    acters  (only  the  characters  a-z are affected).  The
                    _m_a_r_k command is first used to define the "target text".

               ccase <rectangle>
                    Ccase changes all upper case to  lower  case  and  vice
                    versa.   The affected area is first defined by the _m_a_r_k
                    command.

               center  [<range>] width=n
               fill    [<range>] width=n
               justify [<range>] width=n
                    These commands all act upon the range of text and  then
                    replace them with the modified results.  Center centers
                    the text around the column (width/2),  fill  places  as
                    many  words  as  possible  on each line, and justify is
                    like fill except that it also provides a smooth  right-
                    margin  by  embedding blanks.  The old text is saved in
                    the adjust buffer.  As  of  e19,  fill/just  no  longer
                    filters  out  CTRL  characters,  and  no  longer splits
                    hyphenated words at the end of  a  line.   The  command
                    "CMD: set hy" will reinstate splitting.

               close [<area>]
               -close
                    Close deletes the text from the file and  remembers  it
                    in  a "close buffer", -close puts the close buffer back
                    into the file at the current cursor position.

               command
               -command
                    Usually you issue commands one at a time by typing  the
                    <CMD>  key,  the  command, and then <RETURN>. The "com-
                    mand" command places you in a mode where you  can  keep
                    typing  commands  without  hitting  the  <CMD> key each
                    time.   To  return  to  normal  edit  mode,  issue  the
                    -command command.

               delete
                    Tells the editor to delete the current file on exit.



          Sun Release 1.1         Last change: Rand                       7






          E(1)                      USER COMMANDS                      E(1)



               dword
               -dword
                    Delete word.  If the cursor is "on" a word,  that  word
                    is  deleted,  othwise  the  next  word is deleted.  The
                    deleted word can be restored by -dword.

               e | edit  [<filename>]
                    If no filename is specified,  the  alternate  file  (if
                    any)  is  brought  up in the current window.  Otherwise
                    the specified file is displayed.   If  _f_i_l_e_n_a_m_e  begins
                    with  "~"  or  "~/",  the appropriate home directory is
                    prepended.

               erase  [<area>]
               -erase
                    Erase replaces the area by blanks and puts the contents
                    into  an "erase" buffer; -erase inserts the contents of
                    this buffer at the current cursor position.

               feed [<range>] command-string
                    A Unix shell is started with  the  command-string,  and
                    the  text  in  <range>  is supplied to the shell as the
                    standard  input.   Any  results  of  the  command   are
                    inserted before the first line in the range.

               goto [ b | line-number | e | rb | re ]
                    Goto is a quick way of moving around a file.  Goto  'b'
                    moves  to  the  beginning of the file, 'e' moves to the
                    end.  A line-number goes to that line of the file; 'rb'
                    and 're' move to the beginning or end of a marked range
                    (see the range command).  A  short-cut  to  goto  line-
                    number _n is "CMD: _n<RETURN>".

               join | -split
               split | -join
                    Join (or -spilt) combines two lines, split  (or  -join)
                    breaks a line in two.

               name <newfilename>
                    Tells  the  editor  to  rename  the  current  file   to
                    newfilename when it exits.

               open [<area>]
                    Move existing text to make room  for  <area>  worth  of
                    blank lines (or a rectangle of blanks).

               pick [<area>]
               -pick
                    Pick copies the <area> to a pick buffer, -pick  inserts
                    the pick buffer at the current cursor position.





          Sun Release 1.1         Last change: Rand                       8






          E(1)                      USER COMMANDS                      E(1)



               range [<range>]
               -range
               ?range
                    The range command is another  method  of  limiting  the
                    range of some commands (notably the replace, fill, jus-
                    tify and center commands).  -range turns it off, ?range
                    tells you the line numbers of the range area.

               redraw | red
                    The redraw command is used to erase and redraw the edi-
                    tor  windows  in case something has happened to it (for
                    example, line noise on dialup lines, or  messages  from
                    the operator).

               replace  [<range>] [<option>] /search-string/replace-string/
               -replace [<range>] [<option>] /search-string/replace-string/
                    The replace command searches  forward  over  the  range
                    replacing  the  search-string  with the replace-string;
                    -replace searches backwards doing the same thing.   The
                    '/'  delimiter  can be replaced by other symbols if the
                    search or replace strings contain a  slash.   Any  non-
                    control,  non-alphanumeric character can be used as the
                    delimiter.

                    Two options can also be specified, they  are  _s_h_o_w  and
                    _i_n_t_e_r_a_c_t_i_v_e. The _s_h_o_w option allows the user to see the
                    replacements as they  occur.   The  _i_n_t_e_r_a_c_t_i_v_e  option
                    additionally  allows  the  user control over whether or
                    not to make each replacement by displaying the  search-
                    string  and  allowing the user to hit the <REPL> key to
                    do replacement, or the <+_SRCH> key to skip the replace-
                    ment.

               regexp
                    -regexp
                    The "regexp" command  enters  regular  expression  mode
                    (for  searching and replacing); "-regexp" exits regular
                    expression mode.  While  in  regular  expression  mode,
                    "RE" is displayed at the bottom of the screen.

               run [<range>] command-string
                    The run command is similar to the feed command,  except
                    that  the  marked  text  is deleted and replaced by the
                    results of running the command-string.

               save <newfilename>
                    Immediately write a copy of the file to newfilename.

               set _o_p_t_i_o_n
               ?set
                    used to set various options:
9                    +line _n        sets the <+line> key to _n lines



          Sun Release 1.1         Last change: Rand                       9


9


          E(1)                      USER COMMANDS                      E(1)



                    -line _n        sets the <-line> key to _n lines
                    line _n         sets the <+line> and <-line> keys to _n lines
                    +page _n        sets the <+page> key to _n screens
                    -page _n        sets the <-page> key to _n screens
                    page _n         sets the <-page> and <+page> keys to _n screens
                    ?              displays the values of several options (same as the "?set" command)
                    bell           enable the bell
                    nobell         disable the bell
                    hy             enables splitting of hyphenated words by fill/just
                    nohy           disables splitting of hyphenated words by fill/just (default)
                    left _n         sets the <window left> key to _n cols
                    lmar _n         sets the left margin to _n
                    right _n        sets the <window right> key to _n cols
                    rmar _n         sets the right margin (linewidth) to _n
                    window _n       sets the <window left> and <window right> keys to _n cols
                    width _n        sets the linewidth to _n cols (for fill, etc.)
                    wordelim _m_o_d_e  sets the action of the <+word> and <-word> keys.
                                   If mode  is  _w_h_i_t_e_s_p_a_c_e  (the  default),
                                   words  are  delimited by blanks and new-
                                   lines, and the cursor  advances  to  the
                                   first character following the delimiter.
                                   If mode is _a_l_p_h_a_n_u_m, words are delimited
                                   by all special characters in addition to
                                   blanks  and  newlines,  and  the  cursor
                                   advances to the first alphanumeric char-
                                   acter following the delimiter.
9                    The "line", "page", and "window" options may be indivi-
                    dually set in different windows.

               stop
                    Use 'job control' to suspend the editor.  Returns  con-
                    trol to the shell, resume with the 'fg' command.  Works
                    only under Berkeley versions 4.xBSD.

               tab  [ column ... ]
               -tab [ column ... ]
                    Tab sets tabs in the specified  columns;  -tab  removes
                    tabs in specified columns.

               tabs n
               -tabs
                    Tabs sets tabs every nth column, and -tabs removes  all
                    tabs.

               tabfile tabfilename
               -tabfile tabfilename
                    Tabfile sets  tabs  (-tabfile  clears  tabs)  at  every
                    column listed in the tabfilename.

               track
               -track
                    The track command  is  used  to  scroll  the  main  and



          Sun Release 1.1         Last change: Rand                      10


9


          E(1)                      USER COMMANDS                      E(1)



                    alternate  files  together.   This  is normally used to
                    visually compare the main file and the  alternate  file
                    by  rapidly  changing  between  them  and watching what
                    changes on the screen (similar to blink-comparators  as
                    used  in  astronomy).  The -track command turns off the
                    tracking mode.  As of e19, the TRACK flag has been made
                    part  of  the  state  of each window, so that the TRACK
                    mode can be reestablished  following  an  interruption,
                    and  the TRACK command has been changed so that it tog-
                    gles the TRACK mode, in the same way as does INSERT.

               update | -update [ inplace | -inplace | -readonly]
                    The update command tells the editor what to do on  exit
                    with  the current file.  The update command can be used
                    to specify whether or not to break links,  the  -update
                    command  causes  any  changes to the current file to be
                    ignored.  The -_r_e_a_d_o_n_l_y option is used to enable modif-
                    ications to files which (1) you own but lack write per-
                    mission on, or, (2) to override a  "-readonly"  invoca-
                    tion argument.

               w | window [filename]
               -w | -window
                    The window command will create  a  new  window  at  the
                    current cursor position (as long as the cursor is along
                    the top or left margins).  If a filename is  specified,
                    that file is displayed in the new window, otherwise the
                    current file is used.  The -window command deletes  the
                    current window.

               wp
               -wp
                    The _w_p command enters word procesing mode,  -_w_p  exits.
                    A  WP  is displayed at the bottom of the screen in word
                    processing mode.  WP mode enables power typing or "word
                    wrap",  where  any  text that is entered past the right
                    margin (see "set rmar" above) is  automatically  placed
                    on  the  following  line  at  the left margin (see "set
                    lmar" above).  WP mode also moves  the  cursor  to  the
                    left  margin  in response to a <RETURN>.  If WP mode is
                    entered while a marked area is  in  effect,  the  boun-
                    daries  of  the  marked  area become the left and right
                    margin settings.

          FUNCTION KEYS
               E supports a set of _f_u_n_c_t_i_o_n_s that are normally executed  by
               hitting  one  of the terminal functions keys, or by typing a
               specific control- or escape sequence.  Details  of  what  _E-
               _f_u_n_c_t_i_o_n_s are emitted for a specific terminal's function key
               layout are hard to document generally, since each site  usu-
               ally tailor's this to their liking.  Your system administra-
               tor should be able to provide  a  mapping.   Also,  see  the



          Sun Release 1.1         Last change: Rand                      11






          E(1)                      USER COMMANDS                      E(1)



               following    sections    _S_t_a_n_d_a_r_d   _E_d_i_t_i_o_r   _K_e_y_b_o_a_r_d   and
               _T_e_r_m_i_n_a_l_s/_K_e_y_b_o_a_r_d_s _S_u_p_p_o_r_t_e_d.  Note that  some  _E-_f_u_n_c_t_i_o_n_s
               require  first  pressing  the  <CMD> key followed by another
               <KEY>.  Refer to the previous section "COMMANDS" for a  dis-
               cussion of the following functions: <CLOSE>, <OPEN>, <PICK>,
               <ERASE>, and <MARK> which are normally implemented on  func-
               tion  keys.   The  sequences  <CMD><CLOSE>,  <CMD><PICK> and
               <CMD><ERASE> are documented  above  as  -_c_l_o_s_e,  -_p_i_c_k,  and
               -_e_r_a_s_e.

               <<-> | <|^> | <> | <->>
                      The arrow keys move one space (or line) in the  indi-
                      cated  direction.  An <|^> on the top line of the win-
                      dow (or <> on the bottom  line)  scrolls  the  window
                      down  (or up) one line.  A <<-> at the left edge of a
                      window does nothing.  A <->> at the right edge  of  a
                      window  does  nothing if option -_s_t_i_c_k is set, other-
                      wise the  window  is  shifted  right  (16  spaces  by
                      default).

               <+LINE>, <+LINE>
                      scrolls window down/up about 1/4 the window size.

               <CMD><+LINE>, <CMD><-LINE>
                      makes the current cursor line  the  top  (or  bottom)
                      line of the window.

               <+PAGE>, <-PAGE>
                      scrolls window down/up a full "page".

               <CMD><+PAGE>, <CMD><-PAGE>
                      moves to the end/beginning of the file.

               <+SRCH>, <-SRCH>
                      searches in the  indicated  direction  for  the  next
                      occurrence of the "search string".  The _s_e_a_r_c_h _s_t_r_i_n_g
                      is set by <CMD>search string<+_SRCH>.

               <CMD><+SRCH>, <CMD><-SRCH>
                      sets the "search string" to the current cursor  word,
                      and performs a search in the indicated direction.

               <+TAB>, <-TAB>
                      tabs in indicated direction.

               <+WORD>, <-WORD>
                      moves to next/previous word.

               <ALT>  switch to alternate file, also "<CMD>e<RETURN>".

               <BS>   backspace, moves left and erases previous  character.
                      <CMD><BS> deletes text from the beginning of the line



          Sun Release 1.1         Last change: Rand                      12






          E(1)                      USER COMMANDS                      E(1)



                      up to the current cursor character.  In _i_n_s_e_r_t  _m_o_d_e,
                      the rest of the line is moved left.

               <CHGWIN>
                      switch to next window.

               <CTRLCHAR>
                      used to enter  non-ascii  characters  (\000-\037)  in
                      text.

               <DELCHAR>
                      deletes cursor character.

               <CMD><DELCHAR>
                      deletes text from the cursor to the end of the line.

               <HOME> moves to upper left corner of window.

               <INSERT>
                      toggles _i_n_s_e_r_t mode.

               <INT>  Interrupts the operation of various functions:  e.g.,
                      searches.

               <MARK> used to define a range of lines or  rectangular  area
                      of  text.  A simple <MARK> will start by defining one
                      _l_i_n_e.  Moving the  cursor  up/down  will  extend  the
                      defined  range  of  _l_i_n_e_s  (as  will  other keys like
                      <+LINE> <+PAGE>).  When full lines  are  _m_a_r_k_e_d,  the
                      message  "MARK  _n"  is displayed at the bottom of the
                      screen.  Rectangular areas  of  text  are  marked  by
                      first marking one or more lines, then moving the cur-
                      sor left  or  right.   When  a  rectangular  area  is
                      marked, the message "MARK _nx_m" is displayed.

               <CMD><MARK>
                      cancels any mark in affect.

               <REPL> Executes the "replacement" while doing an interactive
                      replace command.

               <RETURN>
                      Positions cursor at column 1 on  the  next  line.   A
                      <RETURN> on the bottom line of a window is equivalent
                      to a <+LINE>.  In WP mode, a <RETURN>  positions  the
                      cursor  at  the current _l_e_f_t _m_a_r_g_i_n on the next line,
                      which may automatically shift the window left.

               <WINLEFT>, <WINRIGHT>
                      Shifts the window left/right (16 spaces by default).

               <CMD><WINLEFT>



          Sun Release 1.1         Last change: Rand                      13






          E(1)                      USER COMMANDS                      E(1)



                      Shifts the window all the way back to column 1.

               <CMD><WINRIGHT>
                      Shifts the  window  as  far  right  as  necessary  to
                      display the end of the line.


















































          Sun Release 1.1         Last change: Rand                      14






          E(1)                      USER COMMANDS                      E(1)



          STANDARD EDITOR KEYBOARD
               There is now a "standard" E keyboard, that is designed to be
               usable  on  all  video  display  terminals.   Either  say "e
               -keyboard=standard" or "setenv EKBD standard" before running
               E to select this keyboard.  This is also the keyboard that E
               will use if there is no specific knowledge of your  type  of
               terminal compiled into the editor.

               This keyboard layout is designed to  be  used  on  terminals
               with  no  function  keys.  "^H" means control-H, and "^X-^U"
               means control-X followed by control-U.  The ALT entry  gives
               you  a  choice  of  "^/" or "^_" because one or the other or
               both will not  work  on  some  terminals.  ("_"  is  correct
               ASCII.)


                                    E STANDARD KEYBOARD
98          _______________________________________________________________________
           (extend)       ^X                      DOWN           ^J
           +LINE          ^F                      ERASE          ^X-^E or ^^
           +PAGE          ^R                      HOME           ^G
           +SRCH          ^Y                      INSERT         ^Z or ESC or ^[
           +TAB           ^I                      INT            ^\
           +WORD          ^N                      JOIN           ^X-^J
           -LINE          ^D                      LEFT           ^H
           -PAGE          ^E                      MARK           ^U
           -SRCH          ^T                      OPEN           ^O
           -TAB           ^X-^U                   PICK           ^P
           -WORD          ^B                      REPL           ^X-^R or ^]
           ALT            ^X-^A or ^_ or ^/       RETURN         ^M
           BSP            ^C                      RIGHT          ^L
           CHG WIN        ^X-^W                   SPLIT          ^X-^B
           CLOSE          ^V                      TABS           ^X-^T
           CMD            ^A or NULL or ^@        UP             ^K
           CTRLCHAR       ^X-^C                   WIN LEFT       ^X-^H
           DELCHAR        ^W                      WIN RIGHT      ^X-^L
8          _______________________________________________________________________
7          |8|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|
















9                        |8|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|
















9                                                |8|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|99999999999999999999999999999999999|8|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|
















9                                                               |8|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|
















9                                                                                 |8|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|7|


















98          ___________________________________________________________________________________
           ^@    CMD        ^L       RIGHT          ^X       (extend)     ^X-^H    WIN LEFT
           ^A    CMD        ^M       RETURN         ^Y       +SRCH        ^X-^J    JOIN
           ^B    -WORD      ^N       +WORD          ^Z       INSMODE      ^X-^L    WIN RIGHT
           ^C    BSP        ^O       OPEN           ^[       INSERT       ^X-^N    DEL WORD
           ^D    -LINE      ^P       PICK           ^\       INT          ^X-^R    REPL
           ^E    -PAGE      ^Q       (not used)     ^]       REPL         ^X-^B    SPLIT
           ^F    +LINE      ^R       +PAGE          ^^       ERASE        ^X-^T    TABS
           ^G    HOME       ^S       (not used)     ^_       ALT          ^X-^U    -TAB
           ^H    LEFT       ^T       -SRCH          ^/       ALT          ^X-^W    CHG WIN
           ^I    +TAB       ^U       MARK           ^X-^A    ALT
           ^J    DOWN       ^V       CLOSE          ^X-^C    CTRLCHAR
           ^K    UP         ^W       DELCHAR        ^X-^E    ERASE
8          ___________________________________________________________________________________
7          |8|7|7|7|7|7|7|7|7|7|7|7|










9               |8|7|7|7|7|7|7|7|7|7|7|7|










9                          |8|7|7|7|7|7|7|7|7|7|7|7|99999999999999999999999|8|7|7|7|7|7|7|7|7|7|7|7|










9                                   |8|7|7|7|7|7|7|7|7|7|7|7|










9                                                 |8|7|7|7|7|7|7|7|7|7|7|7|99999999999999999999999|8|7|7|7|7|7|7|7|7|7|7|7|










9                                                           |8|7|7|7|7|7|7|7|7|7|7|7|










9                                                                        |8|7|7|7|7|7|7|7|7|7|7|7|99999999999999999999999|8|7|7|7|7|7|7|7|7|7|7|7|










9                                                                                 |8|7|7|7|7|7|7|7|7|7|7|7|










9                                                                                             |8|7|7|7|7|7|7|7|7|7|7|7|
















          Sun Release 1.1         Last change: Rand                      15






          E(1)                      USER COMMANDS                      E(1)



          TERMINALS/KEYBOARDS SUPPORTED
               This is installation specific information  with  the  actual
               terminal  types  defined  varying across the host cpu's ver-
               sions of the editor.  Now that the editor can be driven from
               termcap  and  the standard keyboard, many sites will wish to
               delete specific terminal type support to save memory.   Code
               is  provided  with  E for the following terminals and may be
               compiled into your version.


                       DESCRIPTION                      TERMINAL TYPE

               Ann Arbor S001901/2             aa, aa0, annarbor, default
               New ann arbor (Model Q2878)     aa1
               Ann Arbor Ambassador            aaa, ambas, ambassador, aaa-N
                and Ann Arbor XLs              (N is one of 18,20,22,24,26,28,
                                                30,36,40,48 or 60 )
               Beehive MicroBee                microb
               Termcap terminal                [from /etc/termcap]
               INTERACTIVE Systems Intext2     intext2, T_2intext
               Lear Siegler ADM3a              3a, adm3a, la
               Liberty Freedom 100             fr10, free100
               Heathkit H19 & H89              h19, k1
               INTERACTIVE Systems Intext      in, intext
               Perkin Elmer 1251               po
               Standard keyboard               standard
               DEC VT100  (needs termcap)      vt100, vt100w (wide)
               Lear Siegler ADM31              31, adm31, l1
               Lear Siegler ADM42              adm42, l4
               Concept 100                     c100, co, concept, concept100
               Datamedia DM4000                dm4000
               Wyse 50                         wyse50, wyse50w (wide)

          STARTUP FILE
               As a means of  individually  tailoring  various  options  on
               startup,  E will run commands specified in a "profile" file.
               The order in which E decides which "profile"  file  to  run,
               is:  command line option -profile=_f_i_l_e, environment variable
               EPROFILE, a "safe" ./._e__p_r_o_f_i_l_e, and if none of these exist,
               ~/._e__p_r_o_f_i_l_e.   ("./.e_profile"  is deemed to be safe if you
               are not the super-user, you own the file, and it's not writ-
               able  by  everyone  else.)  If the first line of the profile
               file begins with the keyword "options:", then  the  rest  of
               the  line  is treated as if typed as initial arguments to E.
               The options must currently all be on one line.

               Profile format notes:
               o+  blank lines and lines beginning with # are ignored
               o+  E function keys are denoted by "<keyword>";  use "\<"  to
                  insert  a "<" in text; "<#keyword>" may be used to repeat
                  _k_e_y_w_o_r_d #-times
               o+  keywords:



          Sun Release 1.1         Last change: Rand                      16






          E(1)                      USER COMMANDS                      E(1)



                       <+line>   <bksp>    <home>     <right>*
                       <+page>   <cchar>   <insmd>    <split>
                       <+sch>    <chwin>   <int>      <srtab>
                       <+tab>    <close>   <join>     <tab>
                       <+word>   <cmd>     <left>*    <up>*
                       <-line>   <dchar>   <mark>     <wleft>
                       <-page>   <down>*   <open>     <wright>
                       <-sch>    <dword>   <pick>
                       <-tab>    <edit>    <replace>
                       <-word>   <erase>   <ret>
                       (*=cursor motion)
               o+  essentially anything that  can  be  typed  at  E  can  be
                  inserted in the ".e_profile".  Thus, application specific
                  editor scripts can be created.
               o+  note that various CMD options end with  a  <ret>,  others
                  don't; e.g., "<cmd>tabs 4<ret>", but "<cmd><+page>".
               o+  E looks for a "profile" file and examines the  "options:"
                  line  _b_e_f_o_r_e  processing any command line arguments, thus
                  command line options override any  options  specified  in
                  the  "options:" line.  _C_a_v_e_a_t: if the -profile=file argu-
                  ment is used, any "options:" in a profile file will  have
                  already  been  processed.   This  is  a  "cart before the
                  horse" problem....
               o+  any format errors will terminate reading of  the  profile
                  file, and continue the E session;
               o+  example:
                    options: -regex -stick
                    <cmd>tabs 4<ret>
                    <cmd>set window 40<ret>

          BUGS
               By far the worst problem with E is its treatment of tabs.  E
               is  a  "whitespace"  editor,  all  whitespace  is considered
               equal.  On any line that E modifies, it will compress multi-
               ple  initial blanks to tabs, strip trailing blanks, and con-
               vert any embedded tabs to blanks.  This  is  acceptable  for
               most  editing,  but some programs expect real tab characters
               as input, for example tbl(1) and some nroff commands.



          SEE ALSO
               _S_E_L_F-_T_E_A_C_H_I_N_G _G_U_I_D_E _T_O _R_A_N_D'_S _T_E_X_T _P_R_O_C_E_S_S_O_R, October  1983,
               Computer Services Department, The Rand Corporation

               _T_H_E _R_A_N_D _E_D_I_T_O_R, _E, April 1984,  Computer  Services  Depart-
               ment, The Rand Corporation.



          AUTHOR
               David Yost and a cast of thousands.



          Sun Release 1.1         Last change: Rand                      17






          E(1)                      USER COMMANDS                      E(1)



               Lyle McElhaney contributed most of the e19 changes.
               Matt Bishop contributed most of the e18 changes.


          SPONSOR
               The Rand Corporation.

















































          Sun Release 1.1         Last change: Rand                      18



