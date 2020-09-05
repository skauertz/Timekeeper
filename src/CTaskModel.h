/*
Copyright (C) 2020 by Sebastian Kauertz.

This file is part of Timekeeper, a Qt-based time tracking app.

Timekeeper is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Timekeeper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef CTASKMODEL_H
#define CTASKMODEL_H

#include <math.h>
#include <QAbstractListModel>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QBuffer>
#include <QScreen>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include "crypto/Crypto.h"
#include "crypto/CBC.h"
#include "crypto/AES.h"
#include "crypto/SHA256.h"
#include "crypto/pbkdf2.h"


class CTaskModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum TaskRole {
        TitleRole              = Qt::DisplayRole,
        DescriptionRole        = Qt::UserRole,
        taskIDRole             = 0x0101,
        taskActiveRole         = 0x0102,
        allocateTimeRole       = 0x0103,
        HoursTodayRole         = 0x0104,
        MinutesTodayRole       = 0x0105,
        SecondsTodayRole       = 0x0106,
        elapsedSecTodayRole    = 0x0107,
        TodayStringRole        = 0x0108,
        HoursThisMonthRole     = 0x0109,
        MinutesThisMonthRole   = 0x010A,
        SecondsThisMonthRole   = 0x010B,
        ThisMonthStringRole    = 0x010C,
        HoursThisYearRole      = 0x010D,
        MinutesThisYearRole    = 0x010E,
        SecondsThisYearRole    = 0x010F,
        ThisYearStringRole     = 0x0110,
        HoursDailyRole         = 0x0111,
        MinutesDailyRole       = 0x0112,
        SecondsDailyRole       = 0x0113,
        elapsedSecDailyRole    = 0x0114,
        DailyStringRole        = 0x0115,
        HoursMonthlyRole       = 0x0116,
        MinutesMonthlyRole     = 0x0117,
        SecondsMonthlyRole     = 0x0118,
        elapsedSecMonthlyRole  = 0x0119,
        MonthlyStringRole      = 0x011A,
        HoursYearlyRole        = 0x011B,
        MinutesYearlyRole      = 0x011C,
        SecondsYearlyRole      = 0x011D,
        elapsedSecYearlyRole   = 0x011E,
        YearlyStringRole       = 0x011F
    };
    Q_ENUM(TaskRole)

    // Public types
    struct sTime {
        quint16 Hours;
        quint16 Minutes;
        quint16 Seconds;
        quint32 elapsedSeconds;
    };

    CTaskModel(QObject *parent = nullptr);
    ~CTaskModel();

    Q_INVOKABLE int rowCount(const QModelIndex & = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    Q_PROPERTY(quint16 TotalHoursToday        MEMBER m_TotalHoursToday        NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalMinutesToday      MEMBER m_TotalMinutesToday      NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalSecondsToday      MEMBER m_TotalSecondsToday      NOTIFY TimeChanged)
    Q_PROPERTY(QString TotalTimeToday         MEMBER m_TotalTimeToday         NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalHoursThisMonth    MEMBER m_TotalHoursThisMonth    NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalMinutesThisMonth  MEMBER m_TotalMinutesThisMonth  NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalSecondsThisMonth  MEMBER m_TotalSecondsThisMonth  NOTIFY TimeChanged)
    Q_PROPERTY(QString TotalTimeThisMonth     MEMBER m_TotalTimeThisMonth     NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalHoursThisYear     MEMBER m_TotalHoursThisYear     NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalMinutesThisYear   MEMBER m_TotalMinutesThisYear   NOTIFY TimeChanged)
    Q_PROPERTY(quint16 TotalSecondsThisYear   MEMBER m_TotalSecondsThisYear   NOTIFY TimeChanged)
    Q_PROPERTY(QString TotalTimeThisYear      MEMBER m_TotalTimeThisYear      NOTIFY TimeChanged)
    Q_PROPERTY(QString TotalTimeDaily         MEMBER m_totalTimeDailyString   NOTIFY TimeChanged)
    Q_PROPERTY(QString TotalTimeMonthly       MEMBER m_totalTimeMonthlyString NOTIFY TimeChanged)
    Q_PROPERTY(QString TotalTimeYearly        MEMBER m_totalTimeYearlyString  NOTIFY TimeChanged)
    Q_PROPERTY(quint32 TotalSecondsDaily      MEMBER m_totalSecondsDaily      NOTIFY TimeChanged)
    Q_PROPERTY(quint32 TotalSecondsMonthly    MEMBER m_totalSecondsMonthly    NOTIFY TimeChanged)
    Q_PROPERTY(quint32 TotalSecondsYearly     MEMBER m_totalSecondsYearly     NOTIFY TimeChanged)
    Q_PROPERTY(qint16  settingLastSelectedDate     MEMBER m_settingLastSelectedDate      NOTIFY settingChanged)
    Q_PROPERTY(qint16  settingWeightedReallocation MEMBER m_settingWeightedReallocation  NOTIFY settingChanged)
    Q_PROPERTY(qint16  settingAutosave             MEMBER m_settingAutosave              NOTIFY settingChanged)
    Q_PROPERTY(qint16  settingMinimizeToTray       MEMBER m_settingMinimizeToTray        NOTIFY settingChanged)
    Q_PROPERTY(qint16  settingStartingView         MEMBER m_settingStartingView          NOTIFY settingChanged)
    Q_PROPERTY(qint16  settingColorScheme          MEMBER m_settingColorScheme           NOTIFY settingChanged)
    Q_PROPERTY(quint8  defaultFile   MEMBER m_defaultFile      NOTIFY settingChanged)
    Q_PROPERTY(qint16  windowPosX    MEMBER m_windowPosX       NOTIFY windowPosChanged)
    Q_PROPERTY(qint16  windowPosY    MEMBER m_windowPosY       NOTIFY windowPosChanged)
    Q_PROPERTY(quint16 windowWidth   MEMBER m_windowWidth      NOTIFY windowPosChanged)
    Q_PROPERTY(quint16 windowHeight  MEMBER m_windowHeight     NOTIFY windowPosChanged)
    Q_PROPERTY(quint8  PWneeded      MEMBER m_PWneeded         NOTIFY PasswordNeeded)
    Q_PROPERTY(quint8  PWwrong       MEMBER m_PWwrong          NOTIFY WrongPassword)
    Q_PROPERTY(QString SaveFile      MEMBER m_SaveFileName     NOTIFY settingChanged)
    Q_PROPERTY(QString SaveFileFull  MEMBER m_SaveFileNameFull NOTIFY settingChanged)
    Q_PROPERTY(quint8  FileEncrypted MEMBER m_FileEncrypted    NOTIFY settingChanged)
    Q_PROPERTY(QString LogFileFull   MEMBER m_LogFileNameFull  NOTIFY settingChanged)


    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE void checkFileType(QString File);
    Q_INVOKABLE void load_data(QString Password, QString File);
    Q_INVOKABLE int  save_data(QString File);
    Q_INVOKABLE void set_password(QString PW);
    Q_INVOKABLE void removeEncryption();
    Q_INVOKABLE int  append(const QString &title, const QString &description);
    Q_INVOKABLE void set(int row, const QString &title, const QString &description);
    Q_INVOKABLE void updateEntry(int row, const sTime timeTotal, const sTime timeToday, const QString timeTodayString, const sTime timeThisMonth, const QString timeThisMonthString, const sTime timeThisYear, const QString timeThisYearString);
    Q_INVOKABLE void updateEntry2(int row, const QDate date, const sTime timeToDate);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order=Qt::AscendingOrder);
    Q_INVOKABLE int  row(qint32 taskID);
    Q_INVOKABLE void startTimer(int row);
    Q_INVOKABLE void stopTimer(int row);
    Q_INVOKABLE void switchAllocate(int row);
    Q_INVOKABLE void resetAllocate();
    Q_INVOKABLE void reallocate(int row, qint16 day, quint16 weighted);
    Q_INVOKABLE void reallocateAll(int row, qint16 day, quint16 weighted);
    Q_INVOKABLE void add_time(int row, qint16 day, int hours, int minutes, int seconds);
    Q_INVOKABLE void updateTotals();
    Q_INVOKABLE void updateYearlyList(quint16 year);
    Q_INVOKABLE QString updateMonthlyList(qint8 month, quint16 year);
    Q_INVOKABLE QString updateDailyList(qint16 day);
    Q_INVOKABLE void remove(int row);
    Q_INVOKABLE void removeAll();
    Q_INVOKABLE void resetToday(int row);
    Q_INVOKABLE void resetTotal(int row);
    Q_INVOKABLE void resetAll();
    Q_INVOKABLE void savePosition(int x, int y, int width, int height);
    Q_INVOKABLE QString csvWriter();
    Q_INVOKABLE void openHelp();
    Q_INVOKABLE void restoreBackup();    // Unused

    void checkEntries(int row);   // Debug: Print all timelog entries for one task
    void Update();
    void UpdateAll();
    bool readIniFile();
    bool readfile(QString filename);
    bool readfileEncrypted(QString filename);
    int  writefile();
    int  writefileEncrypted();
    void writeIniFile();
    void backupfile();
    void checkDayChange();
    void updatePosition();


signals:
    void TimeChanged();
    void windowPosChanged();
    void settingChanged();
    void PasswordNeeded();
    void WrongPassword();
    void closing();


private:
    // The basic Task structure:
    struct Task {
        QString title;
        QString description;
        quint32 taskID;
        quint8  taskActive;           // 1 if this task is currently counting time
        quint8  allocateTime;         // 1 if this task is a target for reallocation
        sTime   timeTotal;            // Total time logged on task
        sTime   timeToday;            // Time logged on task today
        QString timeTodayString;      // String to display time logged on task today
        sTime   timeThisMonth;        // Time logged on task this month
        QString timeThisMonthString;  // String to display time logged on task this month
        sTime   timeThisYear;         // Time logged on task this year
        QString timeThisYearString;   // String to display time logged on task this year
        sTime   timeDaily;            // Time logged on task per day (for report window; only ever holds the last day queried by the HMI)
        QString timeDailyString;      // String to display time logged on task per day
        sTime   timeMonthly;          // Time logged on task per month (for report window; only ever holds the last month queried by the HMI)
        QString timeMonthlyString;    // String to display time logged on task per month
        sTime   timeYearly;           // Time logged on task per year (for report window; only ever holds the last year queried by the HMI)
        QString timeYearlyString;     // String to display time logged on task per year
        QMap<QDate, sTime> timelog;   // Map between date and time logged per day
    };
    // The global list of Tasks:
    QList<Task> m_tasks;

    sTime   totalTimeToday;
    sTime   totalTimeThisMonth;
    sTime   totalTimeThisYear;
    QString m_totalTimeDailyString;      // For report window
    QString m_totalTimeMonthlyString;    // For report window
    QString m_totalTimeYearlyString;     // For report window
    quint32 m_totalSecondsDaily;         // For report window
    quint32 m_totalSecondsMonthly;       // For report window
    quint32 m_totalSecondsYearly;        // For report window

    QDate   today;
    quint8  thisMonth, thisMonthSaved;
    quint16 thisYear, thisYearSaved;
    quint16 m_TotalHoursToday, m_TotalMinutesToday, m_TotalSecondsToday;
    quint16 m_TotalHoursThisMonth, m_TotalMinutesThisMonth, m_TotalSecondsThisMonth;
    quint16 m_TotalHoursThisYear, m_TotalMinutesThisYear, m_TotalSecondsThisYear;
    QString m_TotalTimeToday, m_TotalTimeThisMonth, m_TotalTimeThisYear;
    QTimer  timer;            // The main timer for tasks
    QTimer  dayChangeCheck;   // Timer to check if day has changed
    // Settings
    qint16  m_settingLastSelectedDate;
    qint16  m_settingWeightedReallocation;
    qint16  m_settingAutosave;
    qint16  m_settingMinimizeToTray;
    qint16  m_settingStartingView;
    qint16  m_settingColorScheme;
    qint16  m_windowPosX, m_windowPosY;
    quint16 m_windowWidth, m_windowHeight;
    qint16  m_windowPosXSave, m_windowPosYSave;
    quint16 m_windowWidthSave, m_windowHeightSave;
    quint8  m_PWneeded, m_PWwrong, m_FileEncrypted;
    QScreen *Screen;
    QRect   ScreenSize;

    int     activeRow;  // Holds the currently active row in the QList.
    qint32  activeID;   // Holds the currently active (counting) taskID. -1 if no task is counting.

    // File management
    quint32 magic_no;            // File type magic number (0x051076A0: Unencrypted, 0x051076B0: Encrypted)
    quint16 version_no;          // Save file version number (currently: 100)
    QString m_SaveFileName;      // Holds name of the currently used save file
    QString m_SaveFileNameFull;  // Holds full path to the currently used save file
    QString m_LogFileNameFull;   // Holds full path to the currently used log file
    quint8  m_defaultFile;       // Flags whether there is a default file specified in the ini file (to inform QML)
    struct sFile {
        QString SaveFileName;          // Holds name of the currently used save file
        QString SaveFileNameFull;      // Holds full path to the currently used save file
        quint32 magic_no;              // File type magic number (0x051076A0: Unencrypted, 0x051076B0: Encrypted)
        uint8_t encrypted;             // File is encrypted [1] or not [0]
        uint8_t salt[16];              // The salt used with this password
        QString Password;              // The password to use for this file
        uint8_t PasswordHashRead[32];  // The hashed password read from this file
    };
    sFile  activeFile;      // The file currently loaded
    sFile  newFile;         // Temporary file object for loading operations
    QFile  logFile;         // File for logging output
    QTextStream logOut;

    // Crypto
    // Encryption works as follows:
    // All time data is encrypted in Cipher Block Chaining mode using a random IV.
    // 16 Bytes of null data are prepended when writing the file and discarded on read so we don't need to remember the IV.
    // The encryption key is derived from the user password and a random salt using a PBKDF2 function.
    // The salt and the hashed password are stored within the encrypted file (the salt to generate the encryption key for decryption,
    // the hashed password to check whether the entered password is correct).
    CBC<AES256>  cbc;      // Cipher Block Chaining using AES256
    SHA256  sha256;        // Hashing algorithm
    uint8_t key[32];       // The key used for encryption (this is not the password!)
    uint8_t salt[16];      // The salt used with the password to generate the key
    uint8_t iv[16];        // The initialization vector for CBC
    uint8_t PasswordHash[32];   // The hashed password (for saving in the encrypted file & comparing entered password with saved one)

};

#endif // CTASKMODEL_H
