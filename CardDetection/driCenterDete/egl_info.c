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

#define EGL_EGLEXT_PROTOTYPES


#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdlib.h>
#include <string.h>

#include "../include/misc.h"
#include "../include/display_system.h"
#include "../include/egl_info.h"

static void free_egl_gles_info(struct display_system_info *display_system_info)
{
    struct egl_info *egl_info = display_system_info->egl_info;
    struct egl_gles_info *egl_gles_info = egl_info->egl_gles_info;

    if (egl_info->struct_flag_gles_info != FILLED)
        return;

    if (egl_gles_info->has_gles1)
        free_gl_info(&egl_gles_info->base_gles1);
    if (egl_gles_info->has_gles2)
        free_gl_info(&egl_gles_info->base_gles2);
    if (egl_gles_info->has_gles3)
        free_gl_info(&egl_gles_info->base_gles3);
    free(egl_info->egl_gles_info);
    egl_info->egl_gles_info = NULL;
    egl_info->struct_flag_gles_info = NOT_FILLED_YET;
}

static void free_egl_gl_info(struct display_system_info *display_system_info)
{
    struct egl_info *egl_info = display_system_info->egl_info;

    if (egl_info->struct_flag_gl_info != FILLED)
        return;

    free_gl_info(&egl_info->egl_gl_info->base_compatibility);
    if (egl_info->egl_gl_info->has_core)
        free_gl_info(&egl_info->egl_gl_info->base_core);
    free(egl_info->egl_gl_info);
    egl_info->egl_gl_info = NULL;
    egl_info->struct_flag_gl_info = NOT_FILLED_YET;
}

void free_egl_info(struct display_system_info *display_system_info)
{
    struct egl_info *egl_info = display_system_info->egl_info;
    if (display_system_info->struct_flag_egl_info != FILLED)
        return;

    free_egl_gl_info(display_system_info);
    free_egl_gles_info(display_system_info);
    /*(void) eglTerminate(display_system_info->egl_info->dpy); crashes because of a bug in Mesa */
    free(egl_info->client_apis_string);
    free(egl_info->egl_extensions_string);
    free(egl_info->egl_vendor_string);
    free(egl_info->egl_version_string);
    free(egl_info->egl_configs);
    free(egl_info);
    display_system_info->egl_info = NULL;
    display_system_info->struct_flag_egl_info = NOT_FILLED_YET;
}

static void fill_egl_gles_info(struct display_system_info *display_system_info)
{
    struct egl_info *egl_info = display_system_info->egl_info;
    struct egl_gles_info *egl_gles_info;
    EGLint config_attribs_es1[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
        EGL_NONE
    };
    EGLint config_attribs_es2[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLint config_attribs_es3[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_NONE
    };
    EGLint context_es1_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 1,
        EGL_CONTEXT_MINOR_VERSION_KHR, 0,
        EGL_NONE
    };
    EGLint context_es2_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_CONTEXT_MINOR_VERSION_KHR, 0,
        EGL_NONE
    };
    EGLint context_es3_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION_KHR, 0,
        EGL_NONE
    };
    EGLConfig temp_config;
    EGLint i;
    EGLDisplay dpy = egl_info->dpy;
    bool has_gles1=FALSE, has_gles2=FALSE, has_gles3=FALSE;
    bool fail = FALSE;

    if (!strstr(egl_info->egl_extensions_string, "EGL_KHR_create_context") ||
        !strstr(egl_info->egl_extensions_string, "EGL_KHR_surfaceless_context")) {
        egl_info->struct_flag_gl_info = NOT_APPLICABLE;
        return;
    }

    egl_info->egl_gles_info = (struct egl_gles_info*) calloc(1, sizeof(struct egl_gles_info));
    if (!egl_info->egl_gles_info) {
        egl_info->struct_flag_gles_info = MEMORY_FAILURE;
        return;
    }

    egl_gles_info = egl_info->egl_gles_info;
    eglBindAPI(EGL_OPENGL_ES_API);

    if (strstr(egl_info->client_apis_string, "OpenGL_ES"))
        has_gles1 = TRUE;

    if (strstr(egl_info->client_apis_string, "OpenGL_ES2"))
        has_gles2 = TRUE;

    if (strstr(egl_info->client_apis_string, "OpenGL_ES3"))
        has_gles3 = TRUE;

    if (has_gles1) {
        fail = FALSE;
        if (!eglChooseConfig(dpy, config_attribs_es1, &temp_config, 1, &i))
            fail = TRUE;
        if (!fail)
            egl_gles_info->context_gles1 = eglCreateContext(dpy, temp_config, EGL_NO_CONTEXT, context_es1_attribs);
        if (!fail && egl_gles_info->context_gles1 == EGL_NO_CONTEXT)
            fail = TRUE;

        if (!fail)
            if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_gles_info->context_gles1) == EGL_FALSE)
                fail = TRUE;
        if (!fail)
            if (!fill_gl_info(&egl_gles_info->base_gles1, 0, 1))
                fail = TRUE;

        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (fail)
            has_gles1 = FALSE;
        else
            egl_gles_info->has_gles1 = TRUE;
    }

    if (has_gles2) {
        fail = FALSE;
        if (!eglChooseConfig(dpy, config_attribs_es2, &temp_config, 1, &i))
            fail = TRUE;
        if (!fail)
            egl_gles_info->context_gles2 = eglCreateContext(dpy, temp_config, EGL_NO_CONTEXT, context_es2_attribs);
        if (!fail && egl_gles_info->context_gles2 == EGL_NO_CONTEXT)
            fail = TRUE;

        if (!fail)
            if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_gles_info->context_gles2) == EGL_FALSE)
                fail = TRUE;
        if (!fail)
            if (!fill_gl_info(&egl_gles_info->base_gles2, 0, 1))
                fail = TRUE;

        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (fail)
            has_gles2 = FALSE;
        else
            egl_gles_info->has_gles2 = TRUE;
    }

    if (has_gles3) {
        fail = FALSE;
        if (!eglChooseConfig(dpy, config_attribs_es3, &temp_config, 1, &i))
            fail = TRUE;
        if (!fail)
            egl_gles_info->context_gles3 = eglCreateContext(dpy, temp_config, EGL_NO_CONTEXT, context_es3_attribs);
        if (!fail && egl_gles_info->context_gles3 == EGL_NO_CONTEXT)
            fail = TRUE;

        if (!fail)
            if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_gles_info->context_gles3) == EGL_FALSE)
                fail = TRUE;
        if (!fail)
            if (!fill_gl_info(&egl_gles_info->base_gles3, 0, 1))
                fail = TRUE;

        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (fail)
            has_gles3 = FALSE;
        else
            egl_gles_info->has_gles3 = TRUE;
    }

    if (!has_gles1 && !has_gles2 && !has_gles3) {
        egl_info->struct_flag_gles_info = FAILURE;
        free(egl_gles_info);
    } else
        egl_info->struct_flag_gles_info = FILLED;
}

static char * egl_gles_get_gl_renderer(struct egl_gles_info *egl_gles_info)
{
    if (egl_gles_info->has_gles3)
        return gl_get_renderer(&egl_gles_info->base_gles3);
    if (egl_gles_info->has_gles2)
        return gl_get_renderer(&egl_gles_info->base_gles2);
    if (egl_gles_info->has_gles1)
        return gl_get_renderer(&egl_gles_info->base_gles1);
    return NULL;
}

static void fill_egl_gl_info(struct display_system_info *display_system_info)
{
    struct egl_info *egl_info = display_system_info->egl_info;
    EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };
    EGLint context_compatibility_attribs[] = {
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR,
        EGL_NONE
    };
    /* Note: One would expect that to create a core context, the following attributes would be sufficient:
     * EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR
     * But if you do that you'll get a compatibility context... because by default
     * you ask for OpenGL version 1.0.
     * However the spec says if you ask for OpenGL 3.1, you'll get a core context.
     */
    EGLint context_core_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION_KHR, 1,
        EGL_NONE
    };
    EGLConfig config;
    EGLint i;
    EGLDisplay dpy = egl_info->dpy;

    /* We don't support old drivers not supporting the following extensions:
     * They are required to get a core context, and to create a context without
     * creating a surface for the display system we use.
     */
    if (!strstr(egl_info->egl_extensions_string, "EGL_KHR_create_context") ||
        !strstr(egl_info->egl_extensions_string, "EGL_KHR_surfaceless_context")) {
        egl_info->struct_flag_gl_info = NOT_APPLICABLE;
        return;
    }

    if (!strstr(egl_info->client_apis_string, "OpenGL")) {
        egl_info->struct_flag_gl_info = NOT_APPLICABLE;
        return;
    }


    egl_info->egl_gl_info = (struct egl_gl_info*) calloc(1, sizeof(struct egl_gl_info));
    if (!egl_info->egl_gl_info) {
        egl_info->struct_flag_gl_info = MEMORY_FAILURE;
        return;
    }

    if (!eglChooseConfig(dpy, config_attribs, &config, 1, &i))
        goto fail;

    eglBindAPI(EGL_OPENGL_API);
    egl_info->egl_gl_info->context_compatibility = eglCreateContext(dpy, config, EGL_NO_CONTEXT, context_compatibility_attribs);
    if (egl_info->egl_gl_info->context_compatibility == EGL_NO_CONTEXT)
        goto fail;

    egl_info->egl_gl_info->context_core = eglCreateContext(dpy, config, EGL_NO_CONTEXT, context_core_attribs);
    if (egl_info->egl_gl_info->context_core == EGL_NO_CONTEXT)
        egl_info->egl_gl_info->has_core = FALSE;
    else
        egl_info->egl_gl_info->has_core = TRUE;

    if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_info->egl_gl_info->context_compatibility) == EGL_FALSE)
        goto fail;

    if (!fill_gl_info(&egl_info->egl_gl_info->base_compatibility, 0, 0))
        goto fail_context;

    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (egl_info->egl_gl_info->has_core) {
        if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_info->egl_gl_info->context_core) == EGL_FALSE)
            goto fail;

        if (!fill_gl_info(&egl_info->egl_gl_info->base_core, 1, 0))
            goto fail_context;

        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    egl_info->struct_flag_gl_info = FILLED;
    return;

fail_context:
    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
fail:
    egl_info->struct_flag_gl_info = FAILURE;
    free(egl_info->egl_gl_info);
}

static char * egl_gl_get_gl_renderer(struct egl_gl_info *egl_gl_info)
{
    return gl_get_renderer(&egl_gl_info->base_compatibility);
}

bool egl_gl_info_similar(struct egl_gl_info *first, struct egl_gl_info *second)
{
    if (gl_info_similar(&first->base_compatibility, &second->base_compatibility) &&
        first->has_core == second->has_core &&
        (!first->has_core || gl_info_similar(&first->base_core, &second->base_core)))
        return TRUE;
    return FALSE;
}

/* These extensions and the symbols associated are only advertised since Mesa 10.2
 * The reason we have to use these, is that but creating the display a standard way,
 * we'll crash if we create a display on a different platform than the previous
 * display we created. This bug could be solved and get into Mesa 10.3, but
 * better rely on something working since Mesa 10.2 */
static bool is_platform_supported(struct display_system_info *display_system_info)
{
    if (display_system_info->display_system_type_info == DRM_DISPLAY_SERVER)
        return strstr(eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS), "EGL_MESA_platform_gbm");
    if (display_system_info->display_system_type_info == X_DISPLAY_SERVER)
        return strstr(eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS), "EGL_EXT_platform_x11");
    if (display_system_info->display_system_type_info == WAYLAND_DISPLAY_SERVER)
        return strstr(eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS), "EGL_EXT_platform_wayland");
    return FALSE;
}

static EGLDisplay get_display_for_platform(struct display_system_info *display_system_info, void *display)
{
    if (display_system_info->display_system_type_info == DRM_DISPLAY_SERVER)
        return (EGLDisplay) eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_MESA, display, NULL);
    if (display_system_info->display_system_type_info == X_DISPLAY_SERVER)
        return (EGLDisplay) eglGetPlatformDisplayEXT(EGL_PLATFORM_X11_EXT, display, NULL);
    if (display_system_info->display_system_type_info == WAYLAND_DISPLAY_SERVER)
        return (EGLDisplay) eglGetPlatformDisplayEXT(EGL_PLATFORM_WAYLAND_EXT, display, NULL);
    return EGL_NO_DISPLAY;
}

void fill_egl_info(struct display_system_info *display_system_info)
{
    void *display;
    const char *temp_string;
    struct egl_info *egl_info;

    if (!is_platform_supported(display_system_info) ||
        !get_display_var(display_system_info, &display)) {
        display_system_info->struct_flag_egl_info = NOT_APPLICABLE;
        return;
    }

    display_system_info->egl_info = (struct egl_info*) calloc(1, sizeof(struct egl_info));
    if (display_system_info->egl_info == NULL) {
        display_system_info->struct_flag_egl_info = MEMORY_FAILURE;
        return;
    }
    egl_info = display_system_info->egl_info;

    egl_info->dpy = get_display_for_platform(display_system_info, display);
    if (egl_info->dpy == NULL)
        goto dpy_failure;

    if (eglInitialize(egl_info->dpy, &egl_info->major, &egl_info->minor) != EGL_TRUE)
        goto egl_failure;

    temp_string = eglQueryString(egl_info->dpy, EGL_CLIENT_APIS);
    if (!temp_string)
        goto egl_failure;
    egl_info->client_apis_string = strdup(temp_string);
    if (!egl_info->client_apis_string) {
        display_system_info->struct_flag_egl_info = MEMORY_FAILURE;
        goto egl_failure;
    }

    temp_string = eglQueryString(egl_info->dpy, EGL_EXTENSIONS);
    if (!temp_string)
        goto egl_failure;
    egl_info->egl_extensions_string = strdup(temp_string);
    if (!egl_info->egl_extensions_string) {
        display_system_info->struct_flag_egl_info = MEMORY_FAILURE;
        goto egl_failure;
    }

    temp_string = eglQueryString(egl_info->dpy, EGL_VENDOR);
    if (!temp_string)
        goto egl_failure;
    egl_info->egl_vendor_string = strdup(temp_string);
    if (!egl_info->egl_vendor_string) {
        display_system_info->struct_flag_egl_info = MEMORY_FAILURE;
        goto egl_failure;
    }

    temp_string = eglQueryString(egl_info->dpy, EGL_VERSION);
    if (!temp_string)
        goto egl_failure;
    egl_info->egl_version_string = strdup(temp_string);
    if (!egl_info->egl_version_string) {
        display_system_info->struct_flag_egl_info = MEMORY_FAILURE;
        goto egl_failure;
    }

    if (eglGetConfigs(egl_info->dpy, NULL, 0, &egl_info->num_configs) != EGL_TRUE ||
        egl_info->num_configs <= 0)
        goto egl_failure;
    egl_info->egl_configs = calloc(egl_info->num_configs, sizeof(EGLConfig));
    if (egl_info->egl_configs == NULL) {
        display_system_info->struct_flag_egl_info = MEMORY_FAILURE;
        goto egl_failure;
    }
    if (eglGetConfigs(egl_info->dpy, egl_info->egl_configs, egl_info->num_configs, &egl_info->num_configs) != EGL_TRUE)
        goto egl_failure;

    display_system_info->struct_flag_egl_info = FILLED;
    fill_egl_gles_info(display_system_info);
    fill_egl_gl_info(display_system_info);
    return;

egl_failure:
    (void) eglTerminate(egl_info->dpy); /* This does not free the display, but destroys everything linked to it */
dpy_failure:
    free(display_system_info->egl_info);
    display_system_info->egl_info = NULL;
    if (display_system_info->struct_flag_egl_info != MEMORY_FAILURE)
        display_system_info->struct_flag_egl_info = FAILURE;
    return;
}

char *egl_get_gl_renderer(struct egl_info *egl_info)
{
    char *ret = NULL;
    if (egl_info->struct_flag_gl_info == FILLED) {
        ret = egl_gl_get_gl_renderer(egl_info->egl_gl_info);
        if (ret)
            return ret;
    }
    if (egl_info->struct_flag_gles_info == FILLED) {
        ret = egl_gles_get_gl_renderer(egl_info->egl_gles_info);
        if (ret)
            return ret;
    }
    return NULL;
}

bool egl_info_similar(struct egl_info *first, struct egl_info *second)
{
    if (first->major == second->major && first->minor == second->minor &&
        !strcmp(first->client_apis_string, second->client_apis_string) &&
        !strcmp(first->egl_extensions_string, second->egl_extensions_string) &&
        !strcmp(first->egl_vendor_string, second->egl_vendor_string) &&
        !strcmp(first->egl_version_string, second->egl_version_string) &&
        first->struct_flag_gl_info == second->struct_flag_gl_info &&
        (first->struct_flag_gl_info != FILLED || egl_gl_info_similar(first->egl_gl_info, second->egl_gl_info)))
        return TRUE;
    return FALSE;
}
