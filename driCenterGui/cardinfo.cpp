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

#include <qmetatype.h>
#include <stdlib.h>

#include "cardinfo.h"
extern "C" {
#include "misc.h"
#include "system.h"
#include "gpu_info.h"
#include "display_system.h"
#include "egl_info.h"
#include "glx_info.h"
#include "gl_info.h"
#include "loader.h"
}

///////////////////////////////////////

CardModel::CardModel(QObject *parent)
    : QAbstractListModel(parent)
{
    system_info = create_and_fill_system_info();
}

CardModel::~CardModel(){

}

int CardModel::rowCount(const QModelIndex &parent) const
{
    (void) parent;
    return system_info->num_gpus;
}

int CardModel::numCards() const
{
    return system_info->num_gpus;
}

QVariant CardModel::GetTableForDisplay(struct display_system_info *display_info) const
{
    QMap<QString, QVariant> itemData;
    itemData["has_glx"] = (display_info->struct_flag_glx_info == FILLED);
    itemData["has_egl"] = (display_info->struct_flag_egl_info == FILLED);
    return QVariant(itemData);
}

QVariant CardModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= system_info->num_gpus)
        return QVariant();

    struct gpu_info *gpu = system_info->gpu_info[index.row()];

    if (role == NameRole) {
        char *renderer = get_gl_renderer(gpu);
        if (!renderer)
            return gpu->devnode;
        else
            return renderer;
    } else if (role == IDRole) {
        return gpu->id_path_tag;
    } else if (role == DriverNameRole) {
        if (gpu->struct_flag_display_system_info_drm != FILLED)
            return QVariant();
        int fd = gpu->display_system_info_drm->display_system.drm_info->fd;
        return QString(loader_get_driver_for_fd(fd, 0));
    } else if (role == DrmRole) {
        if (gpu->struct_flag_display_system_info_drm != FILLED)
            return QVariant(false);
        return GetTableForDisplay(gpu->display_system_info_drm);
    } else if (role == X11Role) {
        if (gpu->struct_flag_display_system_info_x != FILLED)
            return QVariant(false);
        return GetTableForDisplay(gpu->display_system_info_x);
    } else if (role == DrmRole) {
        if (gpu->struct_flag_display_system_info_wayland != FILLED)
            return QVariant(false);
        return GetTableForDisplay(gpu->display_system_info_wayland);
    }
    return QVariant();
}

QVariant CardModel::get(int row)
{
    QModelIndex index = this->createIndex(row, 0);
    QMap<QString, QVariant> itemData;
    QHashIterator<int, QByteArray> hashItr(this->roleNames());
    while(hashItr.hasNext()){
        hashItr.next();
        itemData.insert(hashItr.value(),this->data(index, hashItr.key()));
    }
    return QVariant(itemData);
}

QList<QString> CardModel::getIDList()
{
    QList<QString> ret;

    for (int i =0; i < system_info->num_gpus; i++) {
        struct gpu_info *gpu = system_info->gpu_info[i];

        if (gpu->id_path_tag != NULL)
            ret.append(gpu->id_path_tag);
    }
    return ret;
}

QList<QString> CardModel::getDriverList()
{
    QList<QString> ret;

    for (int i = 0; i < system_info->num_gpus; i++) {
        struct gpu_info *gpu = system_info->gpu_info[i];
        if (gpu->struct_flag_display_system_info_drm != FILLED)
            continue;
        int fd = gpu->display_system_info_drm->display_system.drm_info->fd;
        QString driver = QString(loader_get_driver_for_fd(fd, 0));
        if (!ret.contains(driver))
            ret.append(driver);
    }
    return ret;
}

QHash<int, QByteArray> CardModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IDRole] = "id_path_tag";
    roles[DriverNameRole] = "driver_name";
    roles[DrmRole] = "drm";
    roles[X11Role] = "x11";
    roles[WaylandRole] = "Wayland";
    return roles;
}
