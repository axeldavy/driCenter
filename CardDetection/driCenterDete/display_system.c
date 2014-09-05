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

#include <malloc.h>

#include "../include/misc.h"
#include "../include/display_system.h"
#include "../include/gpu_info.h"

bool get_display_var(struct display_system_info *display_system_info, void **display)
{
    if (display_system_info->struct_flag_display_system != FILLED)
        return FALSE; /* It doesn't make sense to try to fill it, because we don't know which display system to try */

    if (display_system_info->display_system_type_info == WAYLAND_DISPLAY_SERVER)
        return wayland_get_display_var(display_system_info, display);

    if (display_system_info->display_system_type_info == X_DISPLAY_SERVER)
        return x_get_display_var(display_system_info, display);

    if (display_system_info->display_system_type_info == DRM_DISPLAY_SERVER)
        return drm_get_display_var(display_system_info, display);

    return FALSE;
}

void free_display_info(struct display_system_info *display_system_info)
{
    if (display_system_info->struct_flag_display_system == FILLED) {
        switch (display_system_info->display_system_type_info) {
            case DRM_DISPLAY_SERVER: free_drm_info(display_system_info);
            case X_DISPLAY_SERVER: free_x_info(display_system_info);
            case WAYLAND_DISPLAY_SERVER: free_wayland_info(display_system_info);
        }
    }
    if (display_system_info->struct_flag_egl_info == FILLED) {
        free_egl_info(display_system_info);
    }
    if (display_system_info->struct_flag_glx_info == FILLED) {
        free_glx_info(display_system_info);
    }
}

// TODO: handle memory failure
void fill_display_system_info_drm(struct gpu_info *gpu_info, char *path)
{
    struct display_system_info *display_system_info;
    
    display_system_info = (struct display_system_info *) calloc(1, sizeof(struct display_system_info));
    display_system_info->display_system_type_info = DRM_DISPLAY_SERVER;

    fill_drm_info(display_system_info, path);
    if (display_system_info->struct_flag_display_system != FILLED) {
        gpu_info->struct_flag_display_system_info_drm = FAILURE;
        free(display_system_info);
        return;
    }
    
    gpu_info->display_system_info_drm = display_system_info;
    gpu_info->struct_flag_display_system_info_drm = FILLED;
    fill_egl_info(display_system_info);
    fill_glx_info(display_system_info);
}

void fill_display_system_info_x(struct gpu_info *gpu_info)
{
    struct display_system_info *display_system_info;
    
    display_system_info = (struct display_system_info *) calloc(1, sizeof(struct display_system_info));
    display_system_info->display_system_type_info = X_DISPLAY_SERVER;

    fill_x_info(display_system_info);
    if (display_system_info->struct_flag_display_system != FILLED) {
        gpu_info->struct_flag_display_system_info_x = FAILURE;
        free(display_system_info);
        return;
    }
    
    gpu_info->display_system_info_x = display_system_info;
    gpu_info->struct_flag_display_system_info_x = FILLED;
    fill_egl_info(display_system_info);
    fill_glx_info(display_system_info);
}

void fill_display_system_info_wayland(struct gpu_info *gpu_info)
{
    struct display_system_info *display_system_info;
    
    display_system_info = (struct display_system_info *) calloc(1, sizeof(struct display_system_info));
    display_system_info->display_system_type_info = WAYLAND_DISPLAY_SERVER;

    fill_wayland_info(display_system_info);
    if (display_system_info->struct_flag_display_system != FILLED) {
        gpu_info->struct_flag_display_system_info_wayland = FAILURE;
        free(display_system_info);
        return;
    }
    
    gpu_info->display_system_info_wayland = display_system_info;
    gpu_info->struct_flag_display_system_info_wayland = FILLED;
    fill_egl_info(display_system_info);
    fill_glx_info(display_system_info);
}



void display_system_info_clean(struct display_system_info *reference, struct display_system_info *to_clean)
{
    if (reference->display_system_type_info == X_DISPLAY_SERVER
        && to_clean->display_system_type_info == X_DISPLAY_SERVER) {
        if (reference->struct_flag_glx_info == FILLED &&
            to_clean->struct_flag_glx_info == FILLED &&
            glx_info_similar(reference->glx_info, to_clean->glx_info))
            free_glx_info(to_clean);
        if (reference->struct_flag_egl_info == FILLED &&
            to_clean->struct_flag_egl_info == FILLED &&
            egl_info_similar(reference->egl_info, to_clean->egl_info))
            free_egl_info(to_clean);
    }
}

char *display_get_gl_renderer(struct display_system_info *display_system_info)
{
    if (display_system_info->struct_flag_glx_info == FILLED)
       return glx_get_gl_renderer(display_system_info->glx_info);
    if (display_system_info->struct_flag_egl_info == FILLED)
       return egl_get_gl_renderer(display_system_info->egl_info);
    return NULL;
}

bool display_system_info_similar(struct display_system_info *first, struct display_system_info *second)
{
    bool ret = TRUE;

    if (first->struct_flag_glx_info == FILLED && second->struct_flag_glx_info == FILLED)
        ret &= glx_info_similar(first->glx_info, second->glx_info);
    ret &= first->struct_flag_glx_info == second->struct_flag_glx_info;

    if (first->struct_flag_egl_info == FILLED && second->struct_flag_egl_info == FILLED)
        ret &= egl_info_similar(first->egl_info, second->egl_info);
    ret &= first->struct_flag_egl_info == second->struct_flag_egl_info;
    return ret;
}

bool display_system_info_equal(struct display_system_info *first, struct display_system_info *second)
{
    if (first->display_system_type_info != second->display_system_type_info)
        return FALSE;
    return display_system_info_similar(first, second);
}
