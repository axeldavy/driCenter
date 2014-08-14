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

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <EGL/egl.h>

CardInfo::CardInfo(QObject *parent)
    : QObject(parent)
{
    m_pci_id = "undefined";
    m_name = "undefined";
}

void CardInfo::fillInfo(const QString &pci_id)
{
    m_pci_id = pci_id;
    m_name = "name";
    this->fillEGLInfo();
}

bool CardInfo::getPlaforms(QString* &platforms, char &numPlatforms)
{
    platforms[0] = "undefined";
    numPlatforms = 1;
    return true;
}

QString CardInfo::getPciId()
{
    return m_pci_id;
}

QString CardInfo::getName()
{
    return m_name;
}

void CardInfo::fillGLXInfo()
{
    char *dri_prime_env;
    dri_prime_env = strdup("DRI_PRIME=");
    dri_prime_env = strcat(dri_prime_env, m_pci_id.toUtf8().constData());
    if (m_pci_id != "undefined")
        putenv(dri_prime_env);
    Display *dpy = XOpenDisplay(NULL);


}

void CardInfo::fillEGLInfo()
{
    char *dri_prime_env;
    const char *pci_id_path_tag = m_pci_id.toUtf8().constData();
    dri_prime_env = strdup("DRI_PRIME=");
    dri_prime_env = strcat(dri_prime_env, pci_id_path_tag);
    if (m_pci_id != "undefined")
        putenv(dri_prime_env);


    const char *version, *extensions;
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    EGLContext context;
    EGLConfig configs[1024];
    EGLint count;

    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE,         EGL_WINDOW_BIT,
        EGL_RED_SIZE,             1,
        EGL_GREEN_SIZE,           1,
        EGL_BLUE_SIZE,            1,
        EGL_ALPHA_SIZE,           0,
        EGL_RENDERABLE_TYPE,      EGL_OPENGL_BIT,
        EGL_CONFIG_CAVEAT,        EGL_NONE,
        EGL_NONE,
    };
    const EGLint context_attribs_legacy[] = {
        EGL_NONE
    };

    m_egl_info = "";
    eglInitialize(dpy, &major, &minor);
    version = eglQueryString(dpy, EGL_VERSION);
    extensions = eglQueryString(dpy, EGL_EXTENSIONS);
    eglBindAPI(EGL_OPENGL_API);
    m_egl_info += "EGL version: ";
    /*m_egl_info += QString::number(major);
    m_egl_info += '.';
    m_egl_info += QString::number(minor);
    m_egl_info += "\nversion string: ";
    m_egl_info += version;
    m_egl_info += "\nEGL extensions: ";
    m_egl_info += extensions;*/
    if (eglChooseConfig(dpy, config_attribs, configs, 1, &count) == EGL_FALSE || count != 1) {
       return;
    }
    context = eglCreateContext(dpy, configs[0], EGL_NO_CONTEXT, context_attribs_legacy);
    if (context == EGL_NO_CONTEXT)
        return;
    if (eglMakeCurrent(dpy, NULL, NULL, context) == EGL_FALSE)
        return;
    m_egl_info += "\nVendor string: ";
    m_egl_info += (char*) glGetString(GL_VENDOR);

}

QString CardInfo::getEGLInfo() {
    return m_egl_info;
}

///////////////////////////////////////

CardModel::CardModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

CardModel::~CardModel(){

}


void CardModel::addCard(CardInfo* card)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_cards << card;
    endInsertRows();
}

int CardModel::rowCount(const QModelIndex &parent) const
{
    (void) parent;
    return m_cards.count();
}

QVariant CardModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_cards.count())
        return QVariant();

    CardInfo* card = m_cards[index.row()];
    if (role == NameRole) {
        QString name = card->getName();
        return name;
    }
    return QVariant();
}

QHash<int, QByteArray> CardModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    return roles;
}
