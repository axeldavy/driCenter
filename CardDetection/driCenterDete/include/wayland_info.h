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

#ifndef WAYLAND_INFO_H
#define WAYLAND_INFO_H

#include <wayland-client.h>

#include "display_system.h"

/*struct wayland_egl_specific {
    struct wl_egl_window *wl_egl_surface;
}*/

struct wayland_info {
    struct wl_display *display;
/*    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_surface *dummy_surface;
    struct wayland_egl_specific *egl_specific;
    enum struct_flag struct_flag_egl_specific;*/
};

bool wayland_get_display_var(struct display_system_info *display_system_info, void **display);
void free_wayland_info(struct display_system_info *display_system_info);
void fill_wayland_info(struct display_system_info *display_system_info);

#endif /* WAYLAND_INFO_H */
