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

#include <libudev.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "../include/misc.h"
#include "../include/gpu_info.h"
#include "../include/system.h"

struct device_info {
    struct {
        char *devnode;
        struct udev_device *device;
    } render_node, node;
    bool has_render_node;
    bool render_node_has_id_path_tag;
    bool has_node;
    char *id_path_tag;
    char *dev_path_prefix; 
    struct device_info *next;
};

#define free_checked(a) if(a) free(a);
static void clean_device_info(struct device_info *device_info)
{
    struct device_info *next;

    while (device_info != NULL) {
        next = device_info->next;
        if (device_info->has_render_node) {
            free_checked(device_info->render_node.devnode);
            if (device_info->render_node.device)
                udev_device_unref(device_info->render_node.device);
        }
        if (device_info->has_node) {
            free_checked(device_info->node.devnode);
            if (device_info->node.device)
                udev_device_unref(device_info->node.device);
        }
        free_checked(device_info->id_path_tag);
        free_checked(device_info->dev_path_prefix);
        free(device_info);
        device_info = next;
    }
}

static char *get_dev_path_prefix(const char *devpath)
{
    int last_slach_pos = -1;
    int i;

    for(i=0; devpath[i] != '\0'; i++) {
        if (devpath[i] == '/')
            last_slach_pos = i;
    }

    return strndup(devpath, last_slach_pos + 1);
}


// TODO: better memory handling
static bool add_device(struct device_info **parent, bool is_render_node, struct udev_device *device, const char *id_path_tag, const char *dev_path)
{
    struct device_info *current = *parent;
    struct device_info *parent_init = *parent;
    int samedevice;

    while (current != NULL) {
        if (current->id_path_tag && id_path_tag)
            samedevice = strcmp(current->id_path_tag, id_path_tag) == 0;
        else
            samedevice = strncmp(current->dev_path_prefix, dev_path, strlen(current->dev_path_prefix)) == 0;
        if (samedevice)
            break;
        current = current->next;
    }

    if (current != NULL) {
        if (is_render_node) {
            if (current->has_render_node) { /* Already present ? */
                udev_device_unref(device);
                return TRUE;
            } else {
                current->render_node.devnode = strdup(udev_device_get_devnode(device));
                current->render_node.device = device;
                current->render_node.device = device;
                current->has_render_node = TRUE;
                current->render_node_has_id_path_tag = (id_path_tag != NULL);
                return TRUE;
            }     
        } else {
            if (current->has_node) { /* Already present ? */
                udev_device_unref(device);
                return TRUE;
            } else {
                current->node.devnode = strdup(udev_device_get_devnode(device));
                current->node.device = device;
                current->has_node = TRUE;
                return TRUE;
            }
        }
    }

    current = (struct device_info *) calloc(1, sizeof(struct device_info));
    if (current == NULL)
        return FALSE;

    if (is_render_node) {
        current->render_node.devnode = strdup(udev_device_get_devnode(device));
        current->render_node.device = device;
        current->has_render_node = TRUE;
        current->render_node_has_id_path_tag = (id_path_tag != NULL);
    } else {
        current->node.devnode = strdup(udev_device_get_devnode(device));
        current->node.device = device;
        current->has_node = TRUE;
    }

    if (id_path_tag) {
        current->id_path_tag = strdup(id_path_tag);
        if (current->id_path_tag == NULL)
            return FALSE;
    }

    current->dev_path_prefix = get_dev_path_prefix(dev_path);
    if (current->dev_path_prefix == NULL)
        return FALSE;
    current->next = parent_init;
    *parent = current;
    return TRUE;
}

/* analyses /dev/dri */

/*void find_devices_dri()
{
    
    
}*/

/* use udev to get gpus */

static bool find_devices_udev(struct device_info **parent)
{
    struct udev *udev;
    struct udev_device *device;
    struct udev_enumerate *e;
    struct udev_list_entry *entry;
    const char *path, *id_path_tag_tmp, *dev_path;
    /* find render-nodes */
    udev = udev_new();
    e = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(e, "drm");
    udev_enumerate_add_match_sysname(e, "render*");
    udev_enumerate_scan_devices(e);

    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e)) {
        path = udev_list_entry_get_name(entry);
        device = udev_device_new_from_syspath(udev, path);
        if (!device)
            continue;
        id_path_tag_tmp = udev_device_get_property_value(device, "ID_PATH_TAG");
        dev_path = udev_device_get_property_value(device, "DEVPATH");
        if (!dev_path) { /* it can be ok to have no id_path_tag, but we need a fallback */
            udev_device_unref(device);
            continue;
        }
        if (!add_device(parent, TRUE, device, id_path_tag_tmp, dev_path))
            goto fail;
    }

    udev_enumerate_unref(e);
    e = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(e, "drm");
    udev_enumerate_add_match_sysname(e, "card*");
    udev_enumerate_scan_devices(e);

    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e)) {
        path = udev_list_entry_get_name(entry);
        device = udev_device_new_from_syspath(udev, path);
        if (!device)
            continue;
        id_path_tag_tmp = udev_device_get_property_value(device, "ID_PATH_TAG");
        dev_path = udev_device_get_property_value(device, "DEVPATH");
        if (!dev_path) { /* it can be ok to have no id_path_tag, but we need a fallback */
            udev_device_unref(device);
            continue;
        }
        if (!add_device(parent, FALSE, device, id_path_tag_tmp, dev_path))
            goto fail;
    }
    udev_enumerate_unref(e);
    return TRUE;

fail:
    assert (FALSE); //TODO
}

static bool create_devices(struct system_info *system_info) {
    struct device_info *parent = NULL, *current;
    int num_render_nodes = 0, num_nodes = 0, num_gpus = 0;
    int i;
    bool has_id_path_tag = TRUE;
    bool render_node_has_id_path_tag = TRUE;
    struct gpu_info *default_gpu = NULL;
    struct gpu_info **gpu_info_array = NULL;
    int gpu_free_index = 0;
    char *init_dri_prime = NULL;

    if (getenv("DRI_PRIME"))
        init_dri_prime = strdup(getenv("DRI_PRIME"));

    if (!find_devices_udev(&parent) || (parent == NULL)) {
        printf("[Warning] Detection via udev failed.\n");
        goto failed_detection;
    }

    current = parent;
    while (current != NULL) {
        num_gpus++;
        if (current->has_render_node)
            num_render_nodes++;
        if (current->has_node)
            num_render_nodes++;
        if (current->id_path_tag == NULL)
            has_id_path_tag = FALSE;
        if (!current->render_node_has_id_path_tag)
            render_node_has_id_path_tag = FALSE;
        current = current->next;
    }

    if (num_nodes > num_render_nodes && num_gpus >= 2) {
        printf("[Warning] Some of detected gpus have no render-nodes. You may not be able to use all your gpus\n");
    }

    if (!render_node_has_id_path_tag && num_render_nodes >= 2) {
        printf("[Warning] You have render-nodes, but some of them have no ID_PATH_TAG advertised. You might not be able to use all your gpus.\n");
    }

    if (num_gpus >= 2)
    {
        /* fill non-drm gpus with DRI_PRIME set, either num or id_path_tag when available
        *  fill drm with the render node we have
        *  Clear/Merge later */
        gpu_info_array = (struct gpu_info **) calloc(3 * num_gpus, sizeof(struct gpu_info*));
        for (i = 0; i < num_gpus; i++) {
            char *temp_string = calloc(1, 10);
            sprintf(temp_string, "%d", i);
            setenv("DRI_PRIME", temp_string, 1);
            gpu_info_array[gpu_free_index++] = fill_gpu_display_info(NULL, NULL, NULL, FALSE, TRUE);
            free(temp_string);
        }
        current = parent;
        while (current != NULL) {
            if (current->has_render_node && current->render_node_has_id_path_tag) {
                setenv("DRI_PRIME", current->id_path_tag, 1);
                gpu_info_array[gpu_free_index++] = fill_gpu_display_info(current->render_node.devnode, current->id_path_tag, current->render_node.device, TRUE, TRUE);
                current->render_node.devnode = NULL;
                current->render_node.device = NULL;
                current->id_path_tag = NULL;
            } else if (current->has_render_node) {
                gpu_info_array[gpu_free_index++] = fill_gpu_display_info(current->render_node.devnode, NULL, NULL, TRUE, FALSE);
                current->render_node.devnode = NULL;
            }
            current = current->next;
        }
        clean_device_info(parent);
        setenv("DRI_PRIME", "0", 1);
        default_gpu = fill_gpu_display_info(NULL, NULL, NULL, FALSE, TRUE);
        num_gpus = gpu_info_list_clean(default_gpu, gpu_info_array, 3 * num_gpus);
        if (init_dri_prime)
            setenv("DRI_PRIME", init_dri_prime, 1);
        else
            unsetenv("DRI_PRIME");
        free_gpu_info(default_gpu);
        free_checked(init_dri_prime);
        system_info->gpu_info = gpu_info_array;
        system_info->struct_flag_gpu_info = FILLED;
        system_info->num_gpus = num_gpus;
        return TRUE;
    }
    
failed_detection:

    gpu_info_array = (struct gpu_info **) calloc(1, sizeof(struct gpu_info*));

    if (parent && parent->has_render_node) {
        gpu_info_array[0] = fill_gpu_display_info(parent->render_node.devnode, parent->id_path_tag, parent->render_node.device, TRUE, TRUE);
        parent->render_node.devnode = NULL;
        parent->render_node.device = NULL;
        parent->id_path_tag = NULL;
    } else {
        gpu_info_array[0] = fill_gpu_display_info(NULL, NULL, NULL, FALSE, TRUE);
    }
    clean_device_info(parent);
    free_checked(init_dri_prime);
    system_info->gpu_info = gpu_info_array;
    system_info->struct_flag_gpu_info = FILLED;
    system_info->num_gpus = 1;
    return TRUE;
}

void free_system_info(struct system_info *system_info)
{
    int i;

    for (i=0; i<system_info->num_gpus; i++)
        free_gpu_info(system_info->gpu_info[i]);
    free(system_info);
}

struct system_info * create_and_fill_system_info()
{
    struct system_info *system_info;

    system_info = (struct system_info *) calloc(1, sizeof(struct system_info));
    if (!system_info)
        return NULL;

    if (!create_devices(system_info)) {
        free(system_info);
        return NULL;
    }
    return system_info;
}
