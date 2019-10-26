# Window AppMenu Applet

This is a Plasma 5 applet that shows the current window appmenu in your panels. This plasmoid is coming from [Latte land](https://phabricator.kde.org/source/latte-dock/repository/master/) but it can also support Plasma panels.

<p align="center">
<img src="https://i.imgur.com/T0sLWav.gif" width="580"><br/>
<i>hide menu when active window is minimized</i>
</p>

<p align="center">
<img src="https://i.imgur.com/ARJbMWX.gif" width="580"><br/>
<i>scroll menu support</i>
</p>

<p align="center">
<img src="https://i.imgur.com/pAX0a2J.png" width="580"><br/>
<i>Settings window</i>
</p>

# Requires

- Qt >= 5.9
- KF5 >= 5.38
- Plasma >= 5.12

**Qt elements**: Quick Widgets DBus

**KF5 elements**: Plasma WindowSystem KDecoration2 extra-cmake-modules

**X11 libraries**: XCB RandR

# Install

You can execute `sh install.sh` in the root directory as long as you have installed the previous mentioned development packages

