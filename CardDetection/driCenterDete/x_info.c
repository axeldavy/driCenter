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

#include <X11/Xlib.h>
#include <stdbool.h>
#include <malloc.h>

#include "../include/display_system.h"
#include "../include/x_info.h"

bool x_get_display_var(struct display_system_info *display_system_info, void **display)
{
    *display = display_system_info->display_system.x_info->display;
    return TRUE;
}


void free_x_info(struct display_system_info *display_system_info)
{
    if (display_system_info->struct_flag_display_system != FILLED)
        return;

    XCloseDisplay(display_system_info->display_system.x_info->display);
    free(display_system_info->display_system.x_info);
    display_system_info->display_system.x_info = NULL;
    display_system_info->struct_flag_display_system = NOT_FILLED_YET;
}

void fill_x_info(struct display_system_info *display_system_info)
{
    struct x_info *x_info;

    x_info = calloc(1, sizeof(x_info));
    if (!x_info) {
        display_system_info->struct_flag_display_system = MEMORY_FAILURE;
        return;
    }

    x_info->display = XOpenDisplay(NULL);
    if (!x_info->display)
        goto display_failure;

    display_system_info->display_system.x_info = x_info;
    display_system_info->struct_flag_display_system = FILLED;
    return;
    

display_failure:
    free(x_info);
    if (display_system_info->struct_flag_display_system != MEMORY_FAILURE)
        display_system_info->struct_flag_display_system = FAILURE;
}
