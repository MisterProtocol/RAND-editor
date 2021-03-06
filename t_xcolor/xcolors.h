typedef struct XRGB_table {
    int c_num;
    int r;
    int g;
    int b;
} XRGB;

/* x_rgb data came from xterm-256color.dat in the ncurses testing directory */
XRGB x_rgb[] = {
{0,        0,   0,   0},

#ifdef OUT
{1,      205,   0,   0},
{2,        0, 205,   0},
#endif

{1,      205, 205,   0},
{2,      255, 255,   0},

{3,      205, 205,   0},
{4,        0,   0, 238},
{5,      205,   0, 205},
{6,        0, 205, 205},
{7,      229, 229, 229},
{8,      127, 127, 127},
{9,      255,   0,   0},
{10,       0, 255,   0},
{11,     255, 255,   0},
{12,      92,  92, 255},
{13,     255,   0, 255},
{14,       0, 255, 255},
{15,     255, 255, 255},
{16,       0,   0,   0},
{17,       0,   0,  95},
{18,       0,   0, 135},
{19,       0,   0, 175},
{20,       0,   0, 215},
{21,       0,   0, 255},
{22,       0,  95,   0},
{23,       0,  95,  95},
{24,       0,  95, 135},
{25,       0,  95, 175},
{26,       0,  95, 215},
{27,       0,  95, 255},
{28,       0, 135,   0},
{29,       0, 135,  95},
{30,       0, 135, 135},
{31,       0, 135, 175},
{32,       0, 135, 215},
{33,       0, 135, 255},
{34,       0, 175,   0},
{35,       0, 175,  95},
{36,       0, 175, 135},
{37,       0, 175, 175},
{38,       0, 175, 215},
{39,       0, 175, 255},
{40,       0, 215,   0},
{41,       0, 215,  95},
{42,       0, 215, 135},
{43,       0, 215, 175},
{44,       0, 215, 215},
{45,       0, 215, 255},
{46,       0, 255,   0},
{47,       0, 255,  95},
{48,       0, 255, 135},
{49,       0, 255, 175},
{50,       0, 255, 215},
{51,       0, 255, 255},
{52,      95,   0,   0},
{53,      95,   0,  95},
{54,      95,   0, 135},
{55,      95,   0, 175},
{56,      95,   0, 215},
{57,      95,   0, 255},
{58,      95,  95,   0},
{59,      95,  95,  95},
{60,      95,  95, 135},
{61,      95,  95, 175},
{62,      95,  95, 215},
{63,      95,  95, 255},
{64,      95, 135,   0},
{65,      95, 135,  95},
{66,      95, 135, 135},
{67,      95, 135, 175},
{68,      95, 135, 215},
{69,      95, 135, 255},
{70,      95, 175,   0},
{71,      95, 175,  95},
{72,      95, 175, 135},
{73,      95, 175, 175},
{74,      95, 175, 215},
{75,      95, 175, 255},
{76,      95, 215,   0},
{77,      95, 215,  95},
{78,      95, 215, 135},
{79,      95, 215, 175},
{80,      95, 215, 215},
{81,      95, 215, 255},
{82,      95, 255,   0},
{83,      95, 255,  95},
{84,      95, 255, 135},
{85,      95, 255, 175},
{86,      95, 255, 215},
{87,      95, 255, 255},
{88,     135,   0,   0},
{89,     135,   0,  95},
{90,     135,   0, 135},
{91,     135,   0, 175},
{92,     135,   0, 215},
{93,     135,   0, 255},
{94,     135,  95,   0},
{95,     135,  95,  95},
{96,     135,  95, 135},
{97,     135,  95, 175},
{98,     135,  95, 215},
{99,     135,  95, 255},
{100,    135, 135,   0},
{101,    135, 135,  95},
{102,    135, 135, 135},
{103,    135, 135, 175},
{104,    135, 135, 215},
{105,    135, 135, 255},
{106,    135, 175,   0},
{107,    135, 175,  95},
{108,    135, 175, 135},
{109,    135, 175, 175},
{110,    135, 175, 215},
{111,    135, 175, 255},
{112,    135, 215,   0},
{113,    135, 215,  95},
{114,    135, 215, 135},
{115,    135, 215, 175},
{116,    135, 215, 215},
{117,    135, 215, 255},
{118,    135, 255,   0},
{119,    135, 255,  95},
{120,    135, 255, 135},
{121,    135, 255, 175},
{122,    135, 255, 215},
{123,    135, 255, 255},
{124,    175,   0,   0},
{125,    175,   0,  95},
{126,    175,   0, 135},
{127,    175,   0, 175},
{128,    175,   0, 215},
{129,    175,   0, 255},
{130,    175,  95,   0},
{131,    175,  95,  95},
{132,    175,  95, 135},
{133,    175,  95, 175},
{134,    175,  95, 215},
{135,    175,  95, 255},
{136,    175, 135,   0},
{137,    175, 135,  95},
{138,    175, 135, 135},
{139,    175, 135, 175},
{140,    175, 135, 215},
{141,    175, 135, 255},
{142,    175, 175,   0},
{143,    175, 175,  95},
{144,    175, 175, 135},
{145,    175, 175, 175},
{146,    175, 175, 215},
{147,    175, 175, 255},
{148,    175, 215,   0},
{149,    175, 215,  95},
{150,    175, 215, 135},
{151,    175, 215, 175},
{152,    175, 215, 215},
{153,    175, 215, 255},
{154,    175, 255,   0},
{155,    175, 255,  95},
{156,    175, 255, 135},
{157,    175, 255, 175},
{158,    175, 255, 215},
{159,    175, 255, 255},
{160,    215,   0,   0},
{161,    215,   0,  95},
{162,    215,   0, 135},
{163,    215,   0, 175},
{164,    215,   0, 215},
{165,    215,   0, 255},
{166,    215,  95,   0},
{167,    215,  95,  95},
{168,    215,  95, 135},
{169,    215,  95, 175},
{170,    215,  95, 215},
{171,    215,  95, 255},
{172,    215, 135,   0},
{173,    215, 135,  95},
{174,    215, 135, 135},
{175,    215, 135, 175},
{176,    215, 135, 215},
{177,    215, 135, 255},
{178,    215, 175,   0},
{179,    215, 175,  95},
{180,    215, 175, 135},
{181,    215, 175, 175},
{182,    215, 175, 215},
{183,    215, 175, 255},
{184,    215, 215,   0},
{185,    215, 215,  95},
{186,    215, 215, 135},
{187,    215, 215, 175},
{188,    215, 215, 215},
{189,    215, 215, 255},
{190,    215, 255,   0},
{191,    215, 255,  95},
{192,    215, 255, 135},
{193,    215, 255, 175},
{194,    215, 255, 215},
{195,    215, 255, 255},
{196,    255,   0,   0},
{197,    255,   0,  95},
{198,    255,   0, 135},
{199,    255,   0, 175},
{200,    255,   0, 215},
{201,    255,   0, 255},
{202,    255,  95,   0},
{203,    255,  95,  95},
{204,    255,  95, 135},
{205,    255,  95, 175},
{206,    255,  95, 215},
{207,    255,  95, 255},
{208,    255, 135,   0},
{209,    255, 135,  95},
{210,    255, 135, 135},
{211,    255, 135, 175},
{212,    255, 135, 215},
{213,    255, 135, 255},
{214,    255, 175,   0},
{215,    255, 175,  95},
{216,    255, 175, 135},
{217,    255, 175, 175},
{218,    255, 175, 215},
{219,    255, 175, 255},
{220,    255, 215,   0},
{221,    255, 215,  95},
{222,    255, 215, 135},
{223,    255, 215, 175},
{224,    255, 215, 215},
{225,    255, 215, 255},
{226,    255, 255,   0},
{227,    255, 255,  95},
{228,    255, 255, 135},
{229,    255, 255, 175},
{230,    255, 255, 215},
{231,    255, 255, 255},
{232,      8,   8,   8},
{233,     18,  18,  18},
{234,     28,  28,  28},
{235,     38,  38,  38},
{236,     48,  48,  48},
{237,     58,  58,  58},
{238,     68,  68,  68},
{239,     78,  78,  78},
{240,     88,  88,  88},
{241,     98,  98,  98},
{242,    108, 108, 108},
{243,    118, 118, 118},
{244,    128, 128, 128},
{245,    138, 138, 138},
{246,    148, 148, 148},
{247,    158, 158, 158},
{248,    168, 168, 168},
{249,    178, 178, 178},
{250,    188, 188, 188},
{251,    198, 198, 198},
{252,    208, 208, 208},
{253,    218, 218, 218},
{254,    228, 228, 228},
{255,    238, 238, 238},
{-1,     0,   0,   0}
};


/*  my_rgb data came from the output of "color_content -p",
 *  a pgm in the curses testing directory
 */
XRGB my_rgb[] = {
{0,  383, 886, 777},
{1,  915, 793, 335},
{2,  386, 492, 649},
{3,  421, 362, 27},
{4,  690, 59, 763},
{5,  926, 540, 426},
{6,  172, 736, 211},
{7,  368, 567, 429},
{8,  782, 530, 862},
{9,  123, 67, 135},
{10, 929, 802, 22},
{11, 58, 69, 167},
{12, 393, 456, 11},
{13, 42, 229, 373},
{14, 421, 919, 784},
{15, 537, 198, 324},
{16, 315, 370, 413},
{17, 526, 91, 980},
{18, 956, 873, 862},
{19, 170, 996, 281},
{20, 305, 925, 84},
{21, 327, 336, 505},
{22, 846, 729, 313},
{23, 857, 124, 895},
{24, 582, 545, 814},
{25, 367, 434, 364},
{26, 43, 750, 87},
{27, 808, 276, 178},
{28, 788, 584, 403},
{29, 651, 754, 399},
{30, 932, 60, 676},
{31, 368, 739, 12},
{32, 226, 586, 94},
{33, 539, 795, 570},
{34, 434, 378, 467},
{35, 601, 97, 902},
{36, 317, 492, 652},
{37, 756, 301, 280},
{38, 286, 441, 865},
{39, 689, 444, 619},
{40, 440, 729, 31},
{41, 117, 97, 771},
{42, 481, 675, 709},
{43, 927, 567, 856},
{44, 497, 353, 586},
{45, 965, 306, 683},
{46, 219, 624, 528},
{47, 871, 732, 829},
{48, 503, 19, 270},
{49, 368, 708, 715},
{50, 340, 149, 796},
{51, 723, 618, 245},
{52, 846, 451, 921},
{53, 555, 379, 488},
{54, 764, 228, 841},
{55, 350, 193, 500},
{56, 34, 764, 124},
{57, 914, 987, 856},
{58, 743, 491, 227},
{59, 365, 859, 936},
{60, 432, 551, 437},
{61, 228, 275, 407},
{62, 474, 121, 858},
{63, 395, 29, 237},
{64, 235, 793, 818},
{65, 428, 143, 11},
{66, 928, 529, 776},
{67, 404, 443, 763},
{68, 613, 538, 606},
{69, 840, 904, 818},
{70, 128, 688, 369},
{71, 917, 917, 996},
{72, 324, 743, 470},
{73, 183, 490, 499},
{74, 772, 725, 644},
{75, 590, 505, 139},
{76, 954, 786, 669},
{77, 82, 542, 464},
{78, 197, 507, 355},
{79, 804, 348, 611},
{80, 622, 828, 299},
{81, 343, 746, 568},
{82, 340, 422, 311},
{83, 810, 605, 801},
{84, 661, 730, 878},
{85, 305, 320, 736},
{86, 444, 626, 522},
{87, 465, 708, 416},
{88, 282, 258, 924},
{89, 637, 62, 624},
{90, 600, 36, 452},
{91, 899, 379, 550},
{92, 468, 71, 973},
{93, 131, 881, 930},
{94, 933, 894, 660},
{95, 163, 199, 981},
{96, 899, 996, 959},
{97, 773, 813, 668},
{98, 190, 95, 926},
{99, 466, 84, 340},
{100, 90, 684, 376},
{101, 542, 936, 107},
{102, 445, 756, 179},
{103, 418, 887, 412},
{104, 348, 172, 659},
{105, 9, 336, 210},
{106, 342, 587, 206},
{107, 301, 713, 372},
{108, 321, 255, 819},
{109, 599, 721, 904},
{110, 939, 811, 940},
{111, 667, 705, 228},
{112, 127, 150, 984},
{113, 658, 920, 224},
{114, 422, 269, 396},
{115, 81, 630, 84},
{116, 292, 972, 672},
{117, 850, 625, 385},
{118, 222, 299, 640},
{119, 42, 898, 713},
{120, 298, 190, 524},
{121, 590, 209, 581},
{122, 819, 336, 732},
{123, 155, 994, 4},
{124, 379, 769, 273},
{125, 776, 850, 255},
{126, 860, 142, 579},
{127, 884, 993, 205},
{128, 621, 567, 504},
{129, 613, 961, 754},
{130, 326, 259, 944},
{131, 202, 202, 506},
{132, 784, 21, 842},
{133, 868, 528, 189},
{134, 872, 908, 958},
{135, 498, 36, 808},
{136, 753, 248, 303},
{137, 333, 133, 648},
{138, 890, 754, 567},
{139, 746, 368, 529},
{140, 500, 46, 788},
{141, 797, 249, 990},
{142, 303, 33, 363},
{143, 497, 253, 892},
{144, 686, 125, 152},
{145, 996, 975, 188},
{146, 157, 729, 436},
{147, 460, 414, 921},
{148, 460, 304, 28},
{149, 27, 50, 748},
{150, 556, 902, 794},
{151, 697, 699, 43},
{152, 39, 2, 428},
{153, 403, 500, 681},
{154, 647, 538, 159},
{155, 151, 535, 134},
{156, 339, 692, 215},
{157, 127, 504, 629},
{158, 49, 964, 285},
{159, 429, 343, 335},
{160, 177, 900, 238},
{161, 971, 949, 289},
{162, 367, 988, 292},
{163, 795, 743, 144},
{164, 829, 390, 682},
{165, 340, 541, 569},
{166, 826, 232, 261},
{167, 42, 360, 117},
{168, 23, 761, 81},
{169, 309, 190, 425},
{170, 996, 367, 677},
{171, 234, 690, 626},
{172, 524, 57, 614},
{173, 168, 205, 358},
{174, 312, 386, 100},
{175, 346, 726, 994},
{176, 916, 552, 578},
{177, 529, 946, 290},
{178, 647, 970, 51},
{179, 80, 631, 593},
{180, 857, 627, 312},
{181, 886, 214, 355},
{182, 512, 90, 412},
{183, 479, 610, 969},
{184, 189, 274, 355},
{185, 641, 620, 433},
{186, 987, 888, 338},
{187, 566, 770, 284},
{188, 856, 417, 606},
{189, 260, 849, 237},
{190, 205, 59, 217},
{191, 518, 945, 783},
{192, 873, 458, 873},
{193, 637, 289, 483},
{194, 607, 478, 757},
{195, 314, 471, 729},
{196, 100, 459, 618},
{197, 438, 25, 388},
{198, 74, 233, 157},
{199, 681, 493, 358},
{200, 270, 699, 417},
{201, 839, 569, 363},
{202, 622, 794, 173},
{203, 847, 431, 462},
{204, 682, 390, 292},
{205, 791, 57, 115},
{206, 521, 157, 574},
{207, 491, 947, 951},
{208, 231, 21, 537},
{209, 740, 54, 30},
{210, 98, 325, 81},
{211, 516, 516, 2},
{212, 231, 139, 796},
{213, 404, 338, 580},
{214, 218, 21, 970},
{215, 862, 812, 379},
{216, 977, 685, 536},
{217, 904, 176, 483},
{218, 207, 759, 857},
{219, 744, 499, 911},
{220, 127, 950, 236},
{221, 560, 818, 105},
{222, 563, 49, 244},
{223, 711, 805, 934},
{224, 291, 375, 955},
{225, 614, 589, 768},
{226, 993, 918, 805},
{227, 882, 822, 982},
{228, 717, 30, 93},
{229, 574, 126, 593},
{230, 486, 253, 543},
{231, 74, 814, 713},
{232, 179, 377, 762},
{233, 775, 88, 919},
{234, 710, 732, 294},
{235, 17, 346, 235},
{236, 137, 691, 153},
{237, 943, 573, 328},
{238, 925, 291, 710},
{239, 18, 217, 836},
{240, 963, 55, 90},
{241, 858, 130, 904},
{242, 571, 661, 633},
{243, 685, 789, 73},
{244, 604, 851, 805},
{245, 250, 868, 503},
{246, 485, 6, 195},
{247, 639, 949, 120},
{248, 967, 226, 763},
{249, 677, 596, 981},
{250, 865, 560, 36},
{251, 955, 770, 518},
{252, 211, 342, 532},
{253, 196, 379, 321},
{254, 270, 984, 172},
{255, 427, 234, 40},
{-1, 0, 0, 0}
};
