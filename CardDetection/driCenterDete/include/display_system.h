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

#ifndef DISPLAY_SYSTEM_INFO_H
#define DISPLAY_SYSTEM_INFO_H

struct display_system_info;

#include "dricenter-lib.h"
#include "misc.h"
#include "wayland_info.h"
#include "x_info.h"
#include "drm_info.h"
#include "glx_info.h"
#include "egl_info.h"
#include "gpu_info.h"

struct display_system_info {
    union display_system {
        struct wayland_info *wayland_info;
        struct x_info *x_info;
        struct drm_info *drm_info;
    } display_system;
    enum struct_flag struct_flag_display_system;
    enum display_server_type display_system_type_info;

    struct glx_info *glx_info;
    enum struct_flag struct_flag_glx_info;

    struct egl_info *egl_info;
    enum struct_flag struct_flag_egl_info;
};

bool get_display_var(struct display_system_info *display_system_info, void **display);
void free_display_info(struct display_system_info *display_system_info);
void fill_display_system_info_drm(struct gpu_info *gpu_info, char *path);
void fill_display_system_info_x(struct gpu_info *gpu_info);
void fill_display_system_info_wayland(struct gpu_info *gpu_info);
void display_system_info_clean(struct display_system_info *reference, struct display_system_info *to_clean);
char *display_get_gl_renderer(struct display_system_info *display_system_info);
bool display_system_info_similar(struct display_system_info *first, struct display_system_info *second);
bool display_system_info_equal(struct display_system_info *first, struct display_system_info *second);
#endif /* DISPLAY_SYSTEM_INFO_H */

