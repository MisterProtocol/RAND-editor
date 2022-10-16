/* useful for debugging */

#define CCHIGHEST      0250

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
{0240,           "CCBRACE"},
{0241,           "CCPUT"},
{0242,           "CCUNMARK"},
{0243,           "CCREGEX"},
{0244,           "CCPLWIN"},
{0245,           "CCMIWIN"},
{0246,           "CCMICLOSE"},
{0247,           "CCMIERASE"},
{0250,           "CCMIRECORD"},
{0400,           "NOCHAR"},
{-1,             ""}
};

KeyNameTable Curses_Keys[] = {
{0631,    "KEY_MOUSE"},
{0400,    "KEY_CODE_YES"},
{0401,    "KEY_MIN"},
{0401,    "KEY_BREAK"},     /* idx=0 in Key2Ecode[] */
{0402,    "KEY_DOWN"},
{0403,    "KEY_UP"},
{0404,    "KEY_LEFT"},
{0405,    "KEY_RIGHT"},
{0406,    "KEY_HOME"},
{0407,    "KEY_BACKSPACE"},
{0410,    "KEY_F0"},
{0411,    "KEY_F1"},        /* idx=8 in Key2Ecode[]*/
{0412,    "KEY_F2"},
{0413,    "KEY_F3"},
{0414,    "KEY_F4"},
{0415,    "KEY_F5"},
{0416,    "KEY_F6"},
{0417,    "KEY_F7"},
{0420,    "KEY_F8"},
{0421,    "KEY_F9"},
{0422,    "KEY_F10"},
{0423,    "KEY_F11"},
{0424,    "KEY_F12"},
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
/* {0633,    "KEY_EVENT"}, */
{-1,      ""}
};


typedef struct NcursesKeyCode {
  char *name;
  int val;
}  NcursesKeyCode;


NcursesKeyCode CursesKeyCodes[] = {
{"KEY_MOUSE",          0631},
{"KEY_CODE_YES",       0400},
{"KEY_MIN",            0401},
{"KEY_BREAK",          0401},
{"KEY_DOWN",           0402},
{"KEY_UP",             0403},
{"KEY_LEFT",           0404},
{"KEY_RIGHT",          0405},
{"KEY_HOME",           0406},
{"KEY_BACKSPACE",      0407},
{"KEY_F0",             0410},
{"KEY_F1",             0411},
{"KEY_F2",             0412},
{"KEY_F3",             0413},
{"KEY_F4",             0414},
{"KEY_F5",             0415},
{"KEY_F6",             0416},
{"KEY_F7",             0417},
{"KEY_F8",             0420},
{"KEY_F9",             0421},
{"KEY_F10",            0422},
{"KEY_F11",            0423},
{"KEY_F12",            0424},

{"SHIFT_F1",           0425},
{"SHIFT_F2",           0426},
{"SHIFT_F3",           0427},
{"SHIFT_F4",           0430},
{"SHIFT_F5",           0431},
{"SHIFT_F6",           0432},
{"SHIFT_F7",           0433},
{"SHIFT_F8",           0434},
{"SHIFT_F9",           0435},
{"SHIFT_F10",          0436},
{"SHIFT_F11",          0437},
{"SHIFT_F12",          0440},

{"CTRL_F1",           0441},
{"CTRL_F2",           0442},
{"CTRL_F3",           0443},
{"CTRL_F4",           0444},
{"CTRL_F5",           0445},
{"CTRL_F6",           0446},
{"CTRL_F7",           0447},
{"CTRL_F8",           0450},
{"CTRL_F9",           0451},
{"CTRL_F10",          0452},
{"CTRL_F11",          0453},
{"CTRL_F12",          0454},

{"KEY_DL",             0510},
{"KEY_IL",             0511},
{"KEY_DC",             0512},
{"KEY_IC",             0513},
{"KEY_EIC",            0514},
{"KEY_CLEAR",          0515},
{"KEY_EOS",            0516},
{"KEY_EOL",            0517},
{"KEY_SF",             0520},
{"KEY_SR",             0521},
{"KEY_NPAGE",          0522},
{"KEY_PPAGE",          0523},
{"KEY_STAB",           0524},
{"KEY_CTAB",           0525},
{"KEY_CATAB",          0526},
{"KEY_ENTER",          0527},
{"KEY_SRESET",         0530},
{"KEY_RESET",          0531},
{"KEY_PRINT",          0532},
{"KEY_LL",             0533},
{"KEY_A1",             0534},
{"KEY_A3",             0535},
{"KEY_B2",             0536},
{"KEY_C1",             0537},
{"KEY_C3",             0540},
{"KEY_BTAB",           0541},
{"KEY_BEG",            0542},
{"KEY_CANCEL",         0543},
{"KEY_CLOSE",          0544},
{"KEY_COMMAND",        0545},
{"KEY_COPY",           0546},
{"KEY_CREATE",         0547},
{"KEY_END",            0550},
{"KEY_EXIT",           0551},
{"KEY_FIND",           0552},
{"KEY_HELP",           0553},
{"KEY_MARK",           0554},
{"KEY_MESSAGE",        0555},
{"KEY_MOVE",           0556},
{"KEY_NEXT",           0557},
{"KEY_OPEN",           0560},
{"KEY_OPTIONS",        0561},
{"KEY_PREVIOUS",       0562},
{"KEY_REDO",           0563},
{"KEY_REFERENCE",      0564},
{"KEY_REFRESH",        0565},
{"KEY_REPLACE",        0566},
{"KEY_RESTART",        0567},
{"KEY_RESUME",         0570},
{"KEY_SAVE",           0571},
{"KEY_SBEG",           0572},
{"KEY_SCANCEL",        0573},
{"KEY_SCOMMAND",       0574},
{"KEY_SCOPY",          0575},
{"KEY_SCREATE",        0576},
{"KEY_SDC",            0577},
{"KEY_SDL",            0600},
{"KEY_SELECT",         0601},
{"KEY_SEND",           0602},
{"KEY_SEOL",           0603},
{"KEY_SEXIT",          0604},
{"KEY_SFIND",          0605},
{"KEY_SHELP",          0606},
{"KEY_SHOME",          0607},
{"KEY_SIC",            0610},
{"KEY_SLEFT",          0611},
{"KEY_SMESSAGE",       0612},
{"KEY_SMOVE",          0613},
{"KEY_SNEXT",          0614},
{"KEY_SOPTIONS",       0615},
{"KEY_SPREVIOUS",      0616},
{"KEY_SPRINT",         0617},
{"KEY_SREDO",          0620},
{"KEY_SREPLACE",       0621},
{"KEY_SRIGHT",         0622},
{"KEY_SRSUME",         0623},
{"KEY_SSAVE",          0624},
{"KEY_SSUSPEND",       0625},
{"KEY_SUNDO",          0626},
{"KEY_SUSPEND",        0627},
{"KEY_UNDO",           0630},
{"KEY_RESIZE",         0632},
/* nxt is no longer defined in version 6.2 */
/* {"KEY_EVENT",          0633}, */
{NULL,  -1}
};

#define MAX_KEY 0633

typedef struct sKey2Ecode {
 int keycode;       /* ncurses KEY_ value */
 Uchar  ecmd1;      /* 1st E cmd, eg:  KEY_F6:<+page> */
 Uchar *ecmdstr;    /* 2nd -> end of e cmds, eg:  KEY_F6:<+word><redraw><-page> */
 int len;           /* num Uchars in ecmdstr */
 char keyname[16];  /* tmp for debugging */
} sKey2Ecode;

/*
 * struct KeyCtrl[] is populated with the standard keyboard mapping
 * for each ctrl-{ch}ar.  If a user's keyboard file changes any ^ch
 * we record them here in two methods:
 *
 *  1. if ^ch maps to a single E command, simply replace
 *     the KeyCtrl[^ch].ecmd1 value.
 *  2. if ^maps to more than 1, set the 1st E command (or text char)
 *     to KeyCtrl[^ch].ecmd1, and populate KeyCtrl[^ch].ecmdstr
 *     with the remaining cmds/chars; and update len
 */

sKey2Ecode KeyCtrl[32] = {
  /* see term/standard.c */
{   0,  (Uchar)CCCMD       , (Uchar *) NULL, 1, {} },  /* <BREAK > */
{   1,  (Uchar)CCCMD       , (Uchar *) NULL, 1, {} },  /* <cntr A> */
{   2,  (Uchar)CCLWORD     , (Uchar *) NULL, 1, {} },  /* <cntr B> */
{   3,  (Uchar)CCBACKSPACE , (Uchar *) NULL, 1, {} },  /* <cntr C> */
{   4,  (Uchar)CCMILINE    , (Uchar *) NULL, 1, {} },  /* <cntr D> */
{   5,  (Uchar)CCMIPAGE    , (Uchar *) NULL, 1, {} },  /* <cntr E> */
{   6,  (Uchar)CCPLLINE    , (Uchar *) NULL, 1, {} },  /* <cntr F> */
{   7,  (Uchar)CCHOME      , (Uchar *) NULL, 1, {} },  /* <cntr G> */
{   8,  (Uchar)CCMOVELEFT  , (Uchar *) NULL, 1, {} },  /* <cntr H> */
{   9,  (Uchar)CCTAB       , (Uchar *) NULL, 1, {} },  /* <cntr I> */
{  10,  (Uchar)CCMOVEDOWN  , (Uchar *) NULL, 1, {} },  /* <cntr J> */
{  11,  (Uchar)CCMOVEUP    , (Uchar *) NULL, 1, {} },  /* <cntr K> */
{  12,  (Uchar)CCMOVERIGHT , (Uchar *) NULL, 1, {} },  /* <cntr L> */
{  13,  (Uchar)CCRETURN    , (Uchar *) NULL, 1, {} },  /* <cntr M> */
{  14,  (Uchar)CCRWORD     , (Uchar *) NULL, 1, {} },  /* <cntr N> */
{  15,  (Uchar)CCOPEN      , (Uchar *) NULL, 1, {} },  /* <cntr O> */
{  16,  (Uchar)CCPICK      , (Uchar *) NULL, 1, {} },  /* <cntr P> */
{  17,  (Uchar)CCUNAS1     , (Uchar *) NULL, 0, {} },  /* <cntr Q> */
{  18,  (Uchar)CCPLPAGE    , (Uchar *) NULL, 1, {} },  /* <cntr R> */
{  19,  (Uchar)CCUNAS1     , (Uchar *) NULL, 0, {} },  /* <cntr S> */
{  20,  (Uchar)CCMISRCH    , (Uchar *) NULL, 1, {} },  /* <cntr T> */
{  21,  (Uchar)CCMARK      , (Uchar *) NULL, 1, {} },  /* <cntr U> */
{  22,  (Uchar)CCCLOSE     , (Uchar *) NULL, 1, {} },  /* <cntr V> */
{  23,  (Uchar)CCDELCH     , (Uchar *) NULL, 1, {} },  /* <cntr W> */
{  24,  (Uchar)CCUNAS1     , (Uchar *) NULL, 0, {} },  /* <cntr X> */
{  25,  (Uchar)CCPLSRCH    , (Uchar *) NULL, 1, {} },  /* <cntr Y> */
{  26,  (Uchar)CCINSMODE   , (Uchar *) NULL, 1, {} },  /* <cntr Z> */
{  27,  (Uchar)CCINSMODE   , (Uchar *) NULL, 1, {} },  /* <cntr [> <esc> */
{  28,  (Uchar)CCINT       , (Uchar *) NULL, 1, {} },  /* <cntr \> */
{  29,  (Uchar)CCREPLACE   , (Uchar *) NULL, 1, {} },  /* <cntr ]> */
{  30,  (Uchar)CCERASE     , (Uchar *) NULL, 1, {} },  /* <cntr ^> */
{  31,  (Uchar)CCSETFILE   , (Uchar *) NULL, 1, {} }   /* <cntr _> */
};


sKey2Ecode KeyEsc[64];
