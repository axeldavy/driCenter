/*
 * Copyright © 2014 Axel Davy
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the
 * copyright holders not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef GLX_INFO_H
#define GLX_INFO_H

#include <GL/glx.h>
#include <GL/glxext.h>

#include "misc.h"
#include "gl_info.h"
#include "display_system.h"


struct glx_info {
    struct gl_info base_compatibility;
    struct gl_info base_core;
    bool has_core;
    Display *dpy;
    int major;
    int minor;
    char *glx_extensions_string;
//    char *glx_vendor_string;
//    char *glx_version_string;
    int num_configs;
};

void free_glx_info(struct display_system_info *display_system_info);
void fill_glx_info(struct display_system_info *display_system_info);
char *glx_get_gl_renderer(struct glx_info *glx_info);
bool glx_info_similar(struct glx_info *first, struct glx_info *second);

#endif /* GLX_INFO_H */
