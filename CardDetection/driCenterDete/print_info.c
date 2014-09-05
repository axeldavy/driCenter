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

#include <stdio.h>

#include "../include/misc.h"
#include "../include/system.h"
#include "../include/gpu_info.h"
#include "../include/display_system.h"
#include "../include/egl_info.h"
#include "../include/glx_info.h"
#include "../include/gl_info.h"

static void print_info_gl(struct gl_info *gl_info)
{
    printf("\t\t\t\t\tVersion: %s\n", gl_info->version);
    printf("\t\t\t\t\tVendor: %s\n", gl_info->vendor);
    printf("\t\t\t\t\tRenderer: %s\n", gl_info->renderer);
    printf("\t\t\t\t\tShading language version: %s\n", gl_info->shading_language_version);
    printf("\t\t\t\t\tExtensions: %s\n", gl_info->extensions);
}

static void print_info_egl_gles(struct egl_gles_info *egl_gles_info)
{
    if (egl_gles_info->has_gles1) {
        printf("\t\t\t\tOpenGL ES1:\n");
        print_info_gl(&egl_gles_info->base_gles1);
    } else {
        printf("\t\t\t\tNo OpenGL ES1\n");
    }
    if (egl_gles_info->has_gles2) {
        printf("\t\t\t\tOpenGL ES2:\n");
        print_info_gl(&egl_gles_info->base_gles2);
    } else {
        printf("\t\t\t\tNo OpenGL ES2\n");
    }
    if (egl_gles_info->has_gles3) {
        printf("\t\t\t\tOpenGL ES3:\n");
        print_info_gl(&egl_gles_info->base_gles3);
    } else {
        printf("\t\t\t\tNo OpenGL ES3\n");
    }
}

static void print_info_egl_gl(struct egl_gl_info *egl_gl_info)
{
    printf("\t\t\t\tCompatibility context:\n");
    print_info_gl(&egl_gl_info->base_compatibility);
    if (egl_gl_info->has_core) {
        printf("\t\t\t\tCore context:\n");
        print_info_gl(&egl_gl_info->base_core);
    } else {
        printf("\t\t\t\tNo Core context\n");
    }
}

static void print_info_egl(struct egl_info *egl_info)
{
    printf("\t\t\tVersion string: %s\n", egl_info->egl_version_string);
    printf("\t\t\tClient APIs: %s\n", egl_info->client_apis_string);
    printf("\t\t\tVendor string: %s\n", egl_info->egl_vendor_string);
    printf("\t\t\tExtensions string: %s\n", egl_info->egl_extensions_string);
    printf("\t\t\tGL info: %s\n", egl_info->struct_flag_gl_info == FILLED ? "FILLED" : "NOT FILLED");
    if (egl_info->struct_flag_gl_info == FILLED)
        print_info_egl_gl(egl_info->egl_gl_info);
    printf("\t\t\tGLES info: %s\n", egl_info->struct_flag_gles_info == FILLED ? "FILLED" : "NOT FILLED");
    if (egl_info->struct_flag_gles_info == FILLED)
        print_info_egl_gles(egl_info->egl_gles_info);
}

static void print_info_glx(struct glx_info *glx_info)
{
    printf("\t\t\tExtensions string: %s\n", glx_info->glx_extensions_string);
    printf("\t\t\t\tGL compatibility context:\n");
    print_info_gl(&glx_info->base_compatibility);
    if (glx_info->has_core) {
        printf("\t\t\t\tGL Core context:\n");
        print_info_gl(&glx_info->base_core);
    } else
        printf("\t\t\t\tNo Core context\n");
}

static void print_info_display_system(struct display_system_info *display_server_info)
{
    printf("\t\tEGL info: %s\n", display_server_info->struct_flag_egl_info == FILLED ? "FILLED" : "NOT FILLED");
    if (display_server_info->struct_flag_egl_info == FILLED)
        print_info_egl(display_server_info->egl_info);
    printf("\t\tGLX info: %s\n", display_server_info->struct_flag_glx_info == FILLED ? "FILLED" : "NOT FILLED");
    if (display_server_info->struct_flag_glx_info == FILLED)
        print_info_glx(display_server_info->glx_info);
}

static void print_info_gpu(struct gpu_info *gpu_info)
{
    if (gpu_info->id_path_tag)
        printf("\tid_path_tag: %s\n", gpu_info->id_path_tag);
    if (gpu_info->devnode)
        printf("\tdevnode: %s\n", gpu_info->devnode);

    printf("\tdrm platform: %s\n", gpu_info->struct_flag_display_system_info_drm == FILLED ? "FILLED" : "NOT FILLED");
    if (gpu_info->struct_flag_display_system_info_drm == FILLED)
        print_info_display_system(gpu_info->display_system_info_drm);

    printf("\tX platform: %s\n", gpu_info->struct_flag_display_system_info_x == FILLED ? "FILLED" : "NOT FILLED");
    if (gpu_info->struct_flag_display_system_info_x == FILLED)
        print_info_display_system(gpu_info->display_system_info_x);

    printf("\twayland platform: %s\n", gpu_info->struct_flag_display_system_info_wayland == FILLED ? "FILLED" : "NOT FILLED");
    if (gpu_info->struct_flag_display_system_info_wayland == FILLED)
        print_info_display_system(gpu_info->display_system_info_wayland);
}

static void print_info_system(struct system_info *system_info)
{
    int i;

    printf("%d gpus were detected\n", system_info->num_gpus);
    for (i=0; i<system_info->num_gpus; i++) {
        printf("gpu num: %d\n", i);
        print_info_gpu(system_info->gpu_info[i]);
    }
}

void print_info()
{
    struct system_info *system_info = create_and_fill_system_info();
    print_info_system(system_info);
    free_system_info(system_info);
}
