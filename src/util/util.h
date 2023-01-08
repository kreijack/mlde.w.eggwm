/**
 * Copyright (C) 2023 Goffredo Baroncelli <kreijack@libero.it>
 *
 * @file util/util.h
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @author Goffredo Baroncelli
 */
#include <xcb/xcb.h>

const char * errorCodeToText(unsigned long e);
const char * requestCodeToText(unsigned char rc);
void dumpXCBError(xcb_generic_error_t *err);
