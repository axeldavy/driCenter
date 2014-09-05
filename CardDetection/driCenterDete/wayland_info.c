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

#include <wayland-client.h>
#include <stdbool.h>
#include <malloc.h>

#include "../include/display_system.h"
#include "../include/wayland_info.h"

bool wayland_get_display_var(struct display_system_info *display_system_info, void **display)
{
    struct wayland_info *wayland_info = display_system_info->display_system.wayland_info;
    *display = wayland_info->display;
    return true;
}


void free_wayland_info(struct display_system_info *display_system_info)
{
    if (display_system_info->struct_flag_display_system != FILLED)
        return;

    free(display_system_info->display_system.wayland_info);
    display_system_info->display_system.wayland_info = NULL;
    display_system_info->struct_flag_display_system = NOT_FILLED_YET;
}

/*static void
registry_handle_global(void *data, struct wl_registry *registry,
                       uint32_t name, const char *interface, uint32_t version)
{
    struct wl_compositor **compositor_var = data;

    if (strcmp(interface, "wl_compositor") == 0)
        *compositor_var = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
                              uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
        registry_handle_global,
        registry_handle_global_remove
};

 void fill_wayland_egl_specific(struct wayland_info *wayland_info)
{
    struct wayland_egl_specific *egl_specific;

    egl_specific = calloc(1, sizeof(wayland_egl_specific));
    if (!egl_specific) {
        wayland_info->struct_flag_egl_specific = MEMORY_FAILURE;
        return;
    }

    egl_specific->wl_egl_surface = wl_egl_window_create(wayland_info->dummy_surface, 400, 200);
    if (!egl_specific->wl_egl_surface) {
        wayland_info->struct_flag_egl_specific = FAILURE;
        free(egl_specific);
        return;
    }

    wayland_info->egl_specific = egl_specific;
    wayland_info->struct_flag_egl_specific = FILLED;
} */

void fill_wayland_info(struct display_system_info *display_system_info)
{
    struct wayland_info *wayland_info;

    wayland_info = calloc(1, sizeof(wayland_info));
    if (!wayland_info) {
        display_system_info->struct_flag_display_system = MEMORY_FAILURE;
        return;
    }

    wayland_info->display = wl_display_connect(NULL);
    if (!wayland_info->display)
        goto display_failure;

/*    wayland_info->registry = wl_display_get_registry(wayland_info->display);
    if (!wayland_info->registry)
        goto registry_init_failure;

    wl_registry_add_listener(registry, &registry_listener, &wayland_info->compositor);
    if (!wayland_info->compositor)
        goto registry_failure;
    
    wayland_info->dummy_surface = wl_compositor_create_surface(wayland_info->compositor);
    if (!dummy_surface)
        goto dummy_surface_failure;

    fill_wayland_egl_specific(wayland_info); */
    display_system_info->display_system.wayland_info = wayland_info;
    display_system_info->struct_flag_display_system = FILLED;
    return;
    

/*dummy_surface_failure:
    wl_compositor_destroy(wayland_info->compositor);
registry_failure:
    wl_registry_destroy(wayland_info->registry);
registry_init_failure:
    wl_display_flush(wayland_info->display);
    wl_display_disconnect(wayland_info->display);*/
display_failure:
    free(wayland_info);
    if (display_system_info->struct_flag_display_system != MEMORY_FAILURE)
        display_system_info->struct_flag_display_system = FAILURE;
}
