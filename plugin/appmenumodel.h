/******************************************************************
 * Copyright 2016 Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************/

#ifndef APPMENUMODEL_H
#define APPMENUMODEL_H

#include <QAbstractListModel>
#include <QAbstractNativeEventFilter>
#include <QStringList>
#include <KWindowSystem>
#include <QPointer>
#include <QRect>

class QMenu;
class QAction;
class QModelIndex;
class QDBusServiceWatcher;
class KDBusMenuImporter;

class AppMenuModel : public QAbstractListModel, public QAbstractNativeEventFilter
{
    Q_OBJECT

    Q_PROPERTY(bool menuAvailable READ menuAvailable WRITE setMenuAvailable NOTIFY menuAvailableChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

    Q_PROPERTY(bool filterByActive READ filterByActive WRITE setfilterByActive NOTIFY filterByActiveChanged)
    Q_PROPERTY(QRect screenGeometry READ screenGeometry WRITE setScreenGeometry NOTIFY screenGeometryChanged)
public:
    explicit AppMenuModel(QObject *parent = nullptr);
    ~AppMenuModel() override;

    enum AppMenuRole
    {
        MenuRole = Qt::UserRole + 1, // TODO this should be Qt::DisplayRole
        ActionRole
    };

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath);

    bool filterByActive() const;
    void setfilterByActive(bool active);

    bool menuAvailable() const;
    void setMenuAvailable(bool set);

    bool visible() const;

    QRect screenGeometry() const;
    void setScreenGeometry(QRect geometry);

signals:
    void requestActivateIndex(int index);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long int *result) override;

private Q_SLOTS:
    void onActiveWindowChanged(WId id);
    void onWindowChanged(WId id);
    void filterWindow(KWindowInfo &info);

    void setVisible(bool visible);
    void update();

signals:
    void menuAvailableChanged();
    void modelNeedsUpdate();
    void filterByActiveChanged();
    void visibleChanged();
    void screenGeometryChanged();

private:
    bool m_filterByActive = false;
    bool m_menuAvailable;
    bool m_updatePending = false;
    bool m_visible = true;

    QRect m_screenGeometry;

    //! current active window used
    WId m_currentWindowId = 0;
    //! window that its menu initialization may be delayed
    WId m_delayedMenuWindowId = 0;

    QPointer<QMenu> m_menu;

    QDBusServiceWatcher *m_serviceWatcher;
    QString m_serviceName;
    QString m_menuObjectPath;

    QPointer<KDBusMenuImporter> m_importer;
};

#endif
