/* useful for debugging */

#define CCHIGHEST      0237

/* because we're inialized here,
 * don't #include in multiple files
 *
 */
typedef struct KeyNameTable {
  int val;
  char *name;
}  KeyNameTable;

KeyNameTable E_Keys[] = {
{0236,          "CCMOUSE"},
{000,           "CCCMD"},
{001,           "CCLWINDOW"},
{002,           "CCSETFILE"},
{003,           "CCINT"},
{004,           "CCOPEN"},
{005,           "CCMISRCH"},
{006,           "CCCLOSE"},
{007,           "CCMARK"},
{010,           "CCMOVELEFT"},
{011,           "CCTAB"},
{012,           "CCMOVEDOWN"},
{013,           "CCHOME"},
{014,           "CCPICK"},
{015,           "CCRETURN"},
{016,           "CCMOVEUP"},
{017,           "CCINSMODE"},
{020,           "CCREPLACE"},
{021,           "CCMIPAGE"},
{022,           "CCPLSRCH"},
{023,           "CCRWINDOW"},
{024,           "CCPLLINE"},
{025,           "CCDELCH"},
{026,           "CCRWORD"},
{027,           "CCMILINE"},
{030,           "CCLWORD"},
{031,           "CCPLPAGE"},
{032,           "CCCHWINDOW"},
{033,           "CCTABS"},
{034,           "CCCTRLQUOTE"},
{035,           "CCBACKTAB"},
{036,           "CCBACKSPACE"},
{037,           "CCMOVERIGHT"},
{0177,           "CCDEL"},
{0200,           "CCSTOP"},
{0201,           "CCERASE"},
{0202,           "CCUNAS1"},
{0203,           "CCSPLIT"},
{0204,           "CCJOIN"},
{0205,           "CCEXIT"},
{0206,           "CCABORT"},
{0207,           "CCREDRAW"},
{0210,           "CCCLRTABS"},
{0211,           "CCCENTER"},
{0212,           "CCFILL"},
{0213,           "CCJUSTIFY"},
{0214,           "CCCLEAR"},
{0215,           "CCTRACK"},
{0216,           "CCBOX"},
{0217,           "CCSTOPX"},
{0220,           "CCQUIT"},
{0221,           "CCCOVER"},
{0222,           "CCOVERLAY"},
{0223,           "CCBLOT"},
{0224,           "CCHELP"},
{0225,           "CCCCASE"},
{0226,           "CCCAPS"},
{0227,           "CCAUTOFILL"},
{0230,           "CCRANGE"},
{0231,           "CCNULL"},
{0232,           "CCDWORD"},
{0233,           "CCUNAS1"},
{0234,           "CCRECORD"},
{0235,           "CCPLAY"},
{0237,           "CCMOUSEONOFF"},
{0400,           "NOCHAR"},
{-1,             ""}
};

KeyNameTable Curses_Keys[] = {
{0631,    "KEY_MOUSE"},
{0400,    "KEY_CODE_YES"},
{0401,    "KEY_MIN"},
{0401,    "KEY_BREAK"},
{0402,    "KEY_DOWN"},
{0403,    "KEY_UP"},
{0404,    "KEY_LEFT"},
{0405,    "KEY_RIGHT"},
{0406,    "KEY_HOME"},
{0407,    "KEY_BACKSPACE"},
{0410,    "KEY_F0"},
{0510,    "KEY_DL"},
{0511,    "KEY_IL"},
{0512,    "KEY_DC"},
{0513,    "KEY_IC"},
{0514,    "KEY_EIC"},
{0515,    "KEY_CLEAR"},
{0516,    "KEY_EOS"},
{0517,    "KEY_EOL"},
{0520,    "KEY_SF"},
{0521,    "KEY_SR"},
{0522,    "KEY_NPAGE"},
{0523,    "KEY_PPAGE"},
{0524,    "KEY_STAB"},
{0525,    "KEY_CTAB"},
{0526,    "KEY_CATAB"},
{0527,    "KEY_ENTER"},
{0530,    "KEY_SRESET"},
{0531,    "KEY_RESET"},
{0532,    "KEY_PRINT"},
{0533,    "KEY_LL"},
{0534,    "KEY_A1"},
{0535,    "KEY_A3"},
{0536,    "KEY_B2"},
{0537,    "KEY_C1"},
{0540,    "KEY_C3"},
{0541,    "KEY_BTAB"},
{0542,    "KEY_BEG"},
{0543,    "KEY_CANCEL"},
{0544,    "KEY_CLOSE"},
{0545,    "KEY_COMMAND"},
{0546,    "KEY_COPY"},
{0547,    "KEY_CREATE"},
{0550,    "KEY_END"},
{0551,    "KEY_EXIT"},
{0552,    "KEY_FIND"},
{0553,    "KEY_HELP"},
{0554,    "KEY_MARK"},
{0555,    "KEY_MESSAGE"},
{0556,    "KEY_MOVE"},
{0557,    "KEY_NEXT"},
{0560,    "KEY_OPEN"},
{0561,    "KEY_OPTIONS"},
{0562,    "KEY_PREVIOUS"},
{0563,    "KEY_REDO"},
{0564,    "KEY_REFERENCE"},
{0565,    "KEY_REFRESH"},
{0566,    "KEY_REPLACE"},
{0567,    "KEY_RESTART"},
{0570,    "KEY_RESUME"},
{0571,    "KEY_SAVE"},
{0572,    "KEY_SBEG"},
{0573,    "KEY_SCANCEL"},
{0574,    "KEY_SCOMMAND"},
{0575,    "KEY_SCOPY"},
{0576,    "KEY_SCREATE"},
{0577,    "KEY_SDC"},
{0600,    "KEY_SDL"},
{0601,    "KEY_SELECT"},
{0602,    "KEY_SEND"},
{0603,    "KEY_SEOL"},
{0604,    "KEY_SEXIT"},
{0605,    "KEY_SFIND"},
{0606,    "KEY_SHELP"},
{0607,    "KEY_SHOME"},
{0610,    "KEY_SIC"},
{0611,    "KEY_SLEFT"},
{0612,    "KEY_SMESSAGE"},
{0613,    "KEY_SMOVE"},
{0614,    "KEY_SNEXT"},
{0615,    "KEY_SOPTIONS"},
{0616,    "KEY_SPREVIOUS"},
{0617,    "KEY_SPRINT"},
{0620,    "KEY_SREDO"},
{0621,    "KEY_SREPLACE"},
{0622,    "KEY_SRIGHT"},
{0623,    "KEY_SRSUME"},
{0624,    "KEY_SSAVE"},
{0625,    "KEY_SSUSPEND"},
{0626,    "KEY_SUNDO"},
{0627,    "KEY_SUSPEND"},
{0630,    "KEY_UNDO"},
{0632,    "KEY_RESIZE"},
{0633,    "KEY_EVENT"},
{-1,      ""}
};



