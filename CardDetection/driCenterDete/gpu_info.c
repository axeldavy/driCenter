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

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "../include/misc.h"
#include "../include/display_system.h"
#include "../include/gpu_info.h"

void free_gpu_info(struct gpu_info *gpu_info)
{
    free(gpu_info->devnode);

    if (gpu_info->id_path_tag)
        free(gpu_info->id_path_tag);

    if (gpu_info->device)
        udev_device_unref(gpu_info->device);

    if (gpu_info->struct_flag_display_system_info_drm == FILLED)
        free_display_info(gpu_info->display_system_info_drm);

    if (gpu_info->struct_flag_display_system_info_x == FILLED)
        free_display_info(gpu_info->display_system_info_x);

    if (gpu_info->struct_flag_display_system_info_wayland == FILLED)
        free_display_info(gpu_info->display_system_info_wayland);
}

struct gpu_info * fill_gpu_display_info(char *devnode, char *id_path_tag, struct udev_device *device, bool detect_drm, bool detect_non_drm)
{
    struct gpu_info *gpu_info;

    gpu_info = (struct gpu_info *) calloc(1, sizeof(struct gpu_info));
    if (!gpu_info)
        return NULL;

    gpu_info->devnode = devnode;
    gpu_info->id_path_tag = id_path_tag;
    gpu_info->device = device;

    if (detect_drm)
        fill_display_system_info_drm(gpu_info, devnode);
    else
        gpu_info->struct_flag_display_system_info_drm = NOT_APPLICABLE;

    if (detect_non_drm) {
        fill_display_system_info_x(gpu_info);
        fill_display_system_info_wayland(gpu_info);
    } else {
        gpu_info->struct_flag_display_system_info_x = NOT_APPLICABLE;
        gpu_info->struct_flag_display_system_info_wayland = NOT_APPLICABLE;
    }
    return gpu_info;
}

//static struct display_system_info * get_filled_display_info(struct gpu_info *gpu_info)
//{
//    if (gpu_info->struct_flag_display_system_info_drm == FILLED)
//        return gpu_info->display_system_info_drm;

//    if (gpu_info->struct_flag_display_system_info_x == FILLED)
//        return gpu_info->display_system_info_x;

//    if (gpu_info->struct_flag_display_system_info_wayland == FILLED)
//        return gpu_info->display_system_info_wayland;

//    return NULL;
//}

char *get_gl_renderer(struct gpu_info *gpu_info) {
    char *renderer = NULL;

    if (gpu_info->struct_flag_display_system_info_drm == FILLED)
        renderer = display_get_gl_renderer(gpu_info->display_system_info_drm);
    if (renderer)
        return renderer;

    if (gpu_info->struct_flag_display_system_info_x == FILLED)
        renderer = display_get_gl_renderer(gpu_info->display_system_info_x);
    if (renderer)
        return renderer;

    if (gpu_info->struct_flag_display_system_info_wayland == FILLED)
        renderer = display_get_gl_renderer(gpu_info->display_system_info_wayland);
    return renderer;
}

static bool compare_gl_renderer(struct gpu_info *first, struct gpu_info *second)
{
    char *first_renderer, *second_renderer;

    first_renderer = get_gl_renderer(first);
    second_renderer = get_gl_renderer(second);
    return (first_renderer != NULL && second_renderer != NULL && !strcmp(first_renderer, second_renderer));
}

static bool gpu_info_can_merge(struct gpu_info *first, struct gpu_info *second) {
    if (first->id_path_tag && second->id_path_tag &&
        strcmp(first->id_path_tag, second->id_path_tag))
        return FALSE;

    if (first->struct_flag_display_system_info_drm == FILLED &&
        second->struct_flag_display_system_info_drm == FILLED &&
        !display_system_info_equal(first->display_system_info_drm, second->display_system_info_drm))
        return FALSE;

    if (first->struct_flag_display_system_info_x == FILLED &&
        second->struct_flag_display_system_info_x == FILLED &&
        !display_system_info_equal(first->display_system_info_x, second->display_system_info_x))
        return FALSE;

    if (first->struct_flag_display_system_info_wayland == FILLED &&
        second->struct_flag_display_system_info_wayland == FILLED &&
        !display_system_info_equal(first->display_system_info_wayland, second->display_system_info_wayland))
        return FALSE;

    if (first->struct_flag_display_system_info_drm != second->struct_flag_display_system_info_drm &&
        first->struct_flag_display_system_info_x != second->struct_flag_display_system_info_x &&
        first->struct_flag_display_system_info_wayland != second->struct_flag_display_system_info_wayland &&
        !compare_gl_renderer(first, second))
        return FALSE;
    return TRUE;
}

static void gpu_info_merge(struct gpu_info *first, struct gpu_info *second)
{
    char *temp;
    struct display_system_info *temp_disp;

    if (!first->id_path_tag) {
        first->id_path_tag = second->id_path_tag;
        second->id_path_tag = NULL;
    }

    if (second->struct_flag_display_system_info_drm == FILLED &&
        first->struct_flag_display_system_info_drm != FILLED) {
        /* second's devnode is better. Exchange the devnodes.
         * The first's devnode is going to be free with second.*/
        temp = first->devnode;
        first->devnode = second->devnode;
        second->devnode = temp;
    } else if (!first->devnode) {
        first->devnode = second->devnode;
        second->devnode = NULL;
    }

    if (!first->device) {
        first->device = second->device;
        second->device = NULL;
    }

    if (first->struct_flag_display_system_info_drm != FILLED &&
        second->struct_flag_display_system_info_drm == FILLED) {
        temp_disp = first->display_system_info_drm;
        first->display_system_info_drm = second->display_system_info_drm;
        second->display_system_info_drm = temp_disp;
        second->struct_flag_display_system_info_drm = first->struct_flag_display_system_info_drm;
        first->struct_flag_display_system_info_drm = FILLED;
    }

    if (first->struct_flag_display_system_info_x != FILLED &&
        second->struct_flag_display_system_info_x == FILLED) {
        temp_disp = first->display_system_info_x;
        first->display_system_info_x = second->display_system_info_x;
        second->display_system_info_x = temp_disp;
        second->struct_flag_display_system_info_x = first->struct_flag_display_system_info_x;
        first->struct_flag_display_system_info_x = FILLED;
    }

    if (first->struct_flag_display_system_info_wayland != FILLED &&
        second->struct_flag_display_system_info_wayland == FILLED) {
        temp_disp = first->display_system_info_wayland;
        first->display_system_info_wayland = second->display_system_info_wayland;
        second->display_system_info_wayland = temp_disp;
        second->struct_flag_display_system_info_wayland = first->struct_flag_display_system_info_wayland;
        first->struct_flag_display_system_info_wayland = FILLED;
    }

    free_gpu_info(second);
}

/* Clean cases when DRI_PRIME didn't work */
static void gpu_info_clean(struct gpu_info *reference, struct gpu_info *to_clean)
{
    if (reference->struct_flag_display_system_info_x == FILLED &&
        to_clean->struct_flag_display_system_info_x == FILLED) {
        if (display_system_info_equal(reference->display_system_info_x,
                                       to_clean->display_system_info_x)) {
            free_display_info(to_clean->display_system_info_x);
            to_clean->struct_flag_display_system_info_x = FAILURE;
        } else
            display_system_info_clean(reference->display_system_info_x, to_clean->display_system_info_x);
    }

    if (reference->struct_flag_display_system_info_wayland == FILLED &&
        to_clean->struct_flag_display_system_info_wayland == FILLED) {
        if (display_system_info_equal(reference->display_system_info_wayland,
                                       to_clean->display_system_info_wayland)) {
            free_display_info(to_clean->display_system_info_wayland);
            to_clean->struct_flag_display_system_info_wayland = FAILURE;
        } else
            display_system_info_clean(reference->display_system_info_wayland, to_clean->display_system_info_wayland);
    }
}

int gpu_info_list_clean(struct gpu_info *reference, struct gpu_info **gpu_info_list, int max_elem)
{
    struct gpu_info *first, *second;
    int i, j, num_gpus;
    for (i = 0; i < max_elem; i++) {
        first = gpu_info_list[i];
        if (first == NULL)
            continue;
        for(j = max_elem - 1; j > i; j--) {
            second = gpu_info_list[j];
            if (second == NULL)
                continue;
            if (gpu_info_can_merge(first, second)) {
                gpu_info_merge(first, second);
                gpu_info_list[j] = NULL;
            }
        }
    }

    num_gpus = 0;
    for (i = 0; i < max_elem; i++)
        if (gpu_info_list[i])
            num_gpus++;

    j = 0;
    for (i = 0; i < max_elem; i++) {
        if (gpu_info_list[i]) {
            gpu_info_list[j] = gpu_info_list[i];
            j++;
        }
    }

    if (reference) {
        for (i = 0; i < num_gpus; i++) {
            first = gpu_info_list[i];
            if (!gpu_info_can_merge(reference, first) || !compare_gl_renderer(reference, first))
                gpu_info_clean(reference, first);
        }
    }

    return num_gpus;
}
