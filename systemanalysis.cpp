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

#include "systemanalysis.h"
#include <libudev.h>
#include <assert.h>

SystemAnalysis::SystemAnalysis()
    : QObject()
{
    struct udev *udev = udev_new();
    struct udev_device *device;
    struct udev_enumerate *e;
    struct udev_list_entry *entry;
    const char *path, *id_path_tag_tmp;
    bool found = false;

    assert(udev);

    // Detect available cards for prime
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
        if (id_path_tag_tmp) {
            found = true;
            this->addCard(strdup(id_path_tag_tmp));
        }
        udev_device_unref(device);
    }
    // If there is no render-nodes, or they don't have ID_PATH_TAG,
    // don't care about more than one gpu.
    if (!found){
        this->addCard("undefined");
    }
}

CardInfo* SystemAnalysis::getCard(int num)
{
    return m_cards[num];
}

int SystemAnalysis::getCardsNumber()
{
    return m_cards.count();
}

void SystemAnalysis::addCard(const QString &pci_id)
{
    CardInfo *ci = new CardInfo();
    ci->fillInfo(pci_id);
    m_cards << ci;
}
