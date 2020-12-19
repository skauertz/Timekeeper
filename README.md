<a name="Timekeeper" />

<p align="center">
<img src="https://www.mediafire.com/convkey/fa55/yajr055h6hk5ejg6g.jpg" alt="Timekeeper" />
</p>

---

<p align="center">
<a href="https://github.com/skauertz/Timekeeper/releases">
<img src="https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg" alt="Releases" />
</a>
<a href="https://github.com/skauertz/Timekeeper/blob/master/LICENSE">
<img src="https://img.shields.io/badge/license-GPLv3-blue.svg" alt="GPLv3" />
</a>
</p>

---

<p align="center"><b>
Timekeeper is a simple time-tracking app based on Qt and QML.
</b></p>

---

Timekeeper is a simple app to facilitate time tracking over various tasks and provide some simple statistics on it. It is mainly meant to support more accurate booking of time to different projects, but it can also be used for other purposes, from monitoring speaker time in plenary discussions to tracking how much time you spend in various video games.

Timekeeper is a purely local application. None of your data is saved in the Cloud, and no online account is necessary. All your data is saved locally in a file you specify and can optionally be encrypted.

Timekeeper is currently considered feature-complete, with a lot of testing already done by the author. Nevertheless, <b>bugs are possible</b>, so don’t hesitate to open an issue if something does not work as expected.

---

### Features
- Manual tracking of time spent on various tasks
- Report window showing simple statistics on a daily, monthly and yearly basis
- Modification and reallocation of time from one task to one or multiple others
- Sorting of task list
- Encryption of time and task data
- Export of time and task data to CSV format

---

### Screenshots

![Main Window](https://www.mediafire.com/convkey/db27/1zvj5hwooze575x6g.jpg)
![Report Window](https://www.mediafire.com/convkey/b4a1/d5bsfgkvshzogfv6g.jpg)

---

### Windows Installation
#### Requirements
None. All dependencies are included in the executable.

#### Installation
A release package for Windows can be downloaded from the [Release] page.

Timekeeper does not require system-wide installation. Just extract the archive in a location of your choice and start `Timekeeper.exe` from there.

Timekeeper has been tested on Windows 10 only. Other versions of Windows may work, but are not officially supported.

---

### Linux Installation
#### Requirements
* Qt>=5.9.5 (Ubuntu 18.04 or later)

#### Installation
A release package for Linux can be downloaded from the [Release] page.

Timekeeper does not require system-wide installation. Just extract the archive in a location of your choice and start it by launching `Timekeeper` from there.

Note: Make sure `Timekeeper` is made executable.

---

### Compiling from source
Timekeeper has been developed with QtCreator 4.6.2, and the source code repository includes a project file for it. This should allow you to import the project directly into QtCreator. Provided you have an appropriate kit for your platform you should be able to compile it without any problems.

Timekeeper has been developed and tested using Qt 5.11.1. Any Qt version later than this should be compatible.

---

### Contributions
Timekeeper is currently considered feature-complete, but that doesn't mean new features may not be added in the future. If you think about a feature that may be useful to add, feel free to open an [issue].

Some features that may be added in a future release are:

* Verification mechanism when changing encryption password
* Compensate for time spent in suspend or hibernate with a running task
* Allow opening the app with a data file as argument
* Import from CSV
* Customizable colors

---

### Bug reports
Bug reports are always welcome. If you think you have found a bug, please open an [issue] for it.


[Release]: https://github.com/skauertz/Timekeeper/releases
[issue]: https://github.com/skauertz/Timekeeper/issues
