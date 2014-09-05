/*
 * Copyright © 2014 Alvaro García
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

#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <libudev.h>

#include "misc.h"


struct bios_info {
    char *date;
    char *part_number;
};

struct bus_info {
    char *type;  
    int width; 
};

struct memory_info {
    char *type;
    int clock; 
    int size; 
    double bandwidth;
};

struct hardware_info {
    int core_clock; 

    struct bios_info *bios;
    enum struct_flag struct_flag_bios_info;

    struct bus_info *bus;
    enum struct_flag struct_flag_bus_info;

    struct memory_info *memory;
    enum struct_flag struct_flag_memory_info;
};

struct gpu_info {   
    struct hardware_info *hardware;
    enum struct_flag struct_flag_hardware_info;

    struct display_system_info *display_system_info_x;
    enum struct_flag struct_flag_display_system_info_x;

    struct display_system_info *display_system_info_wayland;
    enum struct_flag struct_flag_display_system_info_wayland;

    struct display_system_info *display_system_info_drm;
    enum struct_flag struct_flag_display_system_info_drm;

    char *id_path_tag;
    char *devnode;
    struct udev_device *device;
};

struct gpu_info * fill_gpu_display_info(char *devnode, char *id_path_tag, struct udev_device *device, bool detect_drm, bool detect_non_drm);
void free_gpu_info(struct gpu_info *gpu_info);
int gpu_info_list_clean(struct gpu_info *reference, struct gpu_info **gpu_info_list, int num_elem);
char *get_gl_renderer(struct gpu_info *gpu_info);

#endif /* GPU_INFO_H */
