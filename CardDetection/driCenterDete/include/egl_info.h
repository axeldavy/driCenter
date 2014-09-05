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

#ifndef EGL_INFO_H
#define EGL_INFO_H

#include <EGL/egl.h>

#include "misc.h"
#include "display_system.h"
#include "gl_info.h"

struct egl_gl_info {
    struct gl_info base_core;
    struct gl_info base_compatibility;
    EGLContext context_core;
    EGLContext context_compatibility;
    bool has_core;
};

struct egl_gles_info {
    struct gl_info base_gles1;
    struct gl_info base_gles2;
    struct gl_info base_gles3;
    EGLContext context_gles1;
    EGLContext context_gles2;
    EGLContext context_gles3;
    bool has_gles1;
    bool has_gles2;
    bool has_gles3;
};

struct egl_info {
    struct egl_gl_info *egl_gl_info;
    enum struct_flag struct_flag_gl_info;

    struct egl_gles_info *egl_gles_info;
    enum struct_flag struct_flag_gles_info;

    EGLDisplay dpy;
    EGLint major;
    EGLint minor;
    char *client_apis_string;
    char *egl_extensions_string;
    char *egl_vendor_string;
    char *egl_version_string;
    EGLint num_configs;
    EGLConfig *egl_configs;
};

void free_egl_info(struct display_system_info *display_system_info);
void fill_egl_info(struct display_system_info *display_system_info);
char *egl_get_gl_renderer(struct egl_info *egl_info);
bool egl_info_similar(struct egl_info *first, struct egl_info *second);

#endif /* EGL_INFO_H */
