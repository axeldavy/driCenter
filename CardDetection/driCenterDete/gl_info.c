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

/* glxinfo Copyright:
 * Copyright (C) 1999-2006  Brian Paul   All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "../include/misc.h"
#include "../include/gl_info.h"


void free_gl_info(struct gl_info *gl_info)
{
    free(gl_info->version);
    free(gl_info->renderer);
    free(gl_info->vendor);
    free(gl_info->extensions);
}


/**
 * Get list of OpenGL extensions using core profile's glGetStringi().
 * Adapted from glxinfo
 */
static char *
build_core_profile_extension_list(void)
{
   GLint i, n, totalLen;
   char *buffer;

   glGetIntegerv(GL_NUM_EXTENSIONS, &n);

   /* compute totalLen */
   totalLen = 0;
   for (i = 0; i < n; i++) {
      const char *ext = (const char *) glGetStringi(GL_EXTENSIONS, i);
      totalLen += strlen(ext) + 1; /* plus a space */
   }

   buffer = malloc(totalLen + 1);
   if (buffer) {
      int pos = 0;
      for (i = 0; i < n; i++) {
         const char *ext = (const char *) glGetStringi(GL_EXTENSIONS, i);
         strcpy(buffer + pos, ext);
         pos += strlen(ext);
         buffer[pos++] = ' ';
      }
      buffer[pos] = '\0';
   }
   return buffer;
}

#define is_num(c) (c >= '0' && c <= '9')

static char get_gles_version_num(const char *version)
{
    int len = strlen(version);
    int i;

    for (i=0; i<len-3; i++) {
        if (is_num(version[i]) && version[i+1] == '.' && is_num(version[i+2]))
            return (version[i] - '0') * 10 + (version[i+2] - '0');
    }
    return 0;
}

/* Adapted from glxinfo */
bool fill_gl_info(struct gl_info *gl_info, bool is_core, bool is_gles)
{
    const char *glVendor = (const char *) glGetString(GL_VENDOR);
    const char *glRenderer = (const char *) glGetString(GL_RENDERER);
    const char *glVersion = (const char *) glGetString(GL_VERSION);
    const char *shading_version;

    if (!glVersion || !glRenderer || !glVendor)
        return FALSE;
    gl_info->is_core = is_core;

    if (is_core)
        gl_info->extensions = build_core_profile_extension_list();
    else {
        if (!glGetString(GL_EXTENSIONS))
            return FALSE;
        gl_info->extensions = strdup((char*)glGetString(GL_EXTENSIONS));
    }
    if (!gl_info->extensions)
        return FALSE;

    gl_info->vendor = strdup(glVendor);
    if (!gl_info->vendor)
        goto free_extensions;

    gl_info->renderer = strdup(glRenderer);
    if (!gl_info->renderer)
        goto free_vendor;

    gl_info->version = strdup(glVersion);
    if (!gl_info->version)
        goto free_renderer;

    if (!is_gles)
        gl_info->version_num = (glVersion[0] - '0') * 10 + (glVersion[2] - '0');
    else
        gl_info->version_num = get_gles_version_num(glVersion);
    if (gl_info->version_num < 20)
        shading_version = "none";
    else
        shading_version = (char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (!shading_version)
        goto free_version;

    gl_info->shading_language_version = strdup(shading_version);
    if (!gl_info->shading_language_version)
        goto free_version;

    return TRUE;

free_version:
    free(gl_info->version);
free_renderer:
    free(gl_info->renderer);
free_vendor:
    free(gl_info->vendor);
free_extensions:    
    free(gl_info->extensions);
    return FALSE;
}

char *gl_get_renderer(struct gl_info *gl_info)
{
    return gl_info->renderer;
}

bool gl_info_similar(struct gl_info *first, struct gl_info *second)
{
    if (first->version_num == second->version_num &&
        !strcmp(first->vendor, second->vendor) &&
        !strcmp(first->renderer, second->renderer) &&
        !strcmp(first->version, second->version) &&
        !strcmp(first->extensions, second->extensions))
        return TRUE;
    return FALSE;
}
