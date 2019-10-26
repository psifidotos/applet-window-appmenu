/*
 * Copyright 2018  Michail Vourlakos <mvourlakos@gmail.com>
 *
 * This file is part of the libappletdecoration library
 *
 * Latte-Dock is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Latte-Dock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "schemesmodel.h"

#include "commontools.h"
#include "schemecolors.h"

#include <QDebug>
#include <QDir>

SchemesModel::SchemesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    initSchemes();
}

SchemesModel::~SchemesModel()
{
    qDeleteAll(m_schemes);
}

bool SchemesModel::currentOptionIsShown() const
{
    return m_currentOptionIsShown;
}

void SchemesModel::setCurrentOptionIsShown(bool isShown)
{
    if (m_currentOptionIsShown == isShown) {
        return;
    }

    m_currentOptionIsShown = isShown;
    emit currentOptionIsShownChanged();
}

QVariant SchemesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= m_schemes.count()) {
        return QVariant();
    }

    const SchemeColors *d = m_schemes[index.row()];

    switch (role) {
        case Qt::DisplayRole:
            if (index.row() == 0) {
                return "Default";
            }

            if (index.row() == 1 && m_currentOptionIsShown) {
                return "Current Window";
            }

            return d->schemeName();

        case Qt::UserRole + 4:
            if (index.row() == 0) {
                return "_default_";
            }

            if (index.row() == 1 && m_currentOptionIsShown) {
                return "_current_";
            }

            return d->schemeFile();

        case Qt::UserRole + 5:
            return d->backgroundColor();

        case Qt::UserRole + 6:
            return d->textColor();
    }

    return QVariant();
}

int SchemesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_schemes.count();
}

QHash< int, QByteArray > SchemesModel::roleNames() const
{
    QHash<int, QByteArray> roles({
        {Qt::DisplayRole, QByteArrayLiteral("display")},
        {Qt::UserRole + 4, QByteArrayLiteral("file")},
        {Qt::UserRole + 5, QByteArrayLiteral("backgroundColor")},
        {Qt::UserRole + 6, QByteArrayLiteral("textColor")}
    });
    return roles;
}

QColor SchemesModel::backgroundOf(const int &index) const
{
    if (index>=0 && index<m_schemes.count()) {
        return m_schemes[index]->backgroundColor();
    }

    return QColor("transparent");
}

void SchemesModel::initSchemes()
{
    qDeleteAll(m_schemes);
    m_schemes.clear();

    QString currentSchemePath = SchemeColors::possibleSchemeFile("kdeglobals");
    insertSchemeInList(currentSchemePath);

    QStringList standardPaths = AppletDecoration::standardPathsFor("color-schemes");

    QStringList registeredSchemes;

    for(auto path : standardPaths) {
        QDir directory(path);
        QStringList tempSchemes = directory.entryList(QStringList() << "*.colors" << "*.COLORS", QDir::Files);

        foreach (QString filename, tempSchemes) {
            if (!registeredSchemes.contains(filename)) {
                QString fullPath = path + "/" + filename;
                insertSchemeInList(fullPath);
                registeredSchemes << filename;
            }
        }
    }
}

void SchemesModel::insertSchemeInList(QString file)
{
    SchemeColors *tempScheme = new SchemeColors(this, file);

    int atPos{0};

    for (int i = 0; i < m_schemes.count(); i++) {
        SchemeColors *s = m_schemes[i];

        int result = QString::compare(tempScheme->schemeName(), s->schemeName(), Qt::CaseInsensitive);

        if (result < 0) {
            atPos = i;
            break;
        } else {
            atPos = i + 1;
        }

    }

    m_schemes.insert(atPos, tempScheme);
}

int SchemesModel::indexOf(QString file)
{
    if (file.isEmpty() || file == "_default_") {
        return 0;
    }

    if (file == "_current_") {
        return 1;
    }

    for (int i = 0; i < m_schemes.count(); i++) {
        SchemeColors *s = m_schemes[i];

        if (s->schemeFile() == file) {
            return i;
        }
    }

    return -1;
}

