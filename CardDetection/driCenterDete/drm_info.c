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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <gbm.h>

#include "../include/drm_info.h"

bool drm_get_display_var(struct display_system_info *display_system_info, void **display)
{
    *display = display_system_info->display_system.drm_info->gbm_device;
    return TRUE;
}


void free_drm_info(struct display_system_info *display_system_info)
{
    if (display_system_info->struct_flag_display_system != FILLED)
        return;

    close(display_system_info->display_system.drm_info->fd);
    free(display_system_info->display_system.drm_info);
    display_system_info->display_system.drm_info = NULL;
    display_system_info->struct_flag_display_system = NOT_FILLED_YET;
}

void fill_drm_info(struct display_system_info *display_system_info, char *path)
{
    struct drm_info *drm_info;

    drm_info = calloc(1, sizeof(drm_info));
    if (!drm_info) {
        display_system_info->struct_flag_display_system = MEMORY_FAILURE;
        return;
    }

    drm_info->fd = open(path, O_RDWR);
    if (!drm_info->fd)
        goto fd_failure;

    drm_info->gbm_device = gbm_create_device(drm_info->fd);
    if (!drm_info->gbm_device)
        goto gbm_failure;

    display_system_info->display_system.drm_info = drm_info;
    display_system_info->struct_flag_display_system = FILLED;
    return;

gbm_failure:
    close(drm_info->fd);
fd_failure:
    free(drm_info);
    if (display_system_info->struct_flag_display_system != MEMORY_FAILURE)
        display_system_info->struct_flag_display_system = FAILURE;
} 
