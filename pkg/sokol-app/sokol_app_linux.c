// ██      ██ ███    ██ ██    ██ ██   ██
// ██      ██ ████   ██ ██    ██  ██ ██
// ██      ██ ██ ██  ██ ██    ██   ███
// ██      ██ ██  ██ ██ ██    ██  ██ ██
// ███████ ██ ██   ████  ██████  ██   ██
//
// >>linux

/* see GLFW's xkb_unicode.c */
static const struct _sapp_x11_codepair {
  uint16_t keysym;
  uint16_t ucs;
} _sapp_x11_keysymtab[] = {
  { 0x01a1, 0x0104 },
  { 0x01a2, 0x02d8 },
  { 0x01a3, 0x0141 },
  { 0x01a5, 0x013d },
  { 0x01a6, 0x015a },
  { 0x01a9, 0x0160 },
  { 0x01aa, 0x015e },
  { 0x01ab, 0x0164 },
  { 0x01ac, 0x0179 },
  { 0x01ae, 0x017d },
  { 0x01af, 0x017b },
  { 0x01b1, 0x0105 },
  { 0x01b2, 0x02db },
  { 0x01b3, 0x0142 },
  { 0x01b5, 0x013e },
  { 0x01b6, 0x015b },
  { 0x01b7, 0x02c7 },
  { 0x01b9, 0x0161 },
  { 0x01ba, 0x015f },
  { 0x01bb, 0x0165 },
  { 0x01bc, 0x017a },
  { 0x01bd, 0x02dd },
  { 0x01be, 0x017e },
  { 0x01bf, 0x017c },
  { 0x01c0, 0x0154 },
  { 0x01c3, 0x0102 },
  { 0x01c5, 0x0139 },
  { 0x01c6, 0x0106 },
  { 0x01c8, 0x010c },
  { 0x01ca, 0x0118 },
  { 0x01cc, 0x011a },
  { 0x01cf, 0x010e },
  { 0x01d0, 0x0110 },
  { 0x01d1, 0x0143 },
  { 0x01d2, 0x0147 },
  { 0x01d5, 0x0150 },
  { 0x01d8, 0x0158 },
  { 0x01d9, 0x016e },
  { 0x01db, 0x0170 },
  { 0x01de, 0x0162 },
  { 0x01e0, 0x0155 },
  { 0x01e3, 0x0103 },
  { 0x01e5, 0x013a },
  { 0x01e6, 0x0107 },
  { 0x01e8, 0x010d },
  { 0x01ea, 0x0119 },
  { 0x01ec, 0x011b },
  { 0x01ef, 0x010f },
  { 0x01f0, 0x0111 },
  { 0x01f1, 0x0144 },
  { 0x01f2, 0x0148 },
  { 0x01f5, 0x0151 },
  { 0x01f8, 0x0159 },
  { 0x01f9, 0x016f },
  { 0x01fb, 0x0171 },
  { 0x01fe, 0x0163 },
  { 0x01ff, 0x02d9 },
  { 0x02a1, 0x0126 },
  { 0x02a6, 0x0124 },
  { 0x02a9, 0x0130 },
  { 0x02ab, 0x011e },
  { 0x02ac, 0x0134 },
  { 0x02b1, 0x0127 },
  { 0x02b6, 0x0125 },
  { 0x02b9, 0x0131 },
  { 0x02bb, 0x011f },
  { 0x02bc, 0x0135 },
  { 0x02c5, 0x010a },
  { 0x02c6, 0x0108 },
  { 0x02d5, 0x0120 },
  { 0x02d8, 0x011c },
  { 0x02dd, 0x016c },
  { 0x02de, 0x015c },
  { 0x02e5, 0x010b },
  { 0x02e6, 0x0109 },
  { 0x02f5, 0x0121 },
  { 0x02f8, 0x011d },
  { 0x02fd, 0x016d },
  { 0x02fe, 0x015d },
  { 0x03a2, 0x0138 },
  { 0x03a3, 0x0156 },
  { 0x03a5, 0x0128 },
  { 0x03a6, 0x013b },
  { 0x03aa, 0x0112 },
  { 0x03ab, 0x0122 },
  { 0x03ac, 0x0166 },
  { 0x03b3, 0x0157 },
  { 0x03b5, 0x0129 },
  { 0x03b6, 0x013c },
  { 0x03ba, 0x0113 },
  { 0x03bb, 0x0123 },
  { 0x03bc, 0x0167 },
  { 0x03bd, 0x014a },
  { 0x03bf, 0x014b },
  { 0x03c0, 0x0100 },
  { 0x03c7, 0x012e },
  { 0x03cc, 0x0116 },
  { 0x03cf, 0x012a },
  { 0x03d1, 0x0145 },
  { 0x03d2, 0x014c },
  { 0x03d3, 0x0136 },
  { 0x03d9, 0x0172 },
  { 0x03dd, 0x0168 },
  { 0x03de, 0x016a },
  { 0x03e0, 0x0101 },
  { 0x03e7, 0x012f },
  { 0x03ec, 0x0117 },
  { 0x03ef, 0x012b },
  { 0x03f1, 0x0146 },
  { 0x03f2, 0x014d },
  { 0x03f3, 0x0137 },
  { 0x03f9, 0x0173 },
  { 0x03fd, 0x0169 },
  { 0x03fe, 0x016b },
  { 0x047e, 0x203e },
  { 0x04a1, 0x3002 },
  { 0x04a2, 0x300c },
  { 0x04a3, 0x300d },
  { 0x04a4, 0x3001 },
  { 0x04a5, 0x30fb },
  { 0x04a6, 0x30f2 },
  { 0x04a7, 0x30a1 },
  { 0x04a8, 0x30a3 },
  { 0x04a9, 0x30a5 },
  { 0x04aa, 0x30a7 },
  { 0x04ab, 0x30a9 },
  { 0x04ac, 0x30e3 },
  { 0x04ad, 0x30e5 },
  { 0x04ae, 0x30e7 },
  { 0x04af, 0x30c3 },
  { 0x04b0, 0x30fc },
  { 0x04b1, 0x30a2 },
  { 0x04b2, 0x30a4 },
  { 0x04b3, 0x30a6 },
  { 0x04b4, 0x30a8 },
  { 0x04b5, 0x30aa },
  { 0x04b6, 0x30ab },
  { 0x04b7, 0x30ad },
  { 0x04b8, 0x30af },
  { 0x04b9, 0x30b1 },
  { 0x04ba, 0x30b3 },
  { 0x04bb, 0x30b5 },
  { 0x04bc, 0x30b7 },
  { 0x04bd, 0x30b9 },
  { 0x04be, 0x30bb },
  { 0x04bf, 0x30bd },
  { 0x04c0, 0x30bf },
  { 0x04c1, 0x30c1 },
  { 0x04c2, 0x30c4 },
  { 0x04c3, 0x30c6 },
  { 0x04c4, 0x30c8 },
  { 0x04c5, 0x30ca },
  { 0x04c6, 0x30cb },
  { 0x04c7, 0x30cc },
  { 0x04c8, 0x30cd },
  { 0x04c9, 0x30ce },
  { 0x04ca, 0x30cf },
  { 0x04cb, 0x30d2 },
  { 0x04cc, 0x30d5 },
  { 0x04cd, 0x30d8 },
  { 0x04ce, 0x30db },
  { 0x04cf, 0x30de },
  { 0x04d0, 0x30df },
  { 0x04d1, 0x30e0 },
  { 0x04d2, 0x30e1 },
  { 0x04d3, 0x30e2 },
  { 0x04d4, 0x30e4 },
  { 0x04d5, 0x30e6 },
  { 0x04d6, 0x30e8 },
  { 0x04d7, 0x30e9 },
  { 0x04d8, 0x30ea },
  { 0x04d9, 0x30eb },
  { 0x04da, 0x30ec },
  { 0x04db, 0x30ed },
  { 0x04dc, 0x30ef },
  { 0x04dd, 0x30f3 },
  { 0x04de, 0x309b },
  { 0x04df, 0x309c },
  { 0x05ac, 0x060c },
  { 0x05bb, 0x061b },
  { 0x05bf, 0x061f },
  { 0x05c1, 0x0621 },
  { 0x05c2, 0x0622 },
  { 0x05c3, 0x0623 },
  { 0x05c4, 0x0624 },
  { 0x05c5, 0x0625 },
  { 0x05c6, 0x0626 },
  { 0x05c7, 0x0627 },
  { 0x05c8, 0x0628 },
  { 0x05c9, 0x0629 },
  { 0x05ca, 0x062a },
  { 0x05cb, 0x062b },
  { 0x05cc, 0x062c },
  { 0x05cd, 0x062d },
  { 0x05ce, 0x062e },
  { 0x05cf, 0x062f },
  { 0x05d0, 0x0630 },
  { 0x05d1, 0x0631 },
  { 0x05d2, 0x0632 },
  { 0x05d3, 0x0633 },
  { 0x05d4, 0x0634 },
  { 0x05d5, 0x0635 },
  { 0x05d6, 0x0636 },
  { 0x05d7, 0x0637 },
  { 0x05d8, 0x0638 },
  { 0x05d9, 0x0639 },
  { 0x05da, 0x063a },
  { 0x05e0, 0x0640 },
  { 0x05e1, 0x0641 },
  { 0x05e2, 0x0642 },
  { 0x05e3, 0x0643 },
  { 0x05e4, 0x0644 },
  { 0x05e5, 0x0645 },
  { 0x05e6, 0x0646 },
  { 0x05e7, 0x0647 },
  { 0x05e8, 0x0648 },
  { 0x05e9, 0x0649 },
  { 0x05ea, 0x064a },
  { 0x05eb, 0x064b },
  { 0x05ec, 0x064c },
  { 0x05ed, 0x064d },
  { 0x05ee, 0x064e },
  { 0x05ef, 0x064f },
  { 0x05f0, 0x0650 },
  { 0x05f1, 0x0651 },
  { 0x05f2, 0x0652 },
  { 0x06a1, 0x0452 },
  { 0x06a2, 0x0453 },
  { 0x06a3, 0x0451 },
  { 0x06a4, 0x0454 },
  { 0x06a5, 0x0455 },
  { 0x06a6, 0x0456 },
  { 0x06a7, 0x0457 },
  { 0x06a8, 0x0458 },
  { 0x06a9, 0x0459 },
  { 0x06aa, 0x045a },
  { 0x06ab, 0x045b },
  { 0x06ac, 0x045c },
  { 0x06ae, 0x045e },
  { 0x06af, 0x045f },
  { 0x06b0, 0x2116 },
  { 0x06b1, 0x0402 },
  { 0x06b2, 0x0403 },
  { 0x06b3, 0x0401 },
  { 0x06b4, 0x0404 },
  { 0x06b5, 0x0405 },
  { 0x06b6, 0x0406 },
  { 0x06b7, 0x0407 },
  { 0x06b8, 0x0408 },
  { 0x06b9, 0x0409 },
  { 0x06ba, 0x040a },
  { 0x06bb, 0x040b },
  { 0x06bc, 0x040c },
  { 0x06be, 0x040e },
  { 0x06bf, 0x040f },
  { 0x06c0, 0x044e },
  { 0x06c1, 0x0430 },
  { 0x06c2, 0x0431 },
  { 0x06c3, 0x0446 },
  { 0x06c4, 0x0434 },
  { 0x06c5, 0x0435 },
  { 0x06c6, 0x0444 },
  { 0x06c7, 0x0433 },
  { 0x06c8, 0x0445 },
  { 0x06c9, 0x0438 },
  { 0x06ca, 0x0439 },
  { 0x06cb, 0x043a },
  { 0x06cc, 0x043b },
  { 0x06cd, 0x043c },
  { 0x06ce, 0x043d },
  { 0x06cf, 0x043e },
  { 0x06d0, 0x043f },
  { 0x06d1, 0x044f },
  { 0x06d2, 0x0440 },
  { 0x06d3, 0x0441 },
  { 0x06d4, 0x0442 },
  { 0x06d5, 0x0443 },
  { 0x06d6, 0x0436 },
  { 0x06d7, 0x0432 },
  { 0x06d8, 0x044c },
  { 0x06d9, 0x044b },
  { 0x06da, 0x0437 },
  { 0x06db, 0x0448 },
  { 0x06dc, 0x044d },
  { 0x06dd, 0x0449 },
  { 0x06de, 0x0447 },
  { 0x06df, 0x044a },
  { 0x06e0, 0x042e },
  { 0x06e1, 0x0410 },
  { 0x06e2, 0x0411 },
  { 0x06e3, 0x0426 },
  { 0x06e4, 0x0414 },
  { 0x06e5, 0x0415 },
  { 0x06e6, 0x0424 },
  { 0x06e7, 0x0413 },
  { 0x06e8, 0x0425 },
  { 0x06e9, 0x0418 },
  { 0x06ea, 0x0419 },
  { 0x06eb, 0x041a },
  { 0x06ec, 0x041b },
  { 0x06ed, 0x041c },
  { 0x06ee, 0x041d },
  { 0x06ef, 0x041e },
  { 0x06f0, 0x041f },
  { 0x06f1, 0x042f },
  { 0x06f2, 0x0420 },
  { 0x06f3, 0x0421 },
  { 0x06f4, 0x0422 },
  { 0x06f5, 0x0423 },
  { 0x06f6, 0x0416 },
  { 0x06f7, 0x0412 },
  { 0x06f8, 0x042c },
  { 0x06f9, 0x042b },
  { 0x06fa, 0x0417 },
  { 0x06fb, 0x0428 },
  { 0x06fc, 0x042d },
  { 0x06fd, 0x0429 },
  { 0x06fe, 0x0427 },
  { 0x06ff, 0x042a },
  { 0x07a1, 0x0386 },
  { 0x07a2, 0x0388 },
  { 0x07a3, 0x0389 },
  { 0x07a4, 0x038a },
  { 0x07a5, 0x03aa },
  { 0x07a7, 0x038c },
  { 0x07a8, 0x038e },
  { 0x07a9, 0x03ab },
  { 0x07ab, 0x038f },
  { 0x07ae, 0x0385 },
  { 0x07af, 0x2015 },
  { 0x07b1, 0x03ac },
  { 0x07b2, 0x03ad },
  { 0x07b3, 0x03ae },
  { 0x07b4, 0x03af },
  { 0x07b5, 0x03ca },
  { 0x07b6, 0x0390 },
  { 0x07b7, 0x03cc },
  { 0x07b8, 0x03cd },
  { 0x07b9, 0x03cb },
  { 0x07ba, 0x03b0 },
  { 0x07bb, 0x03ce },
  { 0x07c1, 0x0391 },
  { 0x07c2, 0x0392 },
  { 0x07c3, 0x0393 },
  { 0x07c4, 0x0394 },
  { 0x07c5, 0x0395 },
  { 0x07c6, 0x0396 },
  { 0x07c7, 0x0397 },
  { 0x07c8, 0x0398 },
  { 0x07c9, 0x0399 },
  { 0x07ca, 0x039a },
  { 0x07cb, 0x039b },
  { 0x07cc, 0x039c },
  { 0x07cd, 0x039d },
  { 0x07ce, 0x039e },
  { 0x07cf, 0x039f },
  { 0x07d0, 0x03a0 },
  { 0x07d1, 0x03a1 },
  { 0x07d2, 0x03a3 },
  { 0x07d4, 0x03a4 },
  { 0x07d5, 0x03a5 },
  { 0x07d6, 0x03a6 },
  { 0x07d7, 0x03a7 },
  { 0x07d8, 0x03a8 },
  { 0x07d9, 0x03a9 },
  { 0x07e1, 0x03b1 },
  { 0x07e2, 0x03b2 },
  { 0x07e3, 0x03b3 },
  { 0x07e4, 0x03b4 },
  { 0x07e5, 0x03b5 },
  { 0x07e6, 0x03b6 },
  { 0x07e7, 0x03b7 },
  { 0x07e8, 0x03b8 },
  { 0x07e9, 0x03b9 },
  { 0x07ea, 0x03ba },
  { 0x07eb, 0x03bb },
  { 0x07ec, 0x03bc },
  { 0x07ed, 0x03bd },
  { 0x07ee, 0x03be },
  { 0x07ef, 0x03bf },
  { 0x07f0, 0x03c0 },
  { 0x07f1, 0x03c1 },
  { 0x07f2, 0x03c3 },
  { 0x07f3, 0x03c2 },
  { 0x07f4, 0x03c4 },
  { 0x07f5, 0x03c5 },
  { 0x07f6, 0x03c6 },
  { 0x07f7, 0x03c7 },
  { 0x07f8, 0x03c8 },
  { 0x07f9, 0x03c9 },
  { 0x08a1, 0x23b7 },
  { 0x08a2, 0x250c },
  { 0x08a3, 0x2500 },
  { 0x08a4, 0x2320 },
  { 0x08a5, 0x2321 },
  { 0x08a6, 0x2502 },
  { 0x08a7, 0x23a1 },
  { 0x08a8, 0x23a3 },
  { 0x08a9, 0x23a4 },
  { 0x08aa, 0x23a6 },
  { 0x08ab, 0x239b },
  { 0x08ac, 0x239d },
  { 0x08ad, 0x239e },
  { 0x08ae, 0x23a0 },
  { 0x08af, 0x23a8 },
  { 0x08b0, 0x23ac },
  { 0x08bc, 0x2264 },
  { 0x08bd, 0x2260 },
  { 0x08be, 0x2265 },
  { 0x08bf, 0x222b },
  { 0x08c0, 0x2234 },
  { 0x08c1, 0x221d },
  { 0x08c2, 0x221e },
  { 0x08c5, 0x2207 },
  { 0x08c8, 0x223c },
  { 0x08c9, 0x2243 },
  { 0x08cd, 0x21d4 },
  { 0x08ce, 0x21d2 },
  { 0x08cf, 0x2261 },
  { 0x08d6, 0x221a },
  { 0x08da, 0x2282 },
  { 0x08db, 0x2283 },
  { 0x08dc, 0x2229 },
  { 0x08dd, 0x222a },
  { 0x08de, 0x2227 },
  { 0x08df, 0x2228 },
  { 0x08ef, 0x2202 },
  { 0x08f6, 0x0192 },
  { 0x08fb, 0x2190 },
  { 0x08fc, 0x2191 },
  { 0x08fd, 0x2192 },
  { 0x08fe, 0x2193 },
  { 0x09e0, 0x25c6 },
  { 0x09e1, 0x2592 },
  { 0x09e2, 0x2409 },
  { 0x09e3, 0x240c },
  { 0x09e4, 0x240d },
  { 0x09e5, 0x240a },
  { 0x09e8, 0x2424 },
  { 0x09e9, 0x240b },
  { 0x09ea, 0x2518 },
  { 0x09eb, 0x2510 },
  { 0x09ec, 0x250c },
  { 0x09ed, 0x2514 },
  { 0x09ee, 0x253c },
  { 0x09ef, 0x23ba },
  { 0x09f0, 0x23bb },
  { 0x09f1, 0x2500 },
  { 0x09f2, 0x23bc },
  { 0x09f3, 0x23bd },
  { 0x09f4, 0x251c },
  { 0x09f5, 0x2524 },
  { 0x09f6, 0x2534 },
  { 0x09f7, 0x252c },
  { 0x09f8, 0x2502 },
  { 0x0aa1, 0x2003 },
  { 0x0aa2, 0x2002 },
  { 0x0aa3, 0x2004 },
  { 0x0aa4, 0x2005 },
  { 0x0aa5, 0x2007 },
  { 0x0aa6, 0x2008 },
  { 0x0aa7, 0x2009 },
  { 0x0aa8, 0x200a },
  { 0x0aa9, 0x2014 },
  { 0x0aaa, 0x2013 },
  { 0x0aae, 0x2026 },
  { 0x0aaf, 0x2025 },
  { 0x0ab0, 0x2153 },
  { 0x0ab1, 0x2154 },
  { 0x0ab2, 0x2155 },
  { 0x0ab3, 0x2156 },
  { 0x0ab4, 0x2157 },
  { 0x0ab5, 0x2158 },
  { 0x0ab6, 0x2159 },
  { 0x0ab7, 0x215a },
  { 0x0ab8, 0x2105 },
  { 0x0abb, 0x2012 },
  { 0x0abc, 0x2329 },
  { 0x0abe, 0x232a },
  { 0x0ac3, 0x215b },
  { 0x0ac4, 0x215c },
  { 0x0ac5, 0x215d },
  { 0x0ac6, 0x215e },
  { 0x0ac9, 0x2122 },
  { 0x0aca, 0x2613 },
  { 0x0acc, 0x25c1 },
  { 0x0acd, 0x25b7 },
  { 0x0ace, 0x25cb },
  { 0x0acf, 0x25af },
  { 0x0ad0, 0x2018 },
  { 0x0ad1, 0x2019 },
  { 0x0ad2, 0x201c },
  { 0x0ad3, 0x201d },
  { 0x0ad4, 0x211e },
  { 0x0ad6, 0x2032 },
  { 0x0ad7, 0x2033 },
  { 0x0ad9, 0x271d },
  { 0x0adb, 0x25ac },
  { 0x0adc, 0x25c0 },
  { 0x0add, 0x25b6 },
  { 0x0ade, 0x25cf },
  { 0x0adf, 0x25ae },
  { 0x0ae0, 0x25e6 },
  { 0x0ae1, 0x25ab },
  { 0x0ae2, 0x25ad },
  { 0x0ae3, 0x25b3 },
  { 0x0ae4, 0x25bd },
  { 0x0ae5, 0x2606 },
  { 0x0ae6, 0x2022 },
  { 0x0ae7, 0x25aa },
  { 0x0ae8, 0x25b2 },
  { 0x0ae9, 0x25bc },
  { 0x0aea, 0x261c },
  { 0x0aeb, 0x261e },
  { 0x0aec, 0x2663 },
  { 0x0aed, 0x2666 },
  { 0x0aee, 0x2665 },
  { 0x0af0, 0x2720 },
  { 0x0af1, 0x2020 },
  { 0x0af2, 0x2021 },
  { 0x0af3, 0x2713 },
  { 0x0af4, 0x2717 },
  { 0x0af5, 0x266f },
  { 0x0af6, 0x266d },
  { 0x0af7, 0x2642 },
  { 0x0af8, 0x2640 },
  { 0x0af9, 0x260e },
  { 0x0afa, 0x2315 },
  { 0x0afb, 0x2117 },
  { 0x0afc, 0x2038 },
  { 0x0afd, 0x201a },
  { 0x0afe, 0x201e },
  { 0x0ba3, 0x003c },
  { 0x0ba6, 0x003e },
  { 0x0ba8, 0x2228 },
  { 0x0ba9, 0x2227 },
  { 0x0bc0, 0x00af },
  { 0x0bc2, 0x22a5 },
  { 0x0bc3, 0x2229 },
  { 0x0bc4, 0x230a },
  { 0x0bc6, 0x005f },
  { 0x0bca, 0x2218 },
  { 0x0bcc, 0x2395 },
  { 0x0bce, 0x22a4 },
  { 0x0bcf, 0x25cb },
  { 0x0bd3, 0x2308 },
  { 0x0bd6, 0x222a },
  { 0x0bd8, 0x2283 },
  { 0x0bda, 0x2282 },
  { 0x0bdc, 0x22a2 },
  { 0x0bfc, 0x22a3 },
  { 0x0cdf, 0x2017 },
  { 0x0ce0, 0x05d0 },
  { 0x0ce1, 0x05d1 },
  { 0x0ce2, 0x05d2 },
  { 0x0ce3, 0x05d3 },
  { 0x0ce4, 0x05d4 },
  { 0x0ce5, 0x05d5 },
  { 0x0ce6, 0x05d6 },
  { 0x0ce7, 0x05d7 },
  { 0x0ce8, 0x05d8 },
  { 0x0ce9, 0x05d9 },
  { 0x0cea, 0x05da },
  { 0x0ceb, 0x05db },
  { 0x0cec, 0x05dc },
  { 0x0ced, 0x05dd },
  { 0x0cee, 0x05de },
  { 0x0cef, 0x05df },
  { 0x0cf0, 0x05e0 },
  { 0x0cf1, 0x05e1 },
  { 0x0cf2, 0x05e2 },
  { 0x0cf3, 0x05e3 },
  { 0x0cf4, 0x05e4 },
  { 0x0cf5, 0x05e5 },
  { 0x0cf6, 0x05e6 },
  { 0x0cf7, 0x05e7 },
  { 0x0cf8, 0x05e8 },
  { 0x0cf9, 0x05e9 },
  { 0x0cfa, 0x05ea },
  { 0x0da1, 0x0e01 },
  { 0x0da2, 0x0e02 },
  { 0x0da3, 0x0e03 },
  { 0x0da4, 0x0e04 },
  { 0x0da5, 0x0e05 },
  { 0x0da6, 0x0e06 },
  { 0x0da7, 0x0e07 },
  { 0x0da8, 0x0e08 },
  { 0x0da9, 0x0e09 },
  { 0x0daa, 0x0e0a },
  { 0x0dab, 0x0e0b },
  { 0x0dac, 0x0e0c },
  { 0x0dad, 0x0e0d },
  { 0x0dae, 0x0e0e },
  { 0x0daf, 0x0e0f },
  { 0x0db0, 0x0e10 },
  { 0x0db1, 0x0e11 },
  { 0x0db2, 0x0e12 },
  { 0x0db3, 0x0e13 },
  { 0x0db4, 0x0e14 },
  { 0x0db5, 0x0e15 },
  { 0x0db6, 0x0e16 },
  { 0x0db7, 0x0e17 },
  { 0x0db8, 0x0e18 },
  { 0x0db9, 0x0e19 },
  { 0x0dba, 0x0e1a },
  { 0x0dbb, 0x0e1b },
  { 0x0dbc, 0x0e1c },
  { 0x0dbd, 0x0e1d },
  { 0x0dbe, 0x0e1e },
  { 0x0dbf, 0x0e1f },
  { 0x0dc0, 0x0e20 },
  { 0x0dc1, 0x0e21 },
  { 0x0dc2, 0x0e22 },
  { 0x0dc3, 0x0e23 },
  { 0x0dc4, 0x0e24 },
  { 0x0dc5, 0x0e25 },
  { 0x0dc6, 0x0e26 },
  { 0x0dc7, 0x0e27 },
  { 0x0dc8, 0x0e28 },
  { 0x0dc9, 0x0e29 },
  { 0x0dca, 0x0e2a },
  { 0x0dcb, 0x0e2b },
  { 0x0dcc, 0x0e2c },
  { 0x0dcd, 0x0e2d },
  { 0x0dce, 0x0e2e },
  { 0x0dcf, 0x0e2f },
  { 0x0dd0, 0x0e30 },
  { 0x0dd1, 0x0e31 },
  { 0x0dd2, 0x0e32 },
  { 0x0dd3, 0x0e33 },
  { 0x0dd4, 0x0e34 },
  { 0x0dd5, 0x0e35 },
  { 0x0dd6, 0x0e36 },
  { 0x0dd7, 0x0e37 },
  { 0x0dd8, 0x0e38 },
  { 0x0dd9, 0x0e39 },
  { 0x0dda, 0x0e3a },
  { 0x0ddf, 0x0e3f },
  { 0x0de0, 0x0e40 },
  { 0x0de1, 0x0e41 },
  { 0x0de2, 0x0e42 },
  { 0x0de3, 0x0e43 },
  { 0x0de4, 0x0e44 },
  { 0x0de5, 0x0e45 },
  { 0x0de6, 0x0e46 },
  { 0x0de7, 0x0e47 },
  { 0x0de8, 0x0e48 },
  { 0x0de9, 0x0e49 },
  { 0x0dea, 0x0e4a },
  { 0x0deb, 0x0e4b },
  { 0x0dec, 0x0e4c },
  { 0x0ded, 0x0e4d },
  { 0x0df0, 0x0e50 },
  { 0x0df1, 0x0e51 },
  { 0x0df2, 0x0e52 },
  { 0x0df3, 0x0e53 },
  { 0x0df4, 0x0e54 },
  { 0x0df5, 0x0e55 },
  { 0x0df6, 0x0e56 },
  { 0x0df7, 0x0e57 },
  { 0x0df8, 0x0e58 },
  { 0x0df9, 0x0e59 },
  { 0x0ea1, 0x3131 },
  { 0x0ea2, 0x3132 },
  { 0x0ea3, 0x3133 },
  { 0x0ea4, 0x3134 },
  { 0x0ea5, 0x3135 },
  { 0x0ea6, 0x3136 },
  { 0x0ea7, 0x3137 },
  { 0x0ea8, 0x3138 },
  { 0x0ea9, 0x3139 },
  { 0x0eaa, 0x313a },
  { 0x0eab, 0x313b },
  { 0x0eac, 0x313c },
  { 0x0ead, 0x313d },
  { 0x0eae, 0x313e },
  { 0x0eaf, 0x313f },
  { 0x0eb0, 0x3140 },
  { 0x0eb1, 0x3141 },
  { 0x0eb2, 0x3142 },
  { 0x0eb3, 0x3143 },
  { 0x0eb4, 0x3144 },
  { 0x0eb5, 0x3145 },
  { 0x0eb6, 0x3146 },
  { 0x0eb7, 0x3147 },
  { 0x0eb8, 0x3148 },
  { 0x0eb9, 0x3149 },
  { 0x0eba, 0x314a },
  { 0x0ebb, 0x314b },
  { 0x0ebc, 0x314c },
  { 0x0ebd, 0x314d },
  { 0x0ebe, 0x314e },
  { 0x0ebf, 0x314f },
  { 0x0ec0, 0x3150 },
  { 0x0ec1, 0x3151 },
  { 0x0ec2, 0x3152 },
  { 0x0ec3, 0x3153 },
  { 0x0ec4, 0x3154 },
  { 0x0ec5, 0x3155 },
  { 0x0ec6, 0x3156 },
  { 0x0ec7, 0x3157 },
  { 0x0ec8, 0x3158 },
  { 0x0ec9, 0x3159 },
  { 0x0eca, 0x315a },
  { 0x0ecb, 0x315b },
  { 0x0ecc, 0x315c },
  { 0x0ecd, 0x315d },
  { 0x0ece, 0x315e },
  { 0x0ecf, 0x315f },
  { 0x0ed0, 0x3160 },
  { 0x0ed1, 0x3161 },
  { 0x0ed2, 0x3162 },
  { 0x0ed3, 0x3163 },
  { 0x0ed4, 0x11a8 },
  { 0x0ed5, 0x11a9 },
  { 0x0ed6, 0x11aa },
  { 0x0ed7, 0x11ab },
  { 0x0ed8, 0x11ac },
  { 0x0ed9, 0x11ad },
  { 0x0eda, 0x11ae },
  { 0x0edb, 0x11af },
  { 0x0edc, 0x11b0 },
  { 0x0edd, 0x11b1 },
  { 0x0ede, 0x11b2 },
  { 0x0edf, 0x11b3 },
  { 0x0ee0, 0x11b4 },
  { 0x0ee1, 0x11b5 },
  { 0x0ee2, 0x11b6 },
  { 0x0ee3, 0x11b7 },
  { 0x0ee4, 0x11b8 },
  { 0x0ee5, 0x11b9 },
  { 0x0ee6, 0x11ba },
  { 0x0ee7, 0x11bb },
  { 0x0ee8, 0x11bc },
  { 0x0ee9, 0x11bd },
  { 0x0eea, 0x11be },
  { 0x0eeb, 0x11bf },
  { 0x0eec, 0x11c0 },
  { 0x0eed, 0x11c1 },
  { 0x0eee, 0x11c2 },
  { 0x0eef, 0x316d },
  { 0x0ef0, 0x3171 },
  { 0x0ef1, 0x3178 },
  { 0x0ef2, 0x317f },
  { 0x0ef3, 0x3181 },
  { 0x0ef4, 0x3184 },
  { 0x0ef5, 0x3186 },
  { 0x0ef6, 0x318d },
  { 0x0ef7, 0x318e },
  { 0x0ef8, 0x11eb },
  { 0x0ef9, 0x11f0 },
  { 0x0efa, 0x11f9 },
  { 0x0eff, 0x20a9 },
  { 0x13a4, 0x20ac },
  { 0x13bc, 0x0152 },
  { 0x13bd, 0x0153 },
  { 0x13be, 0x0178 },
  { 0x20ac, 0x20ac },
  { 0xfe50,    '`' },
  { 0xfe51, 0x00b4 },
  { 0xfe52,    '^' },
  { 0xfe53,    '~' },
  { 0xfe54, 0x00af },
  { 0xfe55, 0x02d8 },
  { 0xfe56, 0x02d9 },
  { 0xfe57, 0x00a8 },
  { 0xfe58, 0x02da },
  { 0xfe59, 0x02dd },
  { 0xfe5a, 0x02c7 },
  { 0xfe5b, 0x00b8 },
  { 0xfe5c, 0x02db },
  { 0xfe5d, 0x037a },
  { 0xfe5e, 0x309b },
  { 0xfe5f, 0x309c },
  { 0xfe63,    '/' },
  { 0xfe64, 0x02bc },
  { 0xfe65, 0x02bd },
  { 0xfe66, 0x02f5 },
  { 0xfe67, 0x02f3 },
  { 0xfe68, 0x02cd },
  { 0xfe69, 0xa788 },
  { 0xfe6a, 0x02f7 },
  { 0xfe6e,    ',' },
  { 0xfe6f, 0x00a4 },
  { 0xfe80,    'a' }, /* XK_dead_a */
  { 0xfe81,    'A' }, /* XK_dead_A */
  { 0xfe82,    'e' }, /* XK_dead_e */
  { 0xfe83,    'E' }, /* XK_dead_E */
  { 0xfe84,    'i' }, /* XK_dead_i */
  { 0xfe85,    'I' }, /* XK_dead_I */
  { 0xfe86,    'o' }, /* XK_dead_o */
  { 0xfe87,    'O' }, /* XK_dead_O */
  { 0xfe88,    'u' }, /* XK_dead_u */
  { 0xfe89,    'U' }, /* XK_dead_U */
  { 0xfe8a, 0x0259 },
  { 0xfe8b, 0x018f },
  { 0xfe8c, 0x00b5 },
  { 0xfe90,    '_' },
  { 0xfe91, 0x02c8 },
  { 0xfe92, 0x02cc },
  { 0xff80 /*XKB_KEY_KP_Space*/,     ' ' },
  { 0xff95 /*XKB_KEY_KP_7*/, 0x0037 },
  { 0xff96 /*XKB_KEY_KP_4*/, 0x0034 },
  { 0xff97 /*XKB_KEY_KP_8*/, 0x0038 },
  { 0xff98 /*XKB_KEY_KP_6*/, 0x0036 },
  { 0xff99 /*XKB_KEY_KP_2*/, 0x0032 },
  { 0xff9a /*XKB_KEY_KP_9*/, 0x0039 },
  { 0xff9b /*XKB_KEY_KP_3*/, 0x0033 },
  { 0xff9c /*XKB_KEY_KP_1*/, 0x0031 },
  { 0xff9d /*XKB_KEY_KP_5*/, 0x0035 },
  { 0xff9e /*XKB_KEY_KP_0*/, 0x0030 },
  { 0xffaa /*XKB_KEY_KP_Multiply*/,  '*' },
  { 0xffab /*XKB_KEY_KP_Add*/,       '+' },
  { 0xffac /*XKB_KEY_KP_Separator*/, ',' },
  { 0xffad /*XKB_KEY_KP_Subtract*/,  '-' },
  { 0xffae /*XKB_KEY_KP_Decimal*/,   '.' },
  { 0xffaf /*XKB_KEY_KP_Divide*/,    '/' },
  { 0xffb0 /*XKB_KEY_KP_0*/, 0x0030 },
  { 0xffb1 /*XKB_KEY_KP_1*/, 0x0031 },
  { 0xffb2 /*XKB_KEY_KP_2*/, 0x0032 },
  { 0xffb3 /*XKB_KEY_KP_3*/, 0x0033 },
  { 0xffb4 /*XKB_KEY_KP_4*/, 0x0034 },
  { 0xffb5 /*XKB_KEY_KP_5*/, 0x0035 },
  { 0xffb6 /*XKB_KEY_KP_6*/, 0x0036 },
  { 0xffb7 /*XKB_KEY_KP_7*/, 0x0037 },
  { 0xffb8 /*XKB_KEY_KP_8*/, 0x0038 },
  { 0xffb9 /*XKB_KEY_KP_9*/, 0x0039 },
  { 0xffbd /*XKB_KEY_KP_Equal*/,     '=' }
};

_SOKOL_PRIVATE int _sapp_x11_error_handler(Display* display, XErrorEvent* event) {
    _SOKOL_UNUSED(display);
    _sapp.x11.error_code = event->error_code;
    return 0;
}

_SOKOL_PRIVATE void _sapp_x11_grab_error_handler(void) {
    _sapp.x11.error_code = Success;
    XSetErrorHandler(_sapp_x11_error_handler);
}

_SOKOL_PRIVATE void _sapp_x11_release_error_handler(void) {
    XSync(_sapp.x11.display, False);
    XSetErrorHandler(NULL);
}

_SOKOL_PRIVATE void _sapp_x11_init_extensions(void) {
    _sapp.x11.UTF8_STRING             = XInternAtom(_sapp.x11.display, "UTF8_STRING", False);
    _sapp.x11.WM_PROTOCOLS            = XInternAtom(_sapp.x11.display, "WM_PROTOCOLS", False);
    _sapp.x11.WM_DELETE_WINDOW        = XInternAtom(_sapp.x11.display, "WM_DELETE_WINDOW", False);
    _sapp.x11.WM_STATE                = XInternAtom(_sapp.x11.display, "WM_STATE", False);
    _sapp.x11.NET_WM_NAME             = XInternAtom(_sapp.x11.display, "_NET_WM_NAME", False);
    _sapp.x11.NET_WM_ICON_NAME        = XInternAtom(_sapp.x11.display, "_NET_WM_ICON_NAME", False);
    _sapp.x11.NET_WM_ICON             = XInternAtom(_sapp.x11.display, "_NET_WM_ICON", False);
    _sapp.x11.NET_WM_STATE            = XInternAtom(_sapp.x11.display, "_NET_WM_STATE", False);
    _sapp.x11.NET_WM_STATE_FULLSCREEN = XInternAtom(_sapp.x11.display, "_NET_WM_STATE_FULLSCREEN", False);
    if (_sapp.drop.enabled) {
        _sapp.x11.xdnd.XdndAware        = XInternAtom(_sapp.x11.display, "XdndAware", False);
        _sapp.x11.xdnd.XdndEnter        = XInternAtom(_sapp.x11.display, "XdndEnter", False);
        _sapp.x11.xdnd.XdndPosition     = XInternAtom(_sapp.x11.display, "XdndPosition", False);
        _sapp.x11.xdnd.XdndStatus       = XInternAtom(_sapp.x11.display, "XdndStatus", False);
        _sapp.x11.xdnd.XdndActionCopy   = XInternAtom(_sapp.x11.display, "XdndActionCopy", False);
        _sapp.x11.xdnd.XdndDrop         = XInternAtom(_sapp.x11.display, "XdndDrop", False);
        _sapp.x11.xdnd.XdndFinished     = XInternAtom(_sapp.x11.display, "XdndFinished", False);
        _sapp.x11.xdnd.XdndSelection    = XInternAtom(_sapp.x11.display, "XdndSelection", False);
        _sapp.x11.xdnd.XdndTypeList     = XInternAtom(_sapp.x11.display, "XdndTypeList", False);
        _sapp.x11.xdnd.text_uri_list    = XInternAtom(_sapp.x11.display, "text/uri-list", False);
    }

    /* check Xi extension for raw mouse input */
    if (XQueryExtension(_sapp.x11.display, "XInputExtension", &_sapp.x11.xi.major_opcode, &_sapp.x11.xi.event_base, &_sapp.x11.xi.error_base)) {
        _sapp.x11.xi.major = 2;
        _sapp.x11.xi.minor = 0;
        if (XIQueryVersion(_sapp.x11.display, &_sapp.x11.xi.major, &_sapp.x11.xi.minor) == Success) {
            _sapp.x11.xi.available = true;
        }
    }
}

_SOKOL_PRIVATE void _sapp_x11_query_system_dpi(void) {
    /* from GLFW:

       NOTE: Default to the display-wide DPI as we don't currently have a policy
             for which monitor a window is considered to be on

        _sapp.x11.dpi = DisplayWidth(_sapp.x11.display, _sapp.x11.screen) *
                        25.4f / DisplayWidthMM(_sapp.x11.display, _sapp.x11.screen);

       NOTE: Basing the scale on Xft.dpi where available should provide the most
             consistent user experience (matches Qt, Gtk, etc), although not
             always the most accurate one
    */
    bool dpi_ok = false;
    char* rms = XResourceManagerString(_sapp.x11.display);
    if (rms) {
        XrmDatabase db = XrmGetStringDatabase(rms);
        if (db) {
            XrmValue value;
            char* type = NULL;
            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value)) {
                if (type && strcmp(type, "String") == 0) {
                    _sapp.x11.dpi = atof(value.addr);
                    dpi_ok = true;
                }
            }
            XrmDestroyDatabase(db);
        }
    }
    // fallback if querying DPI had failed: assume the standard DPI 96.0f
    if (!dpi_ok) {
        _sapp.x11.dpi = 96.0f;
        _SAPP_WARN(LINUX_X11_QUERY_SYSTEM_DPI_FAILED);
    }
}

#if defined(_SAPP_GLX)

_SOKOL_PRIVATE bool _sapp_glx_has_ext(const char* ext, const char* extensions) {
    SOKOL_ASSERT(ext);
    const char* start = extensions;
    while (true) {
        const char* where = strstr(start, ext);
        if (!where) {
            return false;
        }
        const char* terminator = where + strlen(ext);
        if ((where == start) || (*(where - 1) == ' ')) {
            if (*terminator == ' ' || *terminator == '\0') {
                break;
            }
        }
        start = terminator;
    }
    return true;
}

_SOKOL_PRIVATE bool _sapp_glx_extsupported(const char* ext, const char* extensions) {
    if (extensions) {
        return _sapp_glx_has_ext(ext, extensions);
    }
    else {
        return false;
    }
}

_SOKOL_PRIVATE void* _sapp_glx_getprocaddr(const char* procname)
{
    if (_sapp.glx.GetProcAddress) {
        return (void*) _sapp.glx.GetProcAddress(procname);
    }
    else if (_sapp.glx.GetProcAddressARB) {
        return (void*) _sapp.glx.GetProcAddressARB(procname);
    }
    else {
        return dlsym(_sapp.glx.libgl, procname);
    }
}

_SOKOL_PRIVATE void _sapp_glx_init(void) {
    const char* sonames[] = { "libGL.so.1", "libGL.so", 0 };
    for (int i = 0; sonames[i]; i++) {
        _sapp.glx.libgl = dlopen(sonames[i], RTLD_LAZY|RTLD_GLOBAL);
        if (_sapp.glx.libgl) {
            break;
        }
    }
    if (!_sapp.glx.libgl) {
        _SAPP_PANIC(LINUX_GLX_LOAD_LIBGL_FAILED);
    }
    _sapp.glx.GetFBConfigs          = (PFNGLXGETFBCONFIGSPROC)          dlsym(_sapp.glx.libgl, "glXGetFBConfigs");
    _sapp.glx.GetFBConfigAttrib     = (PFNGLXGETFBCONFIGATTRIBPROC)     dlsym(_sapp.glx.libgl, "glXGetFBConfigAttrib");
    _sapp.glx.GetClientString       = (PFNGLXGETCLIENTSTRINGPROC)       dlsym(_sapp.glx.libgl, "glXGetClientString");
    _sapp.glx.QueryExtension        = (PFNGLXQUERYEXTENSIONPROC)        dlsym(_sapp.glx.libgl, "glXQueryExtension");
    _sapp.glx.QueryVersion          = (PFNGLXQUERYVERSIONPROC)          dlsym(_sapp.glx.libgl, "glXQueryVersion");
    _sapp.glx.DestroyContext        = (PFNGLXDESTROYCONTEXTPROC)        dlsym(_sapp.glx.libgl, "glXDestroyContext");
    _sapp.glx.MakeCurrent           = (PFNGLXMAKECURRENTPROC)           dlsym(_sapp.glx.libgl, "glXMakeCurrent");
    _sapp.glx.SwapBuffers           = (PFNGLXSWAPBUFFERSPROC)           dlsym(_sapp.glx.libgl, "glXSwapBuffers");
    _sapp.glx.QueryExtensionsString = (PFNGLXQUERYEXTENSIONSSTRINGPROC) dlsym(_sapp.glx.libgl, "glXQueryExtensionsString");
    _sapp.glx.CreateWindow          = (PFNGLXCREATEWINDOWPROC)          dlsym(_sapp.glx.libgl, "glXCreateWindow");
    _sapp.glx.DestroyWindow         = (PFNGLXDESTROYWINDOWPROC)         dlsym(_sapp.glx.libgl, "glXDestroyWindow");
    _sapp.glx.GetProcAddress        = (PFNGLXGETPROCADDRESSPROC)        dlsym(_sapp.glx.libgl, "glXGetProcAddress");
    _sapp.glx.GetProcAddressARB     = (PFNGLXGETPROCADDRESSPROC)        dlsym(_sapp.glx.libgl, "glXGetProcAddressARB");
    _sapp.glx.GetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC) dlsym(_sapp.glx.libgl, "glXGetVisualFromFBConfig");
    if (!_sapp.glx.GetFBConfigs ||
        !_sapp.glx.GetFBConfigAttrib ||
        !_sapp.glx.GetClientString ||
        !_sapp.glx.QueryExtension ||
        !_sapp.glx.QueryVersion ||
        !_sapp.glx.DestroyContext ||
        !_sapp.glx.MakeCurrent ||
        !_sapp.glx.SwapBuffers ||
        !_sapp.glx.QueryExtensionsString ||
        !_sapp.glx.CreateWindow ||
        !_sapp.glx.DestroyWindow ||
        !_sapp.glx.GetProcAddress ||
        !_sapp.glx.GetProcAddressARB ||
        !_sapp.glx.GetVisualFromFBConfig)
    {
        _SAPP_PANIC(LINUX_GLX_LOAD_ENTRY_POINTS_FAILED);
    }

    if (!_sapp.glx.QueryExtension(_sapp.x11.display, &_sapp.glx.error_base, &_sapp.glx.event_base)) {
        _SAPP_PANIC(LINUX_GLX_EXTENSION_NOT_FOUND);
    }
    if (!_sapp.glx.QueryVersion(_sapp.x11.display, &_sapp.glx.major, &_sapp.glx.minor)) {
        _SAPP_PANIC(LINUX_GLX_QUERY_VERSION_FAILED);
    }
    if (_sapp.glx.major == 1 && _sapp.glx.minor < 3) {
        _SAPP_PANIC(LINUX_GLX_VERSION_TOO_LOW);
    }
    const char* exts = _sapp.glx.QueryExtensionsString(_sapp.x11.display, _sapp.x11.screen);
    if (_sapp_glx_extsupported("GLX_EXT_swap_control", exts)) {
        _sapp.glx.SwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) _sapp_glx_getprocaddr("glXSwapIntervalEXT");
        _sapp.glx.EXT_swap_control = 0 != _sapp.glx.SwapIntervalEXT;
    }
    if (_sapp_glx_extsupported("GLX_MESA_swap_control", exts)) {
        _sapp.glx.SwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC) _sapp_glx_getprocaddr("glXSwapIntervalMESA");
        _sapp.glx.MESA_swap_control = 0 != _sapp.glx.SwapIntervalMESA;
    }
    _sapp.glx.ARB_multisample = _sapp_glx_extsupported("GLX_ARB_multisample", exts);
    if (_sapp_glx_extsupported("GLX_ARB_create_context", exts)) {
        _sapp.glx.CreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) _sapp_glx_getprocaddr("glXCreateContextAttribsARB");
        _sapp.glx.ARB_create_context = 0 != _sapp.glx.CreateContextAttribsARB;
    }
    _sapp.glx.ARB_create_context_profile = _sapp_glx_extsupported("GLX_ARB_create_context_profile", exts);
}

_SOKOL_PRIVATE int _sapp_glx_attrib(GLXFBConfig fbconfig, int attrib) {
    int value;
    _sapp.glx.GetFBConfigAttrib(_sapp.x11.display, fbconfig, attrib, &value);
    return value;
}

_SOKOL_PRIVATE GLXFBConfig _sapp_glx_choosefbconfig(void) {
    GLXFBConfig* native_configs;
    _sapp_gl_fbconfig* usable_configs;
    const _sapp_gl_fbconfig* closest;
    int i, native_count, usable_count;
    const char* vendor;
    bool trust_window_bit = true;

    /* HACK: This is a (hopefully temporary) workaround for Chromium
           (VirtualBox GL) not setting the window bit on any GLXFBConfigs
    */
    vendor = _sapp.glx.GetClientString(_sapp.x11.display, GLX_VENDOR);
    if (vendor && strcmp(vendor, "Chromium") == 0) {
        trust_window_bit = false;
    }

    native_configs = _sapp.glx.GetFBConfigs(_sapp.x11.display, _sapp.x11.screen, &native_count);
    if (!native_configs || !native_count) {
        _SAPP_PANIC(LINUX_GLX_NO_GLXFBCONFIGS);
    }

    usable_configs = (_sapp_gl_fbconfig*) _sapp_malloc_clear((size_t)native_count * sizeof(_sapp_gl_fbconfig));
    usable_count = 0;
    for (i = 0;  i < native_count;  i++) {
        const GLXFBConfig n = native_configs[i];
        _sapp_gl_fbconfig* u = usable_configs + usable_count;
        _sapp_gl_init_fbconfig(u);

        /* Only consider RGBA GLXFBConfigs */
        if (0 == (_sapp_glx_attrib(n, GLX_RENDER_TYPE) & GLX_RGBA_BIT)) {
            continue;
        }
        /* Only consider window GLXFBConfigs */
        if (0 == (_sapp_glx_attrib(n, GLX_DRAWABLE_TYPE) & GLX_WINDOW_BIT)) {
            if (trust_window_bit) {
                continue;
            }
        }
        u->red_bits = _sapp_glx_attrib(n, GLX_RED_SIZE);
        u->green_bits = _sapp_glx_attrib(n, GLX_GREEN_SIZE);
        u->blue_bits = _sapp_glx_attrib(n, GLX_BLUE_SIZE);
        u->alpha_bits = _sapp_glx_attrib(n, GLX_ALPHA_SIZE);
        u->depth_bits = _sapp_glx_attrib(n, GLX_DEPTH_SIZE);
        u->stencil_bits = _sapp_glx_attrib(n, GLX_STENCIL_SIZE);
        if (_sapp_glx_attrib(n, GLX_DOUBLEBUFFER)) {
            u->doublebuffer = true;
        }
        if (_sapp.glx.ARB_multisample) {
            u->samples = _sapp_glx_attrib(n, GLX_SAMPLES);
        }
        u->handle = (uintptr_t) n;
        usable_count++;
    }
    _sapp_gl_fbconfig desired;
    _sapp_gl_init_fbconfig(&desired);
    desired.red_bits = 8;
    desired.green_bits = 8;
    desired.blue_bits = 8;
    desired.alpha_bits = 8;
    desired.depth_bits = 24;
    desired.stencil_bits = 8;
    desired.doublebuffer = true;
    desired.samples = _sapp.sample_count > 1 ? _sapp.sample_count : 0;
    closest = _sapp_gl_choose_fbconfig(&desired, usable_configs, usable_count);
    GLXFBConfig result = 0;
    if (closest) {
        result = (GLXFBConfig) closest->handle;
    }
    XFree(native_configs);
    _sapp_free(usable_configs);
    return result;
}

_SOKOL_PRIVATE void _sapp_glx_choose_visual(Visual** visual, int* depth) {
    GLXFBConfig native = _sapp_glx_choosefbconfig();
    if (0 == native) {
        _SAPP_PANIC(LINUX_GLX_NO_SUITABLE_GLXFBCONFIG);
    }
    XVisualInfo* result = _sapp.glx.GetVisualFromFBConfig(_sapp.x11.display, native);
    if (!result) {
        _SAPP_PANIC(LINUX_GLX_GET_VISUAL_FROM_FBCONFIG_FAILED);
    }
    *visual = result->visual;
    *depth = result->depth;
    XFree(result);
}

_SOKOL_PRIVATE void _sapp_glx_make_current(void) {
    _sapp.glx.MakeCurrent(_sapp.x11.display, _sapp.glx.window, _sapp.glx.ctx);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_sapp.gl.framebuffer);
}

_SOKOL_PRIVATE void _sapp_glx_create_context(void) {
    GLXFBConfig native = _sapp_glx_choosefbconfig();
    if (0 == native){
        _SAPP_PANIC(LINUX_GLX_NO_SUITABLE_GLXFBCONFIG);
    }
    if (!(_sapp.glx.ARB_create_context && _sapp.glx.ARB_create_context_profile)) {
        _SAPP_PANIC(LINUX_GLX_REQUIRED_EXTENSIONS_MISSING);
    }
    _sapp_x11_grab_error_handler();
    const int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, _sapp.desc.gl_major_version,
        GLX_CONTEXT_MINOR_VERSION_ARB, _sapp.desc.gl_minor_version,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0, 0
    };
    _sapp.glx.ctx = _sapp.glx.CreateContextAttribsARB(_sapp.x11.display, native, NULL, True, attribs);
    if (!_sapp.glx.ctx) {
        _SAPP_PANIC(LINUX_GLX_CREATE_CONTEXT_FAILED);
    }
    _sapp_x11_release_error_handler();
    _sapp.glx.window = _sapp.glx.CreateWindow(_sapp.x11.display, native, _sapp.x11.window, NULL);
    if (!_sapp.glx.window) {
        _SAPP_PANIC(LINUX_GLX_CREATE_WINDOW_FAILED);
    }
    _sapp_glx_make_current();
}

_SOKOL_PRIVATE void _sapp_glx_destroy_context(void) {
    if (_sapp.glx.window) {
        _sapp.glx.DestroyWindow(_sapp.x11.display, _sapp.glx.window);
        _sapp.glx.window = 0;
    }
    if (_sapp.glx.ctx) {
        _sapp.glx.DestroyContext(_sapp.x11.display, _sapp.glx.ctx);
        _sapp.glx.ctx = 0;
    }
}

_SOKOL_PRIVATE void _sapp_glx_swap_buffers(void) {
    _sapp.glx.SwapBuffers(_sapp.x11.display, _sapp.glx.window);
}

_SOKOL_PRIVATE void _sapp_glx_swapinterval(int interval) {
    if (_sapp.glx.EXT_swap_control) {
        _sapp.glx.SwapIntervalEXT(_sapp.x11.display, _sapp.glx.window, interval);
    }
    else if (_sapp.glx.MESA_swap_control) {
        _sapp.glx.SwapIntervalMESA(interval);
    }
}

#endif /* _SAPP_GLX */

_SOKOL_PRIVATE void _sapp_x11_send_event(Atom type, int a, int b, int c, int d, int e) {
    XEvent event;
    _sapp_clear(&event, sizeof(event));

    event.type = ClientMessage;
    event.xclient.window = _sapp.x11.window;
    event.xclient.format = 32;
    event.xclient.message_type = type;
    event.xclient.data.l[0] = a;
    event.xclient.data.l[1] = b;
    event.xclient.data.l[2] = c;
    event.xclient.data.l[3] = d;
    event.xclient.data.l[4] = e;

    XSendEvent(_sapp.x11.display, _sapp.x11.root,
               False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               &event);
}

_SOKOL_PRIVATE void _sapp_x11_query_window_size(void) {
    XWindowAttributes attribs;
    XGetWindowAttributes(_sapp.x11.display, _sapp.x11.window, &attribs);
    _sapp.window_width = attribs.width;
    _sapp.window_height = attribs.height;
    _sapp.framebuffer_width = _sapp.window_width;
    _sapp.framebuffer_height = _sapp.window_height;
}

_SOKOL_PRIVATE void _sapp_x11_set_fullscreen(bool enable) {
    /* NOTE: this function must be called after XMapWindow (which happens in _sapp_x11_show_window()) */
    if (_sapp.x11.NET_WM_STATE && _sapp.x11.NET_WM_STATE_FULLSCREEN) {
        if (enable) {
            const int _NET_WM_STATE_ADD = 1;
            _sapp_x11_send_event(_sapp.x11.NET_WM_STATE,
                                _NET_WM_STATE_ADD,
                                _sapp.x11.NET_WM_STATE_FULLSCREEN,
                                0, 1, 0);
        }
        else {
            const int _NET_WM_STATE_REMOVE = 0;
            _sapp_x11_send_event(_sapp.x11.NET_WM_STATE,
                                _NET_WM_STATE_REMOVE,
                                _sapp.x11.NET_WM_STATE_FULLSCREEN,
                                0, 1, 0);
        }
    }
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE void _sapp_x11_create_hidden_cursor(void) {
    SOKOL_ASSERT(0 == _sapp.x11.hidden_cursor);
    const int w = 16;
    const int h = 16;
    XcursorImage* img = XcursorImageCreate(w, h);
    SOKOL_ASSERT(img && (img->width == 16) && (img->height == 16) && img->pixels);
    img->xhot = 0;
    img->yhot = 0;
    const size_t num_bytes = (size_t)(w * h) * sizeof(XcursorPixel);
    _sapp_clear(img->pixels, num_bytes);
    _sapp.x11.hidden_cursor = XcursorImageLoadCursor(_sapp.x11.display, img);
    XcursorImageDestroy(img);
}

 _SOKOL_PRIVATE void _sapp_x11_create_standard_cursor(sapp_mouse_cursor cursor, const char* name, const char* theme, int size, uint32_t fallback_native) {
    SOKOL_ASSERT((cursor >= 0) && (cursor < _SAPP_MOUSECURSOR_NUM));
    SOKOL_ASSERT(_sapp.x11.display);
    if (theme) {
        XcursorImage* img = XcursorLibraryLoadImage(name, theme, size);
        if (img) {
            _sapp.x11.cursors[cursor] = XcursorImageLoadCursor(_sapp.x11.display, img);
            XcursorImageDestroy(img);
        }
    }
    if (0 == _sapp.x11.cursors[cursor]) {
        _sapp.x11.cursors[cursor] = XCreateFontCursor(_sapp.x11.display, fallback_native);
    }
}

_SOKOL_PRIVATE void _sapp_x11_create_cursors(void) {
    SOKOL_ASSERT(_sapp.x11.display);
    const char* cursor_theme = XcursorGetTheme(_sapp.x11.display);
    const int size = XcursorGetDefaultSize(_sapp.x11.display);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_ARROW, "default", cursor_theme, size, XC_left_ptr);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_IBEAM, "text", cursor_theme, size, XC_xterm);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_CROSSHAIR, "crosshair", cursor_theme, size, XC_crosshair);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_POINTING_HAND, "pointer", cursor_theme, size, XC_hand2);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_RESIZE_EW, "ew-resize", cursor_theme, size, XC_sb_h_double_arrow);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_RESIZE_NS, "ns-resize", cursor_theme, size, XC_sb_v_double_arrow);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_RESIZE_NWSE, "nwse-resize", cursor_theme, size, 0);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_RESIZE_NESW, "nesw-resize", cursor_theme, size, 0);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_RESIZE_ALL, "all-scroll", cursor_theme, size, XC_fleur);
    _sapp_x11_create_standard_cursor(SAPP_MOUSECURSOR_NOT_ALLOWED, "no-allowed", cursor_theme, size, 0);
    _sapp_x11_create_hidden_cursor();
}

_SOKOL_PRIVATE void _sapp_x11_destroy_cursors(void) {
    SOKOL_ASSERT(_sapp.x11.display);
    if (_sapp.x11.hidden_cursor) {
        XFreeCursor(_sapp.x11.display, _sapp.x11.hidden_cursor);
        _sapp.x11.hidden_cursor = 0;
    }
    for (int i = 0; i < _SAPP_MOUSECURSOR_NUM; i++) {
        if (_sapp.x11.cursors[i]) {
            XFreeCursor(_sapp.x11.display, _sapp.x11.cursors[i]);
            _sapp.x11.cursors[i] = 0;
        }
    }
}

_SOKOL_PRIVATE void _sapp_x11_toggle_fullscreen(void) {
    _sapp.fullscreen = !_sapp.fullscreen;
    _sapp_x11_set_fullscreen(_sapp.fullscreen);
    _sapp_x11_query_window_size();
}

_SOKOL_PRIVATE void _sapp_x11_update_cursor(sapp_mouse_cursor cursor, bool shown) {
    SOKOL_ASSERT((cursor >= 0) && (cursor < _SAPP_MOUSECURSOR_NUM));
    if (shown) {
        if (_sapp.x11.cursors[cursor]) {
            XDefineCursor(_sapp.x11.display, _sapp.x11.window, _sapp.x11.cursors[cursor]);
        }
        else {
            XUndefineCursor(_sapp.x11.display, _sapp.x11.window);
        }
    }
    else {
        XDefineCursor(_sapp.x11.display, _sapp.x11.window, _sapp.x11.hidden_cursor);
    }
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE void _sapp_x11_lock_mouse(bool lock) {
    if (lock == _sapp.mouse.locked) {
        return;
    }
    _sapp.mouse.dx = 0.0f;
    _sapp.mouse.dy = 0.0f;
    _sapp.mouse.locked = lock;
    if (_sapp.mouse.locked) {
        if (_sapp.x11.xi.available) {
            XIEventMask em;
            unsigned char mask[XIMaskLen(XI_RawMotion)] = { 0 }; // XIMaskLen is a macro
            em.deviceid = XIAllMasterDevices;
            em.mask_len = sizeof(mask);
            em.mask = mask;
            XISetMask(mask, XI_RawMotion);
            XISelectEvents(_sapp.x11.display, _sapp.x11.root, &em, 1);
        }
        XGrabPointer(_sapp.x11.display, // display
            _sapp.x11.window,           // grab_window
            True,                       // owner_events
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask,    // event_mask
            GrabModeAsync,              // pointer_mode
            GrabModeAsync,              // keyboard_mode
            _sapp.x11.window,           // confine_to
            _sapp.x11.hidden_cursor,    // cursor
            CurrentTime);               // time
    }
    else {
        if (_sapp.x11.xi.available) {
            XIEventMask em;
            unsigned char mask[] = { 0 };
            em.deviceid = XIAllMasterDevices;
            em.mask_len = sizeof(mask);
            em.mask = mask;
            XISelectEvents(_sapp.x11.display, _sapp.x11.root, &em, 1);
        }
        XWarpPointer(_sapp.x11.display, None, _sapp.x11.window, 0, 0, 0, 0, (int) _sapp.mouse.x, _sapp.mouse.y);
        XUngrabPointer(_sapp.x11.display, CurrentTime);
    }
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE void _sapp_x11_update_window_title(void) {
    Xutf8SetWMProperties(_sapp.x11.display,
        _sapp.x11.window,
        _sapp.window_title, _sapp.window_title,
        NULL, 0, NULL, NULL, NULL);
    XChangeProperty(_sapp.x11.display, _sapp.x11.window,
        _sapp.x11.NET_WM_NAME, _sapp.x11.UTF8_STRING, 8,
        PropModeReplace,
        (unsigned char*)_sapp.window_title,
        strlen(_sapp.window_title));
    XChangeProperty(_sapp.x11.display, _sapp.x11.window,
        _sapp.x11.NET_WM_ICON_NAME, _sapp.x11.UTF8_STRING, 8,
        PropModeReplace,
        (unsigned char*)_sapp.window_title,
        strlen(_sapp.window_title));
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE void _sapp_x11_set_icon(const sapp_icon_desc* icon_desc, int num_images) {
    SOKOL_ASSERT((num_images > 0) && (num_images <= SAPP_MAX_ICONIMAGES));
    int long_count = 0;
    for (int i = 0; i < num_images; i++) {
        const sapp_image_desc* img_desc = &icon_desc->images[i];
        long_count += 2 + (img_desc->width * img_desc->height);
    }
    long* icon_data = (long*) _sapp_malloc_clear((size_t)long_count * sizeof(long));
    SOKOL_ASSERT(icon_data);
    long* dst = icon_data;
    for (int img_index = 0; img_index < num_images; img_index++) {
        const sapp_image_desc* img_desc = &icon_desc->images[img_index];
        const uint8_t* src = (const uint8_t*) img_desc->pixels.ptr;
        *dst++ = img_desc->width;
        *dst++ = img_desc->height;
        const int num_pixels = img_desc->width * img_desc->height;
        for (int pixel_index = 0; pixel_index < num_pixels; pixel_index++) {
            *dst++ = ((long)(src[pixel_index * 4 + 0]) << 16) |
                     ((long)(src[pixel_index * 4 + 1]) << 8) |
                     ((long)(src[pixel_index * 4 + 2]) << 0) |
                     ((long)(src[pixel_index * 4 + 3]) << 24);
        }
    }
    XChangeProperty(_sapp.x11.display, _sapp.x11.window,
        _sapp.x11.NET_WM_ICON,
        XA_CARDINAL, 32,
        PropModeReplace,
        (unsigned char*)icon_data,
        long_count);
    _sapp_free(icon_data);
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE void _sapp_x11_create_window(Visual* visual, int depth) {
    _sapp.x11.colormap = XCreateColormap(_sapp.x11.display, _sapp.x11.root, visual, AllocNone);
    XSetWindowAttributes wa;
    _sapp_clear(&wa, sizeof(wa));
    const uint32_t wamask = CWBorderPixel | CWColormap | CWEventMask;
    wa.colormap = _sapp.x11.colormap;
    wa.border_pixel = 0;
    wa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                    PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                    ExposureMask | FocusChangeMask | VisibilityChangeMask |
                    EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

    int display_width = DisplayWidth(_sapp.x11.display, _sapp.x11.screen);
    int display_height = DisplayHeight(_sapp.x11.display, _sapp.x11.screen);
    int window_width = _sapp.window_width;
    int window_height = _sapp.window_height;
    if (0 == window_width) {
        window_width = (display_width * 4) / 5;
    }
    if (0 == window_height) {
        window_height = (display_height * 4) / 5;
    }
    int window_xpos = (display_width - window_width) / 2;
    int window_ypos = (display_height - window_height) / 2;
    if (window_xpos < 0) {
        window_xpos = 0;
    }
    if (window_ypos < 0) {
        window_ypos = 0;
    }
    _sapp_x11_grab_error_handler();
    _sapp.x11.window = XCreateWindow(_sapp.x11.display,
                                     _sapp.x11.root,
                                     window_xpos,
                                     window_ypos,
                                     (uint32_t)window_width,
                                     (uint32_t)window_height,
                                     0,     /* border width */
                                     depth, /* color depth */
                                     InputOutput,
                                     visual,
                                     wamask,
                                     &wa);
    _sapp_x11_release_error_handler();
    if (!_sapp.x11.window) {
        _SAPP_PANIC(LINUX_X11_CREATE_WINDOW_FAILED);
    }
    Atom protocols[] = {
        _sapp.x11.WM_DELETE_WINDOW
    };
    XSetWMProtocols(_sapp.x11.display, _sapp.x11.window, protocols, 1);

    XSizeHints* hints = XAllocSizeHints();
    hints->flags = (PWinGravity | PPosition | PSize);
    hints->win_gravity = StaticGravity;
    hints->x = window_xpos;
    hints->y = window_ypos;
    hints->width = window_width;
    hints->height = window_height;
    XSetWMNormalHints(_sapp.x11.display, _sapp.x11.window, hints);
    XFree(hints);

    /* announce support for drag'n'drop */
    if (_sapp.drop.enabled) {
        const Atom version = _SAPP_X11_XDND_VERSION;
        XChangeProperty(_sapp.x11.display, _sapp.x11.window, _sapp.x11.xdnd.XdndAware, XA_ATOM, 32, PropModeReplace, (unsigned char*) &version, 1);
    }
    _sapp_x11_update_window_title();
    _sapp_x11_query_window_size();
}

_SOKOL_PRIVATE void _sapp_x11_destroy_window(void) {
    if (_sapp.x11.window) {
        XUnmapWindow(_sapp.x11.display, _sapp.x11.window);
        XDestroyWindow(_sapp.x11.display, _sapp.x11.window);
        _sapp.x11.window = 0;
    }
    if (_sapp.x11.colormap) {
        XFreeColormap(_sapp.x11.display, _sapp.x11.colormap);
        _sapp.x11.colormap = 0;
    }
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE bool _sapp_x11_window_visible(void) {
    XWindowAttributes wa;
    XGetWindowAttributes(_sapp.x11.display, _sapp.x11.window, &wa);
    return wa.map_state == IsViewable;
}

_SOKOL_PRIVATE void _sapp_x11_show_window(void) {
    if (!_sapp_x11_window_visible()) {
        XMapWindow(_sapp.x11.display, _sapp.x11.window);
        XRaiseWindow(_sapp.x11.display, _sapp.x11.window);
        XFlush(_sapp.x11.display);
    }
}

_SOKOL_PRIVATE void _sapp_x11_hide_window(void) {
    XUnmapWindow(_sapp.x11.display, _sapp.x11.window);
    XFlush(_sapp.x11.display);
}

_SOKOL_PRIVATE unsigned long _sapp_x11_get_window_property(Window window, Atom property, Atom type, unsigned char** value) {
    Atom actualType;
    int actualFormat;
    unsigned long itemCount, bytesAfter;
    XGetWindowProperty(_sapp.x11.display,
                       window,
                       property,
                       0,
                       LONG_MAX,
                       False,
                       type,
                       &actualType,
                       &actualFormat,
                       &itemCount,
                       &bytesAfter,
                       value);
    return itemCount;
}

_SOKOL_PRIVATE int _sapp_x11_get_window_state(void) {
    int result = WithdrawnState;
    struct {
        CARD32 state;
        Window icon;
    } *state = NULL;

    if (_sapp_x11_get_window_property(_sapp.x11.window, _sapp.x11.WM_STATE, _sapp.x11.WM_STATE, (unsigned char**)&state) >= 2) {
        result = (int)state->state;
    }
    if (state) {
        XFree(state);
    }
    return result;
}

_SOKOL_PRIVATE uint32_t _sapp_x11_key_modifier_bit(sapp_keycode key) {
    switch (key) {
        case SAPP_KEYCODE_LEFT_SHIFT:
        case SAPP_KEYCODE_RIGHT_SHIFT:
            return SAPP_MODIFIER_SHIFT;
        case SAPP_KEYCODE_LEFT_CONTROL:
        case SAPP_KEYCODE_RIGHT_CONTROL:
            return SAPP_MODIFIER_CTRL;
        case SAPP_KEYCODE_LEFT_ALT:
        case SAPP_KEYCODE_RIGHT_ALT:
            return SAPP_MODIFIER_ALT;
        case SAPP_KEYCODE_LEFT_SUPER:
        case SAPP_KEYCODE_RIGHT_SUPER:
            return SAPP_MODIFIER_SUPER;
        default:
            return 0;
    }
}

_SOKOL_PRIVATE uint32_t _sapp_x11_button_modifier_bit(sapp_mousebutton btn) {
    switch (btn) {
        case SAPP_MOUSEBUTTON_LEFT:     return SAPP_MODIFIER_LMB;
        case SAPP_MOUSEBUTTON_RIGHT:    return SAPP_MODIFIER_RMB;
        case SAPP_MOUSEBUTTON_MIDDLE:   return SAPP_MODIFIER_MMB;
        default: return 0;
    }
}

_SOKOL_PRIVATE uint32_t _sapp_x11_mods(uint32_t x11_mods) {
    uint32_t mods = 0;
    if (x11_mods & ShiftMask) {
        mods |= SAPP_MODIFIER_SHIFT;
    }
    if (x11_mods & ControlMask) {
        mods |= SAPP_MODIFIER_CTRL;
    }
    if (x11_mods & Mod1Mask) {
        mods |= SAPP_MODIFIER_ALT;
    }
    if (x11_mods & Mod4Mask) {
        mods |= SAPP_MODIFIER_SUPER;
    }
    if (x11_mods & Button1Mask) {
        mods |= SAPP_MODIFIER_LMB;
    }
    if (x11_mods & Button2Mask) {
        mods |= SAPP_MODIFIER_MMB;
    }
    if (x11_mods & Button3Mask) {
        mods |= SAPP_MODIFIER_RMB;
    }
    return mods;
}

_SOKOL_PRIVATE void _sapp_x11_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE sapp_mousebutton _sapp_x11_translate_button(const XEvent* event) {
    switch (event->xbutton.button) {
        case Button1: return SAPP_MOUSEBUTTON_LEFT;
        case Button2: return SAPP_MOUSEBUTTON_MIDDLE;
        case Button3: return SAPP_MOUSEBUTTON_RIGHT;
        default:      return SAPP_MOUSEBUTTON_INVALID;
    }
}

_SOKOL_PRIVATE void _sapp_x11_mouse_update(int x, int y, bool clear_dxdy) {
    if (!_sapp.mouse.locked) {
        const float new_x = (float) x;
        const float new_y = (float) y;
        if (clear_dxdy) {
            _sapp.mouse.dx = 0.0f;
            _sapp.mouse.dy = 0.0f;
        } else if (_sapp.mouse.pos_valid) {
            _sapp.mouse.dx = new_x - _sapp.mouse.x;
            _sapp.mouse.dy = new_y - _sapp.mouse.y;
        }
        _sapp.mouse.x = new_x;
        _sapp.mouse.y = new_y;
        _sapp.mouse.pos_valid = true;
    }
}

_SOKOL_PRIVATE void _sapp_x11_mouse_event(sapp_event_type type, sapp_mousebutton btn, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.mouse_button = btn;
        _sapp.event.modifiers = mods;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_x11_scroll_event(float x, float y, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
        _sapp.event.modifiers = mods;
        _sapp.event.scroll_x = x;
        _sapp.event.scroll_y = y;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_x11_key_event(sapp_event_type type, sapp_keycode key, bool repeat, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.key_code = key;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = mods;
        _sapp_call_event(&_sapp.event);
        /* check if a CLIPBOARD_PASTED event must be sent too */
        if (_sapp.clipboard.enabled &&
            (type == SAPP_EVENTTYPE_KEY_DOWN) &&
            (_sapp.event.modifiers == SAPP_MODIFIER_CTRL) &&
            (_sapp.event.key_code == SAPP_KEYCODE_V))
        {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}

_SOKOL_PRIVATE void _sapp_x11_char_event(uint32_t chr, bool repeat, uint32_t mods) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
        _sapp.event.char_code = chr;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = mods;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE sapp_keycode _sapp_x11_translate_key(int scancode) {
    int dummy;
    KeySym* keysyms = XGetKeyboardMapping(_sapp.x11.display, scancode, 1, &dummy);
    SOKOL_ASSERT(keysyms);
    KeySym keysym = keysyms[0];
    XFree(keysyms);
    switch (keysym) {
        case XK_Escape:         return SAPP_KEYCODE_ESCAPE;
        case XK_Tab:            return SAPP_KEYCODE_TAB;
        case XK_Shift_L:        return SAPP_KEYCODE_LEFT_SHIFT;
        case XK_Shift_R:        return SAPP_KEYCODE_RIGHT_SHIFT;
        case XK_Control_L:      return SAPP_KEYCODE_LEFT_CONTROL;
        case XK_Control_R:      return SAPP_KEYCODE_RIGHT_CONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return SAPP_KEYCODE_LEFT_ALT;
        case XK_Mode_switch:    /* Mapped to Alt_R on many keyboards */
        case XK_ISO_Level3_Shift: /* AltGr on at least some machines */
        case XK_Meta_R:
        case XK_Alt_R:          return SAPP_KEYCODE_RIGHT_ALT;
        case XK_Super_L:        return SAPP_KEYCODE_LEFT_SUPER;
        case XK_Super_R:        return SAPP_KEYCODE_RIGHT_SUPER;
        case XK_Menu:           return SAPP_KEYCODE_MENU;
        case XK_Num_Lock:       return SAPP_KEYCODE_NUM_LOCK;
        case XK_Caps_Lock:      return SAPP_KEYCODE_CAPS_LOCK;
        case XK_Print:          return SAPP_KEYCODE_PRINT_SCREEN;
        case XK_Scroll_Lock:    return SAPP_KEYCODE_SCROLL_LOCK;
        case XK_Pause:          return SAPP_KEYCODE_PAUSE;
        case XK_Delete:         return SAPP_KEYCODE_DELETE;
        case XK_BackSpace:      return SAPP_KEYCODE_BACKSPACE;
        case XK_Return:         return SAPP_KEYCODE_ENTER;
        case XK_Home:           return SAPP_KEYCODE_HOME;
        case XK_End:            return SAPP_KEYCODE_END;
        case XK_Page_Up:        return SAPP_KEYCODE_PAGE_UP;
        case XK_Page_Down:      return SAPP_KEYCODE_PAGE_DOWN;
        case XK_Insert:         return SAPP_KEYCODE_INSERT;
        case XK_Left:           return SAPP_KEYCODE_LEFT;
        case XK_Right:          return SAPP_KEYCODE_RIGHT;
        case XK_Down:           return SAPP_KEYCODE_DOWN;
        case XK_Up:             return SAPP_KEYCODE_UP;
        case XK_F1:             return SAPP_KEYCODE_F1;
        case XK_F2:             return SAPP_KEYCODE_F2;
        case XK_F3:             return SAPP_KEYCODE_F3;
        case XK_F4:             return SAPP_KEYCODE_F4;
        case XK_F5:             return SAPP_KEYCODE_F5;
        case XK_F6:             return SAPP_KEYCODE_F6;
        case XK_F7:             return SAPP_KEYCODE_F7;
        case XK_F8:             return SAPP_KEYCODE_F8;
        case XK_F9:             return SAPP_KEYCODE_F9;
        case XK_F10:            return SAPP_KEYCODE_F10;
        case XK_F11:            return SAPP_KEYCODE_F11;
        case XK_F12:            return SAPP_KEYCODE_F12;
        case XK_F13:            return SAPP_KEYCODE_F13;
        case XK_F14:            return SAPP_KEYCODE_F14;
        case XK_F15:            return SAPP_KEYCODE_F15;
        case XK_F16:            return SAPP_KEYCODE_F16;
        case XK_F17:            return SAPP_KEYCODE_F17;
        case XK_F18:            return SAPP_KEYCODE_F18;
        case XK_F19:            return SAPP_KEYCODE_F19;
        case XK_F20:            return SAPP_KEYCODE_F20;
        case XK_F21:            return SAPP_KEYCODE_F21;
        case XK_F22:            return SAPP_KEYCODE_F22;
        case XK_F23:            return SAPP_KEYCODE_F23;
        case XK_F24:            return SAPP_KEYCODE_F24;
        case XK_F25:            return SAPP_KEYCODE_F25;

        case XK_KP_Divide:      return SAPP_KEYCODE_KP_DIVIDE;
        case XK_KP_Multiply:    return SAPP_KEYCODE_KP_MULTIPLY;
        case XK_KP_Subtract:    return SAPP_KEYCODE_KP_SUBTRACT;
        case XK_KP_Add:         return SAPP_KEYCODE_KP_ADD;

        case XK_KP_Insert:      return SAPP_KEYCODE_KP_0;
        case XK_KP_End:         return SAPP_KEYCODE_KP_1;
        case XK_KP_Down:        return SAPP_KEYCODE_KP_2;
        case XK_KP_Page_Down:   return SAPP_KEYCODE_KP_3;
        case XK_KP_Left:        return SAPP_KEYCODE_KP_4;
        case XK_KP_Begin:       return SAPP_KEYCODE_KP_5;
        case XK_KP_Right:       return SAPP_KEYCODE_KP_6;
        case XK_KP_Home:        return SAPP_KEYCODE_KP_7;
        case XK_KP_Up:          return SAPP_KEYCODE_KP_8;
        case XK_KP_Page_Up:     return SAPP_KEYCODE_KP_9;
        case XK_KP_Delete:      return SAPP_KEYCODE_KP_DECIMAL;
        case XK_KP_Equal:       return SAPP_KEYCODE_KP_EQUAL;
        case XK_KP_Enter:       return SAPP_KEYCODE_KP_ENTER;

        case XK_a:              return SAPP_KEYCODE_A;
        case XK_b:              return SAPP_KEYCODE_B;
        case XK_c:              return SAPP_KEYCODE_C;
        case XK_d:              return SAPP_KEYCODE_D;
        case XK_e:              return SAPP_KEYCODE_E;
        case XK_f:              return SAPP_KEYCODE_F;
        case XK_g:              return SAPP_KEYCODE_G;
        case XK_h:              return SAPP_KEYCODE_H;
        case XK_i:              return SAPP_KEYCODE_I;
        case XK_j:              return SAPP_KEYCODE_J;
        case XK_k:              return SAPP_KEYCODE_K;
        case XK_l:              return SAPP_KEYCODE_L;
        case XK_m:              return SAPP_KEYCODE_M;
        case XK_n:              return SAPP_KEYCODE_N;
        case XK_o:              return SAPP_KEYCODE_O;
        case XK_p:              return SAPP_KEYCODE_P;
        case XK_q:              return SAPP_KEYCODE_Q;
        case XK_r:              return SAPP_KEYCODE_R;
        case XK_s:              return SAPP_KEYCODE_S;
        case XK_t:              return SAPP_KEYCODE_T;
        case XK_u:              return SAPP_KEYCODE_U;
        case XK_v:              return SAPP_KEYCODE_V;
        case XK_w:              return SAPP_KEYCODE_W;
        case XK_x:              return SAPP_KEYCODE_X;
        case XK_y:              return SAPP_KEYCODE_Y;
        case XK_z:              return SAPP_KEYCODE_Z;
        case XK_1:              return SAPP_KEYCODE_1;
        case XK_2:              return SAPP_KEYCODE_2;
        case XK_3:              return SAPP_KEYCODE_3;
        case XK_4:              return SAPP_KEYCODE_4;
        case XK_5:              return SAPP_KEYCODE_5;
        case XK_6:              return SAPP_KEYCODE_6;
        case XK_7:              return SAPP_KEYCODE_7;
        case XK_8:              return SAPP_KEYCODE_8;
        case XK_9:              return SAPP_KEYCODE_9;
        case XK_0:              return SAPP_KEYCODE_0;
        case XK_space:          return SAPP_KEYCODE_SPACE;
        case XK_minus:          return SAPP_KEYCODE_MINUS;
        case XK_equal:          return SAPP_KEYCODE_EQUAL;
        case XK_bracketleft:    return SAPP_KEYCODE_LEFT_BRACKET;
        case XK_bracketright:   return SAPP_KEYCODE_RIGHT_BRACKET;
        case XK_backslash:      return SAPP_KEYCODE_BACKSLASH;
        case XK_semicolon:      return SAPP_KEYCODE_SEMICOLON;
        case XK_apostrophe:     return SAPP_KEYCODE_APOSTROPHE;
        case XK_grave:          return SAPP_KEYCODE_GRAVE_ACCENT;
        case XK_comma:          return SAPP_KEYCODE_COMMA;
        case XK_period:         return SAPP_KEYCODE_PERIOD;
        case XK_slash:          return SAPP_KEYCODE_SLASH;
        case XK_less:           return SAPP_KEYCODE_WORLD_1; /* At least in some layouts... */
        default:                return SAPP_KEYCODE_INVALID;
    }
}

_SOKOL_PRIVATE int32_t _sapp_x11_keysym_to_unicode(KeySym keysym) {
    int min = 0;
    int max = sizeof(_sapp_x11_keysymtab) / sizeof(struct _sapp_x11_codepair) - 1;
    int mid;

    /* First check for Latin-1 characters (1:1 mapping) */
    if ((keysym >= 0x0020 && keysym <= 0x007e) ||
        (keysym >= 0x00a0 && keysym <= 0x00ff))
    {
        return keysym;
    }

    /* Also check for directly encoded 24-bit UCS characters */
    if ((keysym & 0xff000000) == 0x01000000) {
        return keysym & 0x00ffffff;
    }

    /* Binary search in table */
    while (max >= min) {
        mid = (min + max) / 2;
        if (_sapp_x11_keysymtab[mid].keysym < keysym) {
            min = mid + 1;
        }
        else if (_sapp_x11_keysymtab[mid].keysym > keysym) {
            max = mid - 1;
        }
        else {
            return _sapp_x11_keysymtab[mid].ucs;
        }
    }

    /* No matching Unicode value found */
    return -1;
}

_SOKOL_PRIVATE bool _sapp_x11_parse_dropped_files_list(const char* src) {
    SOKOL_ASSERT(src);
    SOKOL_ASSERT(_sapp.drop.buffer);

    _sapp_clear_drop_buffer();
    _sapp.drop.num_files = 0;

    /*
        src is (potentially percent-encoded) string made of one or multiple paths
        separated by \r\n, each path starting with 'file://'
    */
    bool err = false;
    int src_count = 0;
    char src_chr = 0;
    char* dst_ptr = _sapp.drop.buffer;
    const char* dst_end_ptr = dst_ptr + (_sapp.drop.max_path_length - 1); // room for terminating 0
    while (0 != (src_chr = *src++)) {
        src_count++;
        char dst_chr = 0;
        /* check leading 'file://' */
        if (src_count <= 7) {
            if (((src_count == 1) && (src_chr != 'f')) ||
                ((src_count == 2) && (src_chr != 'i')) ||
                ((src_count == 3) && (src_chr != 'l')) ||
                ((src_count == 4) && (src_chr != 'e')) ||
                ((src_count == 5) && (src_chr != ':')) ||
                ((src_count == 6) && (src_chr != '/')) ||
                ((src_count == 7) && (src_chr != '/')))
            {
                _SAPP_ERROR(LINUX_X11_DROPPED_FILE_URI_WRONG_SCHEME);
                err = true;
                break;
            }
        }
        else if (src_chr == '\r') {
            // skip
        }
        else if (src_chr == '\n') {
            src_count = 0;
            _sapp.drop.num_files++;
            // too many files is not an error
            if (_sapp.drop.num_files >= _sapp.drop.max_files) {
                break;
            }
            dst_ptr = _sapp.drop.buffer + _sapp.drop.num_files * _sapp.drop.max_path_length;
            dst_end_ptr = dst_ptr + (_sapp.drop.max_path_length - 1);
        }
        else if ((src_chr == '%') && src[0] && src[1]) {
            // a percent-encoded byte (most likely UTF-8 multibyte sequence)
            const char digits[3] = { src[0], src[1], 0 };
            src += 2;
            dst_chr = (char) strtol(digits, 0, 16);
        }
        else {
            dst_chr = src_chr;
        }
        if (dst_chr) {
            // dst_end_ptr already has adjustment for terminating zero
            if (dst_ptr < dst_end_ptr) {
                *dst_ptr++ = dst_chr;
            }
            else {
                _SAPP_ERROR(DROPPED_FILE_PATH_TOO_LONG);
                err = true;
                break;
            }
        }
    }
    if (err) {
        _sapp_clear_drop_buffer();
        _sapp.drop.num_files = 0;
        return false;
    }
    else {
        return true;
    }
}

// XLib manual says keycodes are in the range [8, 255] inclusive.
// https://tronche.com/gui/x/xlib/input/keyboard-encoding.html
static bool _sapp_x11_keycodes[256];

_SOKOL_PRIVATE void _sapp_x11_process_event(XEvent* event) {
    Bool filtered = XFilterEvent(event, None);
    switch (event->type) {
        case GenericEvent:
            if (_sapp.mouse.locked && _sapp.x11.xi.available) {
                if (event->xcookie.extension == _sapp.x11.xi.major_opcode) {
                    if (XGetEventData(_sapp.x11.display, &event->xcookie)) {
                        if (event->xcookie.evtype == XI_RawMotion) {
                            XIRawEvent* re = (XIRawEvent*) event->xcookie.data;
                            if (re->valuators.mask_len) {
                                const double* values = re->raw_values;
                                if (XIMaskIsSet(re->valuators.mask, 0)) {
                                    _sapp.mouse.dx = (float) *values;
                                    values++;
                                }
                                if (XIMaskIsSet(re->valuators.mask, 1)) {
                                    _sapp.mouse.dy = (float) *values;
                                }
                                _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mods(event->xmotion.state));
                            }
                        }
                        XFreeEventData(_sapp.x11.display, &event->xcookie);
                    }
                }
            }
            break;
        case FocusIn:
            // NOTE: ignoring NotifyGrab and NotifyUngrab is same behaviour as GLFW
            if ((event->xfocus.mode != NotifyGrab) && (event->xfocus.mode != NotifyUngrab)) {
                _sapp_x11_app_event(SAPP_EVENTTYPE_FOCUSED);
            }
            break;
        case FocusOut:
            /* if focus is lost for any reason, and we're in mouse locked mode, disable mouse lock */
            if (_sapp.mouse.locked) {
                _sapp_x11_lock_mouse(false);
            }
            // NOTE: ignoring NotifyGrab and NotifyUngrab is same behaviour as GLFW
            if ((event->xfocus.mode != NotifyGrab) && (event->xfocus.mode != NotifyUngrab)) {
                _sapp_x11_app_event(SAPP_EVENTTYPE_UNFOCUSED);
            }
            break;
        case KeyPress:
            {
                int keycode = (int)event->xkey.keycode;
                const sapp_keycode key = _sapp_x11_translate_key(keycode);
                bool repeat = _sapp_x11_keycodes[keycode & 0xFF];
                _sapp_x11_keycodes[keycode & 0xFF] = true;
                uint32_t mods = _sapp_x11_mods(event->xkey.state);
                // X11 doesn't set modifier bit on key down, so emulate that
                mods |= _sapp_x11_key_modifier_bit(key);
                if (key != SAPP_KEYCODE_INVALID) {
                    _sapp_x11_key_event(SAPP_EVENTTYPE_KEY_DOWN, key, repeat, mods);
                }
                KeySym keysym;
                XLookupString(&event->xkey, NULL, 0, &keysym, NULL);
                int32_t chr = _sapp_x11_keysym_to_unicode(keysym);
                if (chr > 0) {
                    _sapp_x11_char_event((uint32_t)chr, repeat, mods);
                }
            }
            break;
        case KeyRelease:
            {
                int keycode = (int)event->xkey.keycode;
                const sapp_keycode key = _sapp_x11_translate_key(keycode);
                _sapp_x11_keycodes[keycode & 0xFF] = false;
                if (key != SAPP_KEYCODE_INVALID) {
                    uint32_t mods = _sapp_x11_mods(event->xkey.state);
                    // X11 doesn't clear modifier bit on key up, so emulate that
                    mods &= ~_sapp_x11_key_modifier_bit(key);
                    _sapp_x11_key_event(SAPP_EVENTTYPE_KEY_UP, key, false, mods);
                }
            }
            break;
        case ButtonPress:
            {
                _sapp_x11_mouse_update(event->xbutton.x, event->xbutton.y, false);
                const sapp_mousebutton btn = _sapp_x11_translate_button(event);
                uint32_t mods = _sapp_x11_mods(event->xbutton.state);
                // X11 doesn't set modifier bit on button down, so emulate that
                mods |= _sapp_x11_button_modifier_bit(btn);
                if (btn != SAPP_MOUSEBUTTON_INVALID) {
                    _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, btn, mods);
                    _sapp.x11.mouse_buttons |= (1 << btn);
                }
                else {
                    /* might be a scroll event */
                    switch (event->xbutton.button) {
                        case 4: _sapp_x11_scroll_event(0.0f, 1.0f, mods); break;
                        case 5: _sapp_x11_scroll_event(0.0f, -1.0f, mods); break;
                        case 6: _sapp_x11_scroll_event(1.0f, 0.0f, mods); break;
                        case 7: _sapp_x11_scroll_event(-1.0f, 0.0f, mods); break;
                    }
                }
            }
            break;
        case ButtonRelease:
            {
                _sapp_x11_mouse_update(event->xbutton.x, event->xbutton.y, false);
                const sapp_mousebutton btn = _sapp_x11_translate_button(event);
                if (btn != SAPP_MOUSEBUTTON_INVALID) {
                    uint32_t mods = _sapp_x11_mods(event->xbutton.state);
                    // X11 doesn't clear modifier bit on button up, so emulate that
                    mods &= ~_sapp_x11_button_modifier_bit(btn);
                    _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, btn, mods);
                    _sapp.x11.mouse_buttons &= ~(1 << btn);
                }
            }
            break;
        case EnterNotify:
            /* don't send enter/leave events while mouse button held down */
            if (0 == _sapp.x11.mouse_buttons) {
                _sapp_x11_mouse_update(event->xcrossing.x, event->xcrossing.y, true);
                _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mods(event->xcrossing.state));
            }
            break;
        case LeaveNotify:
            if (0 == _sapp.x11.mouse_buttons) {
                _sapp_x11_mouse_update(event->xcrossing.x, event->xcrossing.y, true);
                _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mods(event->xcrossing.state));
            }
            break;
        case MotionNotify:
            if (!_sapp.mouse.locked) {
                _sapp_x11_mouse_update(event->xmotion.x, event->xmotion.y, false);
                _sapp_x11_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID, _sapp_x11_mods(event->xmotion.state));
            }
            break;
        case ConfigureNotify:
            if ((event->xconfigure.width != _sapp.window_width) || (event->xconfigure.height != _sapp.window_height)) {
                _sapp.window_width = event->xconfigure.width;
                _sapp.window_height = event->xconfigure.height;
                _sapp.framebuffer_width = _sapp.window_width;
                _sapp.framebuffer_height = _sapp.window_height;
                _sapp_x11_app_event(SAPP_EVENTTYPE_RESIZED);
            }
            break;
        case PropertyNotify:
            if (event->xproperty.state == PropertyNewValue) {
                if (event->xproperty.atom == _sapp.x11.WM_STATE) {
                    const int state = _sapp_x11_get_window_state();
                    if (state != _sapp.x11.window_state) {
                        _sapp.x11.window_state = state;
                        if (state == IconicState) {
                            _sapp_x11_app_event(SAPP_EVENTTYPE_ICONIFIED);
                        }
                        else if (state == NormalState) {
                            _sapp_x11_app_event(SAPP_EVENTTYPE_RESTORED);
                        }
                    }
                }
            }
            break;
        case ClientMessage:
            if (filtered) {
                return;
            }
            if (event->xclient.message_type == _sapp.x11.WM_PROTOCOLS) {
                const Atom protocol = (Atom)event->xclient.data.l[0];
                if (protocol == _sapp.x11.WM_DELETE_WINDOW) {
                    _sapp.quit_requested = true;
                }
            }
            else if (event->xclient.message_type == _sapp.x11.xdnd.XdndEnter) {
                const bool is_list = 0 != (event->xclient.data.l[1] & 1);
                _sapp.x11.xdnd.source  = (Window)event->xclient.data.l[0];
                _sapp.x11.xdnd.version = event->xclient.data.l[1] >> 24;
                _sapp.x11.xdnd.format  = None;
                if (_sapp.x11.xdnd.version > _SAPP_X11_XDND_VERSION) {
                    return;
                }
                uint32_t count = 0;
                Atom* formats = 0;
                if (is_list) {
                    count = _sapp_x11_get_window_property(_sapp.x11.xdnd.source, _sapp.x11.xdnd.XdndTypeList, XA_ATOM, (unsigned char**)&formats);
                }
                else {
                    count = 3;
                    formats = (Atom*) event->xclient.data.l + 2;
                }
                for (uint32_t i = 0; i < count; i++) {
                    if (formats[i] == _sapp.x11.xdnd.text_uri_list) {
                        _sapp.x11.xdnd.format = _sapp.x11.xdnd.text_uri_list;
                        break;
                    }
                }
                if (is_list && formats) {
                    XFree(formats);
                }
            }
            else if (event->xclient.message_type == _sapp.x11.xdnd.XdndDrop) {
                if (_sapp.x11.xdnd.version > _SAPP_X11_XDND_VERSION) {
                    return;
                }
                Time time = CurrentTime;
                if (_sapp.x11.xdnd.format) {
                    if (_sapp.x11.xdnd.version >= 1) {
                        time = (Time)event->xclient.data.l[2];
                    }
                    XConvertSelection(_sapp.x11.display,
                                      _sapp.x11.xdnd.XdndSelection,
                                      _sapp.x11.xdnd.format,
                                      _sapp.x11.xdnd.XdndSelection,
                                      _sapp.x11.window,
                                      time);
                }
                else if (_sapp.x11.xdnd.version >= 2) {
                    XEvent reply;
                    _sapp_clear(&reply, sizeof(reply));
                    reply.type = ClientMessage;
                    reply.xclient.window = _sapp.x11.xdnd.source;
                    reply.xclient.message_type = _sapp.x11.xdnd.XdndFinished;
                    reply.xclient.format = 32;
                    reply.xclient.data.l[0] = (long)_sapp.x11.window;
                    reply.xclient.data.l[1] = 0;    // drag was rejected
                    reply.xclient.data.l[2] = None;
                    XSendEvent(_sapp.x11.display, _sapp.x11.xdnd.source, False, NoEventMask, &reply);
                    XFlush(_sapp.x11.display);
                }
            }
            else if (event->xclient.message_type == _sapp.x11.xdnd.XdndPosition) {
                /* drag operation has moved over the window
                   FIXME: we could track the mouse position here, but
                   this isn't implemented on other platforms either so far
                */
                if (_sapp.x11.xdnd.version > _SAPP_X11_XDND_VERSION) {
                    return;
                }
                XEvent reply;
                _sapp_clear(&reply, sizeof(reply));
                reply.type = ClientMessage;
                reply.xclient.window = _sapp.x11.xdnd.source;
                reply.xclient.message_type = _sapp.x11.xdnd.XdndStatus;
                reply.xclient.format = 32;
                reply.xclient.data.l[0] = (long)_sapp.x11.window;
                if (_sapp.x11.xdnd.format) {
                    /* reply that we are ready to copy the dragged data */
                    reply.xclient.data.l[1] = 1;    // accept with no rectangle
                    if (_sapp.x11.xdnd.version >= 2) {
                        reply.xclient.data.l[4] = (long)_sapp.x11.xdnd.XdndActionCopy;
                    }
                }
                XSendEvent(_sapp.x11.display, _sapp.x11.xdnd.source, False, NoEventMask, &reply);
                XFlush(_sapp.x11.display);
            }
            break;
        case SelectionNotify:
            if (event->xselection.property == _sapp.x11.xdnd.XdndSelection) {
                char* data = 0;
                uint32_t result = _sapp_x11_get_window_property(event->xselection.requestor,
                                                                event->xselection.property,
                                                                event->xselection.target,
                                                                (unsigned char**) &data);
                if (_sapp.drop.enabled && result) {
                    if (_sapp_x11_parse_dropped_files_list(data)) {
                        _sapp.mouse.dx = 0.0f;
                        _sapp.mouse.dy = 0.0f;
                        if (_sapp_events_enabled()) {
                            // FIXME: Figure out how to get modifier key state here.
                            // The XSelection event has no 'state' item, and
                            // XQueryKeymap() always returns a zeroed array.
                            _sapp_init_event(SAPP_EVENTTYPE_FILES_DROPPED);
                            _sapp_call_event(&_sapp.event);
                        }
                    }
                }
                if (_sapp.x11.xdnd.version >= 2) {
                    XEvent reply;
                    _sapp_clear(&reply, sizeof(reply));
                    reply.type = ClientMessage;
                    reply.xclient.window = _sapp.x11.xdnd.source;
                    reply.xclient.message_type = _sapp.x11.xdnd.XdndFinished;
                    reply.xclient.format = 32;
                    reply.xclient.data.l[0] = (long)_sapp.x11.window;
                    reply.xclient.data.l[1] = result;
                    reply.xclient.data.l[2] = (long)_sapp.x11.xdnd.XdndActionCopy;
                    XSendEvent(_sapp.x11.display, _sapp.x11.xdnd.source, False, NoEventMask, &reply);
                    XFlush(_sapp.x11.display);
                }
            }
            break;
        case DestroyNotify:
            break;
    }
}

#if !defined(_SAPP_GLX)

_SOKOL_PRIVATE void _sapp_egl_init(void) {
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        _SAPP_PANIC(LINUX_EGL_BIND_OPENGL_ES_API_FAILED);
    }

    _sapp.egl.display = eglGetDisplay((EGLNativeDisplayType)_sapp.x11.display);
    if (EGL_NO_DISPLAY == _sapp.egl.display) {
        _SAPP_PANIC(LINUX_EGL_GET_DISPLAY_FAILED);
    }

    EGLint major, minor;
    if (!eglInitialize(_sapp.egl.display, &major, &minor)) {
        _SAPP_PANIC(LINUX_EGL_INITIALIZE_FAILED);
    }

    EGLint sample_count = _sapp.desc.sample_count > 1 ? _sapp.desc.sample_count : 0;
    EGLint alpha_size = _sapp.desc.alpha ? 8 : 0;
    const EGLint config_attrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, alpha_size,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_SAMPLE_BUFFERS, _sapp.desc.sample_count > 1 ? 1 : 0,
        EGL_SAMPLES, sample_count,
        EGL_NONE,
    };

    EGLConfig egl_configs[32];
    EGLint config_count;
    if (!eglChooseConfig(_sapp.egl.display, config_attrs, egl_configs, 32, &config_count) || config_count == 0) {
        _SAPP_PANIC(LINUX_EGL_NO_CONFIGS);
    }

    EGLConfig config = egl_configs[0];
    for (int i = 0; i < config_count; ++i) {
        EGLConfig c = egl_configs[i];
        EGLint r, g, b, a, d, s, n;
        if (eglGetConfigAttrib(_sapp.egl.display, c, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(_sapp.egl.display, c, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(_sapp.egl.display, c, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(_sapp.egl.display, c, EGL_ALPHA_SIZE, &a) &&
            eglGetConfigAttrib(_sapp.egl.display, c, EGL_DEPTH_SIZE, &d) &&
            eglGetConfigAttrib(_sapp.egl.display, c, EGL_STENCIL_SIZE, &s) &&
            eglGetConfigAttrib(_sapp.egl.display, c, EGL_SAMPLES, &n) &&
            (r == 8) && (g == 8) && (b == 8) && (a == alpha_size) && (d == 24) && (s == 8) && (n == sample_count)) {
            config = c;
            break;
        }
    }

    EGLint visual_id;
    if (!eglGetConfigAttrib(_sapp.egl.display, config, EGL_NATIVE_VISUAL_ID, &visual_id)) {
        _SAPP_PANIC(LINUX_EGL_NO_NATIVE_VISUAL);
    }

    XVisualInfo visual_info_template;
    _sapp_clear(&visual_info_template, sizeof(visual_info_template));
    visual_info_template.visualid = (VisualID)visual_id;

    int num_visuals;
    XVisualInfo* visual_info = XGetVisualInfo(_sapp.x11.display, VisualIDMask, &visual_info_template, &num_visuals);
    if (!visual_info) {
        _SAPP_PANIC(LINUX_EGL_GET_VISUAL_INFO_FAILED);
    }

    _sapp_x11_create_window(visual_info->visual, visual_info->depth);
    XFree(visual_info);

    _sapp.egl.surface = eglCreateWindowSurface(_sapp.egl.display, config, (EGLNativeWindowType)_sapp.x11.window, NULL);
    if (EGL_NO_SURFACE == _sapp.egl.surface) {
        _SAPP_PANIC(LINUX_EGL_CREATE_WINDOW_SURFACE_FAILED);
    }

    EGLint ctx_attrs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE,
    };

    _sapp.egl.context = eglCreateContext(_sapp.egl.display, config, EGL_NO_CONTEXT, ctx_attrs);
    if (EGL_NO_CONTEXT == _sapp.egl.context) {
        _SAPP_PANIC(LINUX_EGL_CREATE_CONTEXT_FAILED);
    }

    if (!eglMakeCurrent(_sapp.egl.display, _sapp.egl.surface, _sapp.egl.surface, _sapp.egl.context)) {
        _SAPP_PANIC(LINUX_EGL_MAKE_CURRENT_FAILED);
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_sapp.gl.framebuffer);

    eglSwapInterval(_sapp.egl.display, _sapp.swap_interval);
}

_SOKOL_PRIVATE void _sapp_egl_destroy(void) {
    if (_sapp.egl.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(_sapp.egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (_sapp.egl.context != EGL_NO_CONTEXT) {
            eglDestroyContext(_sapp.egl.display, _sapp.egl.context);
            _sapp.egl.context = EGL_NO_CONTEXT;
        }

        if (_sapp.egl.surface != EGL_NO_SURFACE) {
            eglDestroySurface(_sapp.egl.display, _sapp.egl.surface);
            _sapp.egl.surface = EGL_NO_SURFACE;
        }

        eglTerminate(_sapp.egl.display);
        _sapp.egl.display = EGL_NO_DISPLAY;
    }
}

#endif /* _SAPP_GLX */

_SOKOL_PRIVATE void _sapp_linux_run(const sapp_desc* desc) {
    /* The following lines are here to trigger a linker error instead of an
        obscure runtime error if the user has forgotten to add -pthread to
        the compiler or linker options. They have no other purpose.
    */
    pthread_attr_t pthread_attr;
    pthread_attr_init(&pthread_attr);
    pthread_attr_destroy(&pthread_attr);

    _sapp_init_state(desc);
    _sapp.x11.window_state = NormalState;

    XInitThreads();
    XrmInitialize();
    _sapp.x11.display = XOpenDisplay(NULL);
    if (!_sapp.x11.display) {
        _SAPP_PANIC(LINUX_X11_OPEN_DISPLAY_FAILED);
    }
    _sapp.x11.screen = DefaultScreen(_sapp.x11.display);
    _sapp.x11.root = DefaultRootWindow(_sapp.x11.display);
    XkbSetDetectableAutoRepeat(_sapp.x11.display, true, NULL);
    _sapp_x11_query_system_dpi();
    _sapp.dpi_scale = _sapp.x11.dpi / 96.0f;
    _sapp_x11_init_extensions();
    _sapp_x11_create_cursors();
#if defined(_SAPP_GLX)
    _sapp_glx_init();
    Visual* visual = 0;
    int depth = 0;
    _sapp_glx_choose_visual(&visual, &depth);
    _sapp_x11_create_window(visual, depth);
    _sapp_glx_create_context();
    _sapp_glx_swapinterval(_sapp.swap_interval);
#else
    _sapp_egl_init();
#endif
    sapp_set_icon(&desc->icon);
    _sapp.valid = true;
    _sapp_x11_show_window();
    if (_sapp.fullscreen) {
        _sapp_x11_set_fullscreen(true);
    }

    XFlush(_sapp.x11.display);
    while (!_sapp.quit_ordered) {
        _sapp_timing_measure(&_sapp.timing);
        int count = XPending(_sapp.x11.display);
        while (count--) {
            XEvent event;
            XNextEvent(_sapp.x11.display, &event);
            _sapp_x11_process_event(&event);
        }
        _sapp_frame();
#if defined(_SAPP_GLX)
        _sapp_glx_swap_buffers();
#else
        eglSwapBuffers(_sapp.egl.display, _sapp.egl.surface);
#endif
        XFlush(_sapp.x11.display);
        /* handle quit-requested, either from window or from sapp_request_quit() */
        if (_sapp.quit_requested && !_sapp.quit_ordered) {
            /* give user code a chance to intervene */
            _sapp_x11_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
            /* if user code hasn't intervened, quit the app */
            if (_sapp.quit_requested) {
                _sapp.quit_ordered = true;
            }
        }
    }
    _sapp_call_cleanup();
#if defined(_SAPP_GLX)
    _sapp_glx_destroy_context();
#else
    _sapp_egl_destroy();
#endif
    _sapp_x11_destroy_window();
    _sapp_x11_destroy_cursors();
    XCloseDisplay(_sapp.x11.display);
    _sapp_discard_state();
}

#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_linux_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */
