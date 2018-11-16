# Window AppMenu Applet

This is a Plasma 5 applet that shows the current window appmenu in your panels. This plasmoid is coming from [Latte land](https://phabricator.kde.org/source/latte-dock/repository/master/) but it can also support Plasma panels.

<p align="center">
<img src="https://im6.ezgif.com/tmp/ezgif-6-fd4a32f1fb47.gif" width="580"><br/>
<i>hide menu when active window is minimized</i>
</p>

<p align="center">
<img src="https://i.imgur.com/gBPQfEO.png" width="580"><br/>
<i>Settings window</i>
</p>

# Features

- Minimize-state aware
- Multi-Screen aware
- Support Latte new painting
- based totally on official plasma appmenu applet in order to improve maintainability and code exchange


# Requires

- Qt >= 5.9
- KF5 >= 5.38
- Plasma >= 5.12

**Qt elements**: Quick Widgets DBus
**KF5 elements**: Plasma WindowSystem
**X11 libraries**: XCB RandR

# Install

You can execute `sh install.sh` in the root directory as long as you have installed the previous mentioned development packages

