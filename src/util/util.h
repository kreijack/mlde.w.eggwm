
#include <xcb/xcb.h>

const char * errorCodeToText(unsigned long e);
const char * requestCodeToText(unsigned char rc);
void dumpXCBError(xcb_generic_error_t *err);
