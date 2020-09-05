QT += qml quick gui widgets
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    crypto/AES256.cpp \
    crypto/AESCommon.cpp \
    crypto/BlockCipher.cpp \
    crypto/CBC.cpp \
    crypto/Cipher.cpp \
    crypto/Crypto.cpp \
    crypto/Hash.cpp \
    crypto/pbkdf2.cpp \
    crypto/SHA1.cpp \
    crypto/SHA256.cpp \
    src/CTaskModel.cpp \
    src/CTrayManager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = icons/Timekeeper_SysTray.ico

DISTFILES += \
    icons/bars-solid.png \
    icons/calendar-alt-regular.png \
    icons/edit-regular.png \
    icons/exclamation-triangle-solid.png \
    icons/eye-regular.png \
    icons/eye-slash-regular.png \
    icons/home-solid.png \
    icons/lock-solid.png \
    icons/minus-square-regular.png \
    icons/plus-circle-solid.png \
    icons/plus-square-regular.png \
    icons/sort-alpha-down-solid.png \
    icons/Timekeeper_Logo.png \
    icons/Timekeeper_SysTray.png \
    icons/Timekeeper_SysTray_Alert.png \
    icons/Timekeeper_SysTray.ico \
    src/AboutWindow.qml \
    src/AddEditWindow.qml \
    src/AddTimeDialog.qml \
    src/AreYouSure.qml \
    src/EditContextMenu.qml \
    src/EditContextMenuReport.qml \
    src/LoadFileDialog.qml \
    src/Notification.qml \
    src/OptionsMenu.qml \
    src/PasswordEntry.qml \
    src/PasswordIncorrect.qml \
    src/ReallocateDialog.qml \
    src/ReportWindow.qml \
    src/SaveFileDialog.qml \
    src/SettingsWindow.qml \
    src/SortMenu.qml

HEADERS += \
    crypto/utility/EndianUtil.h \
    crypto/utility/LimbUtil.h \
    crypto/utility/ProgMemUtil.h \
    crypto/utility/RotateUtil.h \
    crypto/AES.h \
    crypto/BlockCipher.h \
    crypto/CBC.h \
    crypto/Cipher.h \
    crypto/Crypto.h \
    crypto/Hash.h \
    crypto/pbkdf2.h \
    crypto/SHA1.h \
    crypto/SHA256.h \
    src/CTaskModel.h \
    src/CTrayManager.h

