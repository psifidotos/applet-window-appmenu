/*
 *   Copyright 2017 David Edmundson <davidedmundson@kde.org>
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QApplication>

#include <QMainWindow>
#include <QMenuBar>
#include <QDateTime>
#include <QIcon>
#include <QDebug>

class MainWindow : public QMainWindow
{
public:
    MainWindow();
};

MainWindow::MainWindow() :
    QMainWindow()
{
    /*set an initial menu with the following
    Menu A
      - Item
      - Checkable Item
      - Item With Icon
      - A separator
      - Menu B
         - Item B1
     Menu C
      - DynamicItem ${timestamp}

      TopLevelItem
    */

    QAction *t;
    auto menuA = new QMenu("Menu A", this);
    menuA->addAction("Item");

    t = menuA->addAction("Checkable Item");
    t->setCheckable(true);

    t = menuA->addAction(QIcon::fromTheme("document-edit"), "Item with icon");

    menuA->addSeparator();

    auto menuB = new QMenu("Menu B", this);
    menuB->addAction("Item B1");
    menuA->addMenu(menuB);

    menuBar()->addMenu(menuA);

    auto menuC = new QMenu("Menu C", this);
    connect(menuC, &QMenu::aboutToShow, this, [menuC]() {
        menuC->clear();
        menuC->addAction("Dynamic Item " + QDateTime::currentDateTime().toString());
    });

    menuBar()->addMenu(menuC);

    menuBar()->addAction("Top Level Item");
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();
    return app.exec();
}
