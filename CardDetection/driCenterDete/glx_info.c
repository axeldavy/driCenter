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

#define GLX_GLXEXT_PROTOTYPES

#include <GL/glx.h>
#include <GL/glxext.h>
#include <stdlib.h>
#include <string.h>

#include "../include/misc.h"
#include "../include/glx_info.h"
#include "../include/gl_info.h"
#include "../include/display_system.h"

void free_glx_info(struct display_system_info *display_system_info)
{
    struct glx_info *glx_info = display_system_info->glx_info;

    if (display_system_info->struct_flag_glx_info != FILLED)
        return;

    free_gl_info(&glx_info->base_compatibility);
    if (glx_info->has_core)
        free_gl_info(&glx_info->base_core);
    free(glx_info);
    display_system_info->struct_flag_glx_info = NOT_FILLED_YET;
}

/* Copied from glxinfo */
static GLXFBConfig *
choose_fb_config(Display *dpy, int scrnum)
{
   int fbAttribSingle[] = {
      GLX_RENDER_TYPE,   GLX_RGBA_BIT,
      GLX_RED_SIZE,      1,
      GLX_GREEN_SIZE,    1,
      GLX_BLUE_SIZE,     1,
      GLX_DOUBLEBUFFER,  False,
      None };
   int fbAttribDouble[] = {
      GLX_RENDER_TYPE,   GLX_RGBA_BIT,
      GLX_RED_SIZE,      1,
      GLX_GREEN_SIZE,    1,
      GLX_BLUE_SIZE,     1,
      GLX_DOUBLEBUFFER,  True,
      None };
   GLXFBConfig *configs;
   int nConfigs;

   configs = glXChooseFBConfig(dpy, scrnum, fbAttribSingle, &nConfigs);
   if (!configs)
      configs = glXChooseFBConfig(dpy, scrnum, fbAttribDouble, &nConfigs);

   return configs;
}

/* Copied from glxinfo */
static const struct { int major, minor; } gl_versions[] = {
   {1, 0},
   {1, 1},
   {1, 2},
   {1, 3},
   {1, 4},
   {1, 5},
   {2, 0},
   {2, 1},
   {3, 0},
   {3, 1},
   {3, 2},
   {3, 3},
   {4, 0},
   {4, 1},
   {4, 2},
   {4, 3},
   {0, 0} /* end of list */
};

#define ELEMENTS(array) (sizeof(array) / sizeof(array[0]))
#define NUM_GL_VERSIONS ELEMENTS(gl_versions)

/* Copied from glxinfo */

static Bool CreateContextErrorFlag;

static int
create_context_error_handler(Display *dpy, XErrorEvent *error)
{
   (void) dpy;
   (void) error->error_code;
   CreateContextErrorFlag = True;
   return 0;
}

/* Copied from glxinfo */
static int
extension_supported(const char *ext, const char *extensionsList)
{
   const char *p = strstr(extensionsList, ext);
   if (p) {
      /* check that next char is a space or end of string */
      int extLen = strlen(ext);
      if (p[extLen] == 0 || p[extLen] == ' ')
         return 1;
   }
   return 0;
}

/* Copied from glxinfo */
static GLXContext
create_context_flags(Display *dpy, GLXFBConfig fbconfig, int major, int minor,
                     int contextFlags, int profileMask, Bool direct)
{
#ifdef GLX_ARB_create_context
   static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB_func = 0;
   static Bool firstCall = True;
   int (*old_handler)(Display *, XErrorEvent *);
   GLXContext context;
   int attribs[20];
   int n = 0;

   if (firstCall) {
      /* See if we have GLX_ARB_create_context_profile and get pointer to
       * glXCreateContextAttribsARB() function.
       */
      const char *glxExt = glXQueryExtensionsString(dpy, 0);
      if (extension_supported("GLX_ARB_create_context_profile", glxExt)) {
         glXCreateContextAttribsARB_func = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
            glXGetProcAddress((const GLubyte *) "glXCreateContextAttribsARB");
      }
      firstCall = False;
   }

   if (!glXCreateContextAttribsARB_func)
      return 0;

   /* setup attribute array */
   if (major) {
      attribs[n++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
      attribs[n++] = major;
      attribs[n++] = GLX_CONTEXT_MINOR_VERSION_ARB;
      attribs[n++] = minor;
   }
   if (contextFlags) {
      attribs[n++] = GLX_CONTEXT_FLAGS_ARB;
      attribs[n++] = contextFlags;
   }
#ifdef GLX_ARB_create_context_profile
   if (profileMask) {
      attribs[n++] = GLX_CONTEXT_PROFILE_MASK_ARB;
      attribs[n++] = profileMask;
   }
#endif
   attribs[n++] = 0;

   /* install X error handler */
   old_handler = XSetErrorHandler(create_context_error_handler);
   CreateContextErrorFlag = False;

   /* try creating context */
   context = glXCreateContextAttribsARB_func(dpy,
                                             fbconfig,
                                             0, /* share_context */
                                             direct,
                                             attribs);

   /* restore error handler */
   XSetErrorHandler(old_handler);

   if (CreateContextErrorFlag)
      context = 0;

   if (direct) {
      if (!glXIsDirect(dpy, context)) {
         glXDestroyContext(dpy, context);
         return 0;
      }
   }

   return context;
#else
   return 0;
#endif
}

/* Copied from glxinfo */
static GLXContext
create_context_with_config(Display *dpy, GLXFBConfig config,
                           Bool coreProfile, Bool direct)
{
   GLXContext ctx = 0;

   if (coreProfile) {
      /* Try to create a core profile, starting with the newest version of
       * GL that we're aware of.  If we don't specify the version
       */
      int i;
      for (i = NUM_GL_VERSIONS - 2; i > 0 ; i--) {
          /* don't bother below GL 3.0 */
          if (gl_versions[i].major == 3 &&
              gl_versions[i].minor == 0)
             return 0;
         ctx = create_context_flags(dpy, config,
                                    gl_versions[i].major,
                                    gl_versions[i].minor,
                                    0x0,
                                    GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                                    direct);
         if (ctx)
            return ctx;
      }
      /* couldn't get core profile context */
      return 0;
   }

   /* GLX should return a context of the latest GL version that supports
    * the full profile.
    */
   ctx = glXCreateNewContext(dpy, config, GLX_RGBA_TYPE, NULL, direct);

   /* make sure the context is direct, if direct was requested */
   if (ctx && direct) {
      if (!glXIsDirect(dpy, ctx)) {
         glXDestroyContext(dpy, ctx);
         return 0;
      }
   }

   return ctx;
}

/* Adapted from glxinfo */
void fill_glx_info(struct display_system_info *display_system_info)
{
    Display *dpy;
    struct glx_info *glx_info;
    Window win;
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    GLXContext ctx = NULL;
    XVisualInfo *visinfo;
    GLXFBConfig *fbconfigs;

    if (display_system_info->display_system_type_info != X_DISPLAY_SERVER ||
        !get_display_var(display_system_info, (void **) &dpy)){
        display_system_info->struct_flag_glx_info = NOT_APPLICABLE;
        return;
    }

    display_system_info->glx_info = (struct glx_info*) calloc(1, sizeof(struct glx_info));
    if (display_system_info->glx_info == NULL) {
        display_system_info->struct_flag_glx_info = MEMORY_FAILURE;
        return;
    }
    glx_info = display_system_info->glx_info;
    glx_info->dpy = dpy;
    display_system_info->struct_flag_glx_info = FAILURE;
    glx_info->glx_extensions_string = strdup(glXQueryExtensionsString(dpy, 0));

    root = RootWindow(dpy, 0);

    fbconfigs = choose_fb_config(dpy, 0);
    if (!fbconfigs)
        goto clean_fbconfigs;

    visinfo = glXGetVisualFromFBConfig(dpy, fbconfigs[0]);
    if (!visinfo)
        goto clean_visinfo;

    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask;
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
    win = XCreateWindow(dpy, root, 0, 0, 100, 100,
                        0, visinfo->depth, InputOutput,
                        visinfo->visual, mask, &attr);

    if (!win)
        goto clean_win;

    ctx = create_context_with_config(dpy, fbconfigs[0],
                                         FALSE, TRUE);
    if (!ctx)
        goto clean_ctx;

    if (!glXMakeCurrent(dpy, win, ctx))
        goto clean_makecurrent;

    if (!fill_gl_info(&glx_info->base_compatibility, FALSE, FALSE))
        goto clean_fill_gl_info;

    display_system_info->struct_flag_glx_info = FILLED;
    glXMakeCurrent(dpy, 0, 0);
    glXDestroyContext(dpy, ctx);

    ctx = create_context_with_config(dpy, fbconfigs[0],
                                         TRUE, TRUE);
    if (ctx) {
        if (!glXMakeCurrent(dpy, win, ctx))
            goto clean_makecurrent;

        if (!fill_gl_info(&glx_info->base_core, TRUE, FALSE))
            goto clean_fill_gl_info;

        glx_info->has_core = TRUE;
        glXMakeCurrent(dpy, 0, 0);
        glXDestroyContext(dpy, ctx);
    }
    goto clean_ctx;

clean_fill_gl_info:
clean_makecurrent:
    glXDestroyContext(dpy, ctx);
clean_ctx:
    XDestroyWindow(dpy, win);
clean_win:
    XFree(visinfo);
clean_visinfo:
    XFree(fbconfigs);
clean_fbconfigs:
    XSync(dpy, 1);
    if (display_system_info->struct_flag_glx_info == FAILURE) {
        free(glx_info->glx_extensions_string);
        free(display_system_info->glx_info);
    }
}

char *glx_get_gl_renderer(struct glx_info *glx_info)
{
    return gl_get_renderer(&glx_info->base_compatibility);
}

bool glx_info_similar(struct glx_info *first, struct glx_info *second)
{
    if (first->major == second->major && first->minor == second->minor &&
        !strcmp(first->glx_extensions_string, second->glx_extensions_string) &&
 //       !strcmp(first->glx_vendor_string, second->glx_vendor_string) &&
 //       !strcmp(first->glx_version_string, second->glx_version_string) &&
        gl_info_similar(&first->base_compatibility, &second->base_compatibility))
        return TRUE;
    return FALSE;
}
