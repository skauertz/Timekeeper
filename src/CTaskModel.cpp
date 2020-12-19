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
#include <QtWidgets/QApplication>
#include "CTaskModel.h"

CTaskModel::CTaskModel(QObject *parent ) : QAbstractListModel(parent)
{

    // Memorize the current date
    today     = QDate::currentDate();
    thisMonth = (quint8)  today.month();
    thisYear  = (quint16) today.year();
    //printf("Today is %s\n",today.toString("dd.MM.yyyy").toUtf8().data());

    Screen = QApplication::primaryScreen();
    ScreenSize = Screen->geometry();
    // Default position for the window:
    m_windowPosX = ScreenSize.width() - 400 - 50;
    m_windowPosY = ScreenSize.height()/2 - 500/2;
    m_windowWidth  = 400;
    m_windowHeight = 500;
    connect(qApp, &QApplication::screenRemoved, this, &CTaskModel::updatePosition);
    connect(qApp, &QApplication::primaryScreenChanged, this, &CTaskModel::updatePosition);


    // Default values for settings:
    m_settingLastSelectedDate     = 0;
    m_settingWeightedReallocation = 0;
    m_settingAutosave             = 1;
    m_settingMinimizeToTray       = 0;
    m_settingStartingView         = 0;
    m_settingColorScheme          = 0;
    m_SaveFileName                = "";
    m_SaveFileNameFull            = "";
    m_LogFileNameFull             = "";
    m_defaultFile                 = 0;
    m_FileEncrypted               = 0;

    m_PWwrong                     = 0;

    activeFile.SaveFileName       = "";
    activeFile.SaveFileNameFull   = "";
    activeFile.magic_no           = 0x00000000;
    activeFile.encrypted          = 0;
    memset(activeFile.PasswordHashRead, 0, 32);

    newFile.SaveFileName          = "";
    newFile.SaveFileNameFull      = "";
    newFile.magic_no              = 0x00000000;
    newFile.encrypted             = 0;
    memset(newFile.PasswordHashRead, 0, 32);

    // Open log file:
    logFile.setFileName("Timekeeper.log");
    if (!logFile.open(QIODevice::WriteOnly)) {
        qWarning("Could not open log file!");
    }
    m_LogFileNameFull = QDir::currentPath() + "/" + logFile.fileName();
    logOut.setDevice(&logFile);
    logOut << "Timekeeper v1.0b" << endl;
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Starting." << endl;

    // Load ini file:
    readIniFile();

    // Loading the data file is triggered from main.qml

    emit windowPosChanged();
    emit settingChanged();

    activeID   = -1;
    activeRow  = -1;

    // Timer setup
    connect(&timer, &QTimer::timeout, this, &CTaskModel::Update);
    timer.setTimerType(Qt::TimerType::PreciseTimer);

    connect(&dayChangeCheck, &QTimer::timeout, this, &CTaskModel::checkDayChange);
    dayChangeCheck.setTimerType(Qt::TimerType::CoarseTimer);
    dayChangeCheck.start(1000);

    // Test PBKDF2
/*
    int     len;
    uint8_t *pass;
    uint8_t *psalt;
    QString Salt;

    Password = "password";
    len = 32;
    if (Password.length()<32) len = Password.length();
    pass = new uint8_t[len];
    memset(pass, 0, len);
    memcpy(pass, Password.toUtf8().data(), len);

    Salt = "IEEE";
    psalt = new uint8_t[4];
    memcpy(psalt, Salt.toUtf8().data(), 4);

    pkcs5_pbkdf2(pass, len, psalt, 4, &key[0], 32, 4096);

//    qInfo("Password = %s  Salt = %s  key = %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
//          pass,psalt,key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7],key[8],key[9],key[10],key[11],key[12],key[13],key[14],key[15],
//            key[16],key[17],key[18],key[19],key[20],key[21],key[22],key[23],key[24],key[25],key[26],key[27],key[28],key[29],key[30],key[31]);
    delete[] pass;
    delete[] psalt;
*/

}

CTaskModel::~CTaskModel()
{

    // Saving / Autosaving is managed from main.qml
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Dtor: Closing." << endl;

    // Ini file is always saved on quit:
    writeIniFile();

    // Close log file:
    logFile.close();
}


// Overloaded public functions:
int CTaskModel::rowCount(const QModelIndex &) const
{
    return m_tasks.count();
}

QVariant CTaskModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:             return m_tasks.at(index.row()).title;
        case DescriptionRole:       return m_tasks.at(index.row()).description;
        case taskIDRole:            return m_tasks.at(index.row()).taskID;
        case taskActiveRole:        return m_tasks.at(index.row()).taskActive;
        case allocateTimeRole:      return m_tasks.at(index.row()).allocateTime;
        case HoursTodayRole:        return m_tasks.at(index.row()).timeToday.Hours;
        case MinutesTodayRole:      return m_tasks.at(index.row()).timeToday.Minutes;
        case SecondsTodayRole:      return m_tasks.at(index.row()).timeToday.Seconds;
        case elapsedSecTodayRole:   return m_tasks.at(index.row()).timeToday.elapsedSeconds;
        case TodayStringRole:       return m_tasks.at(index.row()).timeTodayString;
        case HoursThisMonthRole:    return m_tasks.at(index.row()).timeThisMonth.Hours;
        case MinutesThisMonthRole:  return m_tasks.at(index.row()).timeThisMonth.Minutes;
        case SecondsThisMonthRole:  return m_tasks.at(index.row()).timeThisMonth.Seconds;
        case ThisMonthStringRole:   return m_tasks.at(index.row()).timeThisMonthString;
        case HoursThisYearRole:     return m_tasks.at(index.row()).timeThisYear.Hours;
        case MinutesThisYearRole:   return m_tasks.at(index.row()).timeThisYear.Minutes;
        case SecondsThisYearRole:   return m_tasks.at(index.row()).timeThisYear.Seconds;
        case ThisYearStringRole:    return m_tasks.at(index.row()).timeThisYearString;
        case HoursDailyRole:        return m_tasks.at(index.row()).timeDaily.Hours;
        case MinutesDailyRole:      return m_tasks.at(index.row()).timeDaily.Minutes;
        case SecondsDailyRole:      return m_tasks.at(index.row()).timeDaily.Seconds;
        case elapsedSecDailyRole:   return m_tasks.at(index.row()).timeDaily.elapsedSeconds;
        case DailyStringRole:       return m_tasks.at(index.row()).timeDailyString;
        case HoursMonthlyRole:      return m_tasks.at(index.row()).timeMonthly.Hours;
        case MinutesMonthlyRole:    return m_tasks.at(index.row()).timeMonthly.Minutes;
        case SecondsMonthlyRole:    return m_tasks.at(index.row()).timeMonthly.Seconds;
        case elapsedSecMonthlyRole: return m_tasks.at(index.row()).timeMonthly.elapsedSeconds;
        case MonthlyStringRole:     return m_tasks.at(index.row()).timeMonthlyString;
        case HoursYearlyRole:       return m_tasks.at(index.row()).timeYearly.Hours;
        case MinutesYearlyRole:     return m_tasks.at(index.row()).timeYearly.Minutes;
        case SecondsYearlyRole:     return m_tasks.at(index.row()).timeYearly.Seconds;
        case elapsedSecYearlyRole:  return m_tasks.at(index.row()).timeYearly.elapsedSeconds;
        case YearlyStringRole:      return m_tasks.at(index.row()).timeYearlyString;
        // "timelog" should not be requested!
        default: return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> CTaskModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { TitleRole,              "title" },
        { DescriptionRole,        "description" },
        { taskIDRole,             "taskID" },
        { taskActiveRole,         "taskActive" },
        { allocateTimeRole,       "allocateTime" },
        { HoursTodayRole,         "HoursToday" },
        { MinutesTodayRole,       "MinutesToday" },
        { SecondsTodayRole,       "SecondsToday" },
        { elapsedSecTodayRole,    "elapsedSecToday" },
        { TodayStringRole,        "TodayString" },
        { HoursThisMonthRole,     "HoursThisMonth" },
        { MinutesThisMonthRole,   "MinutesThisMonth" },
        { SecondsThisMonthRole,   "SecondsThisMonth" },
        { ThisMonthStringRole,    "ThisMonthString" },
        { HoursThisYearRole,      "HoursThisYear" },
        { MinutesThisYearRole,    "MinutesThisYear" },
        { SecondsThisYearRole,    "SecondsThisYear" },
        { ThisYearStringRole,     "ThisYearString" },
        { HoursDailyRole,         "HoursDaily" },
        { MinutesDailyRole,       "MinutesDaily" },
        { SecondsDailyRole,       "SecondsDaily" },
        { elapsedSecDailyRole,    "elapsedSecDaily" },
        { DailyStringRole,        "DailyString" },
        { HoursMonthlyRole,       "HoursMonthly" },
        { MinutesMonthlyRole,     "MinutesMonthly" },
        { SecondsMonthlyRole,     "SecondsMonthly" },
        { elapsedSecMonthlyRole,  "elapsedSecMonthly" },
        { MonthlyStringRole,      "MonthlyString" },
        { HoursYearlyRole,        "HoursYearly" },
        { MinutesYearlyRole,      "MinutesYearly" },
        { SecondsYearlyRole,      "SecondsYearly" },
        { elapsedSecYearlyRole,   "elapsedSecYearly" },
        { YearlyStringRole,       "YearlyString" },
        // "timelog" should not be requested!
    };
    return roles;
}

// Custom functions:
QVariantMap CTaskModel::get(int row) const
{
//  Return the contents of an entry
    const Task task = m_tasks.value(row);
    return { {"title", task.title}, {"description", task.description},
             {"taskID", task.taskID}, {"taskActive", task.taskActive},
             {"allocateTime", task.allocateTime},
             {"HoursToday", task.timeToday.Hours}, {"MinutesToday", task.timeToday.Minutes}, {"SecondsToday", task.timeToday.Seconds}, {"elapsedSecToday", task.timeToday.elapsedSeconds}, {"TodayString", task.timeTodayString},
             {"HoursThisMonth", task.timeThisMonth.Hours}, {"MinutesThisMonth", task.timeThisMonth.Minutes}, {"SecondsThisMonth", task.timeThisMonth.Seconds}, {"ThisMonthString", task.timeThisMonthString},
             {"HoursThisYear", task.timeThisYear.Hours}, {"MinutesThisYear", task.timeThisYear.Minutes}, {"SecondsThisYear", task.timeThisYear.Seconds}, {"ThisYearString", task.timeThisYearString},
             {"HoursDaily", task.timeDaily.Hours}, {"MinutesDaily", task.timeDaily.Minutes}, {"SecondsDaily", task.timeDaily.Seconds}, {"elapsedSecDaily", task.timeDaily.elapsedSeconds}, {"DailyString", task.timeDailyString},
             {"HoursMonthly", task.timeMonthly.Hours}, {"MinutesMonthly", task.timeMonthly.Minutes}, {"SecondsMonthly", task.timeMonthly.Seconds}, {"elapsedSecMonthly", task.timeMonthly.elapsedSeconds}, {"MonthlyString", task.timeMonthlyString},
             {"HoursYearly", task.timeYearly.Hours}, {"MinutesYearly", task.timeYearly.Minutes}, {"SecondsYearly", task.timeYearly.Seconds}, {"elapsedSecYearly", task.timeYearly.elapsedSeconds}, {"YearlyString", task.timeYearlyString} };
}



void CTaskModel::checkFileType(QString File)
{
// Find out if the save file given in "File" is encrypted or not
    QFile   infile;

    if (File.isEmpty()) {
        qWarning("checkFileType(): No file name given!");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "checkFileType(): No file name given!" << endl;
    }
    else {
        newFile.SaveFileNameFull = File;
        newFile.SaveFileName     = File.section("/",-1);

        infile.setFileName(File);

        if (!infile.open(QIODevice::ReadOnly)) {
            qWarning("checkFileType(): Could not read save file - %s!",File.toUtf8().data());
            logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "checkFileType(): Could not read save file: " << File.toUtf8().data() << endl;
            newFile.SaveFileNameFull = "";
            newFile.SaveFileName     = "";
        }
        else {
            QDataStream in(&infile);
            in.setVersion(QDataStream::Qt_5_0);

            // Read magic number
            in >> newFile.magic_no;       //qInfo("Magic no. = %X",newFile.magic_no);
            if (newFile.magic_no == 0x051076B0) {
                newFile.encrypted = 1;
                m_PWneeded     = 1;
                emit PasswordNeeded();
            }
            else if (newFile.magic_no == 0x051076A0) {
                newFile.encrypted = 0;
                m_PWneeded     = 0;
            }
            else {
                qWarning("checkFileType(): Bad save file format - %s!",File.toUtf8().data());
                logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "checkFileType(): Bad save file format: " << File.toUtf8().data() << endl;
                newFile.SaveFileNameFull = "";
                newFile.SaveFileName     = "";
                m_PWneeded     = 0;
            }
            infile.close();
        }

        m_SaveFileName     = newFile.SaveFileName;
        m_SaveFileNameFull = newFile.SaveFileNameFull;
        // m_FileEncrypted does not need to be updated here
        //qInfo("SaveFileName = %s   SaveFileNameFull = %s",newFile.SaveFileName.toUtf8().data(),newFile.SaveFileNameFull.toUtf8().data());
        emit settingChanged();
    }

}

void CTaskModel::load_data(QString PW, QString File)
{
//  Find out if a data file is encrypted or not, then load it
    int     i, len, fileGood;
    uint8_t *pass = NULL;
    QFile   infile;

    newFile.Password = PW;
    //qInfo("load_data:  Password= %s   File= %s",newFile.Password.toUtf8().data(),File.toUtf8().data());

    memset(newFile.PasswordHashRead, 0, 32);
    fileGood = 1;

    // Find out if the save file is encrypted or not:
    infile.setFileName(File);

    if (!infile.open(QIODevice::ReadOnly)) {
        qWarning("load_data(): Could not read save file - %s!",File.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "load_data(): Could not read save file: " << File.toUtf8().data() << endl;
        newFile.SaveFileNameFull = "";
        newFile.SaveFileName     = "";
        fileGood = 0;
    }
    else {
        QDataStream in(&infile);
        in.setVersion(QDataStream::Qt_5_0);

        // Read magic number
        in >> newFile.magic_no;       //qInfo("Magic no. = %X",magic_no);
        if (newFile.magic_no == 0x051076B0) {
            newFile.encrypted = 1;
            m_PWneeded     = 1;
            fileGood = 1;
            emit PasswordNeeded();
        }
        else if (newFile.magic_no == 0x051076A0) {
            newFile.encrypted = 0;
            m_PWneeded     = 0;
            fileGood = 1;
        }
        else {
            qWarning("Bad save file format - %s!",File.toUtf8().data());
            logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "load_data(): Bad save file format: " << File.toUtf8().data() << endl;
            newFile.SaveFileNameFull = "";
            newFile.SaveFileName     = "";
            fileGood = 0;
        }
        infile.skip(2);   // Skip version no.
        // Read salt & password hash
        for (i=0; i<16; i++) {
            in >> newFile.salt[i];
        }
        for (i=0; i<32; i++) {
            in >> newFile.PasswordHashRead[i];
        }
        infile.close();
    }

    //qInfo("salt:  %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    //      salt[0],salt[1],salt[2],salt[3],salt[4],salt[5],salt[6],salt[7],salt[8],salt[9],salt[10],salt[11],salt[12],salt[13],salt[14],salt[15]);
    //qInfo("PasswordHash:  %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    //      PasswordHashRead[0],PasswordHashRead[1],PasswordHashRead[2],PasswordHashRead[3],PasswordHashRead[4],PasswordHashRead[5],PasswordHashRead[6],PasswordHashRead[7],PasswordHashRead[8],PasswordHashRead[9],PasswordHashRead[10],PasswordHashRead[11],PasswordHashRead[12],PasswordHashRead[13],PasswordHashRead[14],PasswordHashRead[15]);


    if (fileGood) {
        if (newFile.encrypted) {
            // Read the encrypted save file

            // Initialize key for save file encryption/decryption:
            memset(&key[0], 0, 32);
            len = 32;
            if (newFile.Password.length()<32) len = newFile.Password.length();   // Use only the first 32 Bytes

            // Hash the given password and compare with saved one:
            sha256.clear();
            sha256.update(newFile.Password.toUtf8().data(), len);
            sha256.finalize(&PasswordHash[0], 32);
            if (memcmp(&PasswordHash[0], &(newFile.PasswordHashRead[0]), 32) != 0) {
                qWarning("Warning: Incorrect password!");
                logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "load_data(): Incorrect password provided!" << endl;
                //newFile.SaveFileName     = "";
                //newFile.SaveFileNameFull = "";
                m_PWwrong = 1;
                emit WrongPassword();
            }
            else {
                pass = new uint8_t[len];
                memset(pass, 0, len);
                memcpy(pass, newFile.Password.toUtf8().data(), len);
                pkcs5_pbkdf2(pass, len, newFile.salt, 16, &key[0], 32, 4096);
                cbc.setKey(key, 32);

                cbc.setIV(key, 16);   // Actually we don't care about the IV on loading - the first block is discarded anyway

                if (!readfileEncrypted(File)) {
                    qWarning("Could not read encrypted save file %s!",File.toUtf8().data());
                    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "load_data(): Could not read encrypted save file: " << File.toUtf8().data() << endl;
                    newFile.SaveFileName     = "";
                    newFile.SaveFileNameFull = "";
                }
                else {
                    // Update the active file only when everything went OK
                    activeFile       = newFile;
                    m_FileEncrypted  = newFile.encrypted;
                    backupfile();
                }
            }
        }
        else {
            // Read the unencrypted save file
            if (!readfile(File)) {
                qWarning("Could not read save file %s!",File.toUtf8().data());
                logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "load_data(): Could not read save file: " << File.toUtf8().data() << endl;
                newFile.SaveFileName     = "";
                newFile.SaveFileNameFull = "";
            }
            else {
                // Update the active file only when everything went OK
                activeFile       = newFile;
                m_FileEncrypted  = newFile.encrypted;
                backupfile();
            }
        }
    }
    else {
        // File could not be read OR bad magic no.
        // We already have log file outputs for each of the cases before.
    }


    //checkEntries(5);   // Debug

    // Update all times displayed in main window:
    UpdateAll();

    m_SaveFileName     = newFile.SaveFileName;
    m_SaveFileNameFull = newFile.SaveFileNameFull;

    emit settingChanged();

    if (pass != NULL) delete[] pass;

}


int CTaskModel::save_data(QString File)
{
// The routine to save data to a file.
// Called from several positions in the QML
    int ret;

    // Update the filename of the active file:
    activeFile.SaveFileNameFull = File;
    activeFile.SaveFileName     = File.section("/",-1);
    m_SaveFileName              = activeFile.SaveFileName;
    m_SaveFileNameFull          = activeFile.SaveFileNameFull;
    emit settingChanged();

    if (!activeFile.SaveFileNameFull.isEmpty()) {
        if (activeFile.encrypted) {
            ret = writefileEncrypted();
        }
        else {
            ret = writefile();
        }
        if (ret != 0) return ret;
    }
    else {
        qInfo("save_data(): SaveFileNameFull empty - no save file written!");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "save_data(): SaveFileNameFull empty - no save file written!" << endl;
    }

    return 0;
}


void CTaskModel::set_password(QString PW)
{
// Set the password for the active file
// If the file was previously unencrypted, it will be saved in encrypted format immediately
// If the file was already encrypted, it will be re-encrypted with the new password (even if the password is the same, since a new salt is generated)
    quint32  random_number;
    int      i;

    activeFile.encrypted  = 1;
    activeFile.Password   = PW;
    // Generate a new salt for this password:
    for (i=0; i<16; i+=4) {
        random_number = QRandomGenerator::global()->generate();
        memcpy(&(activeFile.salt[i]), &random_number, 4);
    }

    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "set_password(): Password changed." << endl;

    // Save the file
    if (!activeFile.SaveFileNameFull.isEmpty()) {
        writefileEncrypted();
    }
    else {
        qInfo("set_password(): SaveFileNameFull empty - no save file written!");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "set_password(): SaveFileNameFull empty - no save file written!" << endl;
    }

    m_FileEncrypted    = activeFile.encrypted;
    emit settingChanged();

}


void CTaskModel::removeEncryption()
{
// Remove encryption from the active file
    uint8_t old_format;

    old_format = activeFile.encrypted;

    activeFile.encrypted  = 0;

    // Save the file if the format was changed:
    if (activeFile.encrypted != old_format) {
        if (!activeFile.SaveFileNameFull.isEmpty()) {
            writefile();
        }
        else {
            qInfo("removeEncryption(): SaveFileNameFull empty - no save file written!");
            logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "removeEncryption(): SaveFileNameFull empty - no save file written!" << endl;
        }
    }
    else {
        //qInfo("File already unencrypted - no save file written!");
    }

    m_FileEncrypted    = activeFile.encrypted;
    emit settingChanged();

}



int CTaskModel::append(const QString &title, const QString &description)
{
//  Add a new empty entry at the end of the task list (unsorted)
    QMap<QDate, sTime> entry;
    sTime t={0,0,0,0};
    QString tString = "00:00:00";
    quint16 UID;
    int     i;
    quint8  flag;

    entry.insert(today, t);

    int row = m_tasks.count();

    // Generate 16 Bit random UID
    UID = QRandomGenerator::global()->generate() >> 16;
    // Make sure the UID is not used yet:
    flag = 1;
    while (flag==1) {
        flag = 0;
        for (i=0; i<m_tasks.count(); i++) {
            if (m_tasks.at(i).taskID == UID) {
                flag = 1;
            }
        }
        if (flag==1) {
            UID = QRandomGenerator::global()->generate() >> 16;
        }
    }
    //qInfo("UID: %u",UID);

    beginInsertRows(QModelIndex(), row, row);
    m_tasks.insert(row, {title, description, (quint32)UID, 0, 0, t, t, tString, t, tString, t, tString, t, tString, t, tString, t, tString, entry});
    endInsertRows();
    return 0;
}


void CTaskModel::set(int row, const QString &title, const QString &description)
{
//  Update title or description of an entry
    if (row < 0 || row >= m_tasks.count())
        return;

    m_tasks.replace(row, { title, description, m_tasks.at(row).taskID, m_tasks.at(row).taskActive, m_tasks.at(row).allocateTime, m_tasks.at(row).timeTotal,
                           m_tasks.at(row).timeToday, m_tasks.at(row).timeTodayString, m_tasks.at(row).timeThisMonth, m_tasks.at(row).timeThisMonthString, m_tasks.at(row).timeThisYear, m_tasks.at(row).timeThisYearString,
                           m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString, m_tasks.at(row).timelog });
    dataChanged(index(row, 0), index(row, 0), { TitleRole, DescriptionRole });
}


void CTaskModel::updateEntry(int row, const sTime timeTotal, const sTime timeToday, const QString timeTodayString, const sTime timeThisMonth, const QString timeThisMonthString, const sTime timeThisYear, const QString timeThisYearString)
{
//  Update time contents of an entry for today
//  Note: timeDaily, timeMonthly and timeYearly are updated in updateDailyList() / updateMonthlyList() / updateYearlyList()

    if (row < 0 || row >= m_tasks.count())
        return;

    m_tasks[row].timelog.insert(today, timeToday);   // Overwrites existing entries with new value

    m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description, m_tasks.at(row).taskID, m_tasks.at(row).taskActive, m_tasks.at(row).allocateTime, timeTotal,
                           timeToday, timeTodayString, timeThisMonth, timeThisMonthString, timeThisYear, timeThisYearString,
                           m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString, m_tasks.at(row).timelog });
    dataChanged(index(row, 0), index(row, 0), { taskActiveRole, HoursTodayRole, MinutesTodayRole, SecondsTodayRole, elapsedSecTodayRole, TodayStringRole,
                                                HoursThisMonthRole, MinutesThisMonthRole, SecondsThisMonthRole, ThisMonthStringRole, HoursThisYearRole, MinutesThisYearRole, SecondsThisYearRole, ThisYearStringRole });
}


void CTaskModel::updateEntry2(int row, const QDate date, const sTime timeToDate)
{
//  Update time contents of an arbitrary "date" in an arbitrary entry "row" of the task list to "timeToDate"
//
//  row: Entry in task list to update
//  date: Date of entry to update
//  timeToDate: New time with which to update "date"
    int     n;
    sTime   totalTime;
    sTime   todayTime;
    sTime   thisMonthTime;
    QString thisMonthTimeString;
    QString todayTimeString;
    sTime   thisYearTime;
    QString thisYearTimeString;
    QMap<QDate, sTime>::const_iterator it;

    if (row < 0 || row >= m_tasks.count())
        return;

    m_tasks[row].timelog.insert(date, timeToDate);   // Overwrites existing entries with new value

    if (date == QDate::currentDate()) {
        // If the date to be changed is today, we need to update timeToday for the display:
        todayTime       = timeToDate;
        todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
    }
    else {
        // Otherwise, leave it untouched:
        todayTime       = m_tasks.at(row).timeToday;
        todayTimeString = m_tasks.at(row).timeTodayString;
    }

    // Replace only timelog with updated entry:
    m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description, m_tasks.at(row).taskID, m_tasks.at(row).taskActive, m_tasks.at(row).allocateTime, m_tasks.at(row).timeTotal,
                           todayTime, todayTimeString, m_tasks.at(row).timeThisMonth, m_tasks.at(row).timeThisMonthString, m_tasks.at(row).timeThisYear, m_tasks.at(row).timeThisYearString,
                           m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString, m_tasks.at(row).timelog });
    dataChanged(index(row, 0), index(row, 0), { HoursTodayRole, MinutesTodayRole, SecondsTodayRole, elapsedSecTodayRole, TodayStringRole });



    // Update this month's time for this task:
    thisMonthTime.elapsedSeconds = 0;
    // Iterate over all daily entries:
    it = m_tasks.at(row).timelog.begin();
    for (n = 0; n < m_tasks.at(row).timelog.size(); n++) {
        // If entry corresponds to queried month, add its time:
        if (it.key().month() == QDate::currentDate().month()) {
            thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
        }
        it++;
    }
    thisMonthTime.Hours    = thisMonthTime.elapsedSeconds / 3600;
    thisMonthTime.Minutes  = (thisMonthTime.elapsedSeconds-thisMonthTime.Hours*3600) / 60;
    thisMonthTime.Seconds  = thisMonthTime.elapsedSeconds-(thisMonthTime.Hours*3600 + thisMonthTime.Minutes*60);
    thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));


    // Update this year's time for this task:
    thisYearTime.elapsedSeconds = 0;
    // Iterate over all daily entries:
    it = m_tasks.at(row).timelog.begin();
    for (n = 0; n < m_tasks.at(row).timelog.size(); n++) {
        // If entry corresponds to queried year, add its time:
        if (it.key().year() == QDate::currentDate().year()) {
            thisYearTime.elapsedSeconds += it.value().elapsedSeconds;
        }
        it++;
    }
    thisYearTime.Hours    = thisYearTime.elapsedSeconds / 3600;
    thisYearTime.Minutes  = (thisYearTime.elapsedSeconds-thisYearTime.Hours*3600) / 60;
    thisYearTime.Seconds  = thisYearTime.elapsedSeconds-(thisYearTime.Hours*3600 + thisYearTime.Minutes*60);
    thisYearTimeString = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));


    // Update total time for this task:
    totalTime.elapsedSeconds = 0;
    // Iterate over all daily entries:
    it = m_tasks.at(row).timelog.begin();
    for (n = 0; n < m_tasks.at(row).timelog.size(); n++) {
        totalTime.elapsedSeconds += it.value().elapsedSeconds;
        it++;
    }
    totalTime.Hours    = totalTime.elapsedSeconds / 3600;
    totalTime.Minutes  = (totalTime.elapsedSeconds-totalTime.Hours*3600) / 60;
    totalTime.Seconds  = totalTime.elapsedSeconds-(totalTime.Hours*3600 + totalTime.Minutes*60);


    // Update again for the remaining times:
    m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description, m_tasks.at(row).taskID, m_tasks.at(row).taskActive, m_tasks.at(row).allocateTime, totalTime,
                           m_tasks.at(row).timeToday, m_tasks.at(row).timeTodayString, thisMonthTime, thisMonthTimeString, thisYearTime, thisYearTimeString,
                           m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString, m_tasks.at(row).timelog });
    dataChanged(index(row, 0), index(row, 0), { HoursTodayRole, MinutesTodayRole, SecondsTodayRole, elapsedSecTodayRole, TodayStringRole,
                                                HoursThisMonthRole, MinutesThisMonthRole, SecondsThisMonthRole, ThisMonthStringRole,
                                                HoursThisYearRole, MinutesThisYearRole, SecondsThisYearRole, ThisYearStringRole });

}



void CTaskModel::sort(int column, Qt::SortOrder order)
{
//  Sort the task list
//  column = 0:  Sort by title
//  column = 1:  Sort by daily elapsed seconds
//  column = 2:  Sort by monthly elapsed seconds
//  column = 3:  Sort by yearly elapsed seconds
//  column = 4:  Sort by total elapsed seconds
    int   i;
    int   n;
    bool swapped = true;


    // Bubble Sort
    n = m_tasks.count();
    switch(column) {
    case 0:
        // Sort by "title"
        while (swapped) {
            swapped = false;
            for (i=1; i<=n-1; i++)
            {
                //printf("i = %d - title(i-1): %s - titel(i): %s\n",i,m_tasks.at(i-1).title.toUtf8().data(),m_tasks.at(i).title.toUtf8().data());
                if (order==Qt::AscendingOrder) {
                    if (m_tasks.at(i-1).title > m_tasks.at(i).title)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
                else {
                    if (m_tasks.at(i-1).title < m_tasks.at(i).title)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
            }
            n--;
        }
        break;
    case 1:
        // Sort by daily "elapsedSeconds"
        while (swapped) {
            swapped = false;
            for (i=1; i<=n-1; i++)
            {
                //printf("i = %d - title(i-1): %s - titel(i): %s\n",i,m_tasks.at(i-1).title.toUtf8().data(),m_tasks.at(i).title.toUtf8().data());
                if (order==Qt::AscendingOrder) {
                    if (m_tasks.at(i-1).timeToday.elapsedSeconds > m_tasks.at(i).timeToday.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
                else {
                    if (m_tasks.at(i-1).timeToday.elapsedSeconds < m_tasks.at(i).timeToday.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
            }
            n--;
        }
        break;
    case 2:
        // Sort by monthly "elapsedSeconds"
        while (swapped) {
            swapped = false;
            for (i=1; i<=n-1; i++)
            {
                //printf("i = %d - title(i-1): %s - titel(i): %s\n",i,m_tasks.at(i-1).title.toUtf8().data(),m_tasks.at(i).title.toUtf8().data());
                if (order==Qt::AscendingOrder) {
                    if (m_tasks.at(i-1).timeThisMonth.elapsedSeconds > m_tasks.at(i).timeThisMonth.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
                else {
                    if (m_tasks.at(i-1).timeThisMonth.elapsedSeconds < m_tasks.at(i).timeThisMonth.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
            }
            n--;
        }
        break;
    case 3:
        // Sort by yearly "elapsedSeconds"
        while (swapped) {
            swapped = false;
            for (i=1; i<=n-1; i++)
            {
                //printf("i = %d - title(i-1): %s - titel(i): %s\n",i,m_tasks.at(i-1).title.toUtf8().data(),m_tasks.at(i).title.toUtf8().data());
                if (order==Qt::AscendingOrder) {
                    if (m_tasks.at(i-1).timeThisYear.elapsedSeconds > m_tasks.at(i).timeThisYear.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
                else {
                    if (m_tasks.at(i-1).timeThisYear.elapsedSeconds < m_tasks.at(i).timeThisYear.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
            }
            n--;
        }
        break;
    case 4:
        // Sort by total "elapsedSeconds"
        while (swapped) {
            swapped = false;
            for (i=1; i<=n-1; i++)
            {
                //printf("i = %d - title(i-1): %s - titel(i): %s\n",i,m_tasks.at(i-1).title.toUtf8().data(),m_tasks.at(i).title.toUtf8().data());
                if (order==Qt::AscendingOrder) {
                    if (m_tasks.at(i-1).timeTotal.elapsedSeconds > m_tasks.at(i).timeTotal.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
                else {
                    if (m_tasks.at(i-1).timeTotal.elapsedSeconds < m_tasks.at(i).timeTotal.elapsedSeconds)
                    {
                        beginMoveRows(QModelIndex(), i, i, QModelIndex(), i-1);
                        m_tasks.swap(i-1,i);   // Need to swap the underlying data manually!
                        endMoveRows();
                        swapped = true;
                    }
                }
            }
            n--;
        }
        break;
    }

}


int CTaskModel::row(qint32 taskID)
{
//  Return the row corresponding to a given taskID
//  Returns -1 if taskID not found
    int i;

    i= 0;
    while(i < m_tasks.count()) {
        if (m_tasks.at(i).taskID==taskID) {
            return i;
        }
        i++;
    }

    return -1;
}


void CTaskModel::startTimer(int row)
{
//  (Re)start timer for entry

    // Memorize the currently active ID:
    activeID     = m_tasks.at(row).taskID;

    // Set the corresponding task to "Active":
    m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description,
                           m_tasks.at(row).taskID, 1,
                           m_tasks.at(row).allocateTime,
                           m_tasks.at(row).timeTotal,
                           m_tasks.at(row).timeToday, m_tasks.at(row).timeTodayString, m_tasks.at(row).timeThisMonth, m_tasks.at(row).timeThisMonthString, m_tasks.at(row).timeThisYear, m_tasks.at(row).timeThisYearString,
                           m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString,
                           m_tasks.at(row).timelog });
    dataChanged(index(row, 0), index(row, 0), { taskActiveRole });

    // Start timer with a 1 sec. interval:
    timer.start(1000);

}


void CTaskModel::stopTimer(int row)
{
//  Stop a running timer

    // Set the corresponding task to "Inactive":
    m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description,
                           m_tasks.at(row).taskID, 0,
                           m_tasks.at(row).allocateTime,
                           m_tasks.at(row).timeTotal,
                           m_tasks.at(row).timeToday, m_tasks.at(row).timeTodayString, m_tasks.at(row).timeThisMonth, m_tasks.at(row).timeThisMonthString, m_tasks.at(row).timeThisYear, m_tasks.at(row).timeThisYearString,
                           m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString,
                           m_tasks.at(row).timelog });
    dataChanged(index(row, 0), index(row, 0), { taskActiveRole });

    // Stop timer
    timer.stop();

    // Reset active ID:
    activeID = -1;
    activeRow = -1;

}


void CTaskModel::switchAllocate(int row)
{
//  Switch the flag marking the task as target for time allocation
    quint8  flag;

    flag     = m_tasks.at(row).allocateTime;

    if (flag==1) {
        m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description,
                               m_tasks.at(row).taskID, m_tasks.at(row).taskActive,
                               0,
                               m_tasks.at(row).timeTotal,
                               m_tasks.at(row).timeToday, m_tasks.at(row).timeTodayString, m_tasks.at(row).timeThisMonth, m_tasks.at(row).timeThisMonthString, m_tasks.at(row).timeThisYear, m_tasks.at(row).timeThisYearString,
                               m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString,
                               m_tasks.at(row).timelog });
    }
    else {
        m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description,
                               m_tasks.at(row).taskID, m_tasks.at(row).taskActive,
                               1,
                               m_tasks.at(row).timeTotal,
                               m_tasks.at(row).timeToday, m_tasks.at(row).timeTodayString, m_tasks.at(row).timeThisMonth, m_tasks.at(row).timeThisMonthString, m_tasks.at(row).timeThisYear, m_tasks.at(row).timeThisYearString,
                               m_tasks.at(row).timeDaily, m_tasks.at(row).timeDailyString, m_tasks.at(row).timeMonthly, m_tasks.at(row).timeMonthlyString, m_tasks.at(row).timeYearly, m_tasks.at(row).timeYearlyString,
                               m_tasks.at(row).timelog });
    }
    dataChanged(index(row, 0), index(row, 0), { allocateTimeRole });

}


void CTaskModel::resetAllocate()
{
//  Reset all flags marking tasks as target for time allocation
    int i;

    // Iterate over all tasks:
    for (i=0; i<m_tasks.count(); i++) {
        m_tasks.replace(i, { m_tasks.at(i).title, m_tasks.at(i).description,
                               m_tasks.at(i).taskID, m_tasks.at(i).taskActive,
                               0,
                               m_tasks.at(i).timeTotal,
                               m_tasks.at(i).timeToday, m_tasks.at(i).timeTodayString, m_tasks.at(i).timeThisMonth, m_tasks.at(i).timeThisMonthString, m_tasks.at(i).timeThisYear, m_tasks.at(i).timeThisYearString,
                               m_tasks.at(i).timeDaily, m_tasks.at(i).timeDailyString, m_tasks.at(i).timeMonthly, m_tasks.at(i).timeMonthlyString, m_tasks.at(i).timeYearly, m_tasks.at(i).timeYearlyString,
                               m_tasks.at(i).timelog });
        dataChanged(index(i, 0), index(i, 0), { allocateTimeRole });
    }

}


void CTaskModel::reallocate(int ID, qint16 day, quint16 weighted)
{
//  Reallocate time of taskID "ID" on "day" to targets marked for time allocation
//  ID: Source Task ID in current list
//  day: Day currently displayed (relative days from current date)
//  weighted: Reallocate time weighted by time on target task (2) or equally (0)
    int i, n, row, pos;
    QDate date;
    quint32 sec_to_allocate;
    quint32 fraction;
    quint16 no_of_targets;
    sTime   newTime;
    quint32 elapsedSeconds, targetSecondsSum;
    QList<quint32> targetSeconds;
    QMap<QDate, sTime>::const_iterator it;


    // Determine the date on which we work:
    date = QDate::currentDate();
    date = date.addDays(day);

    // Find row to work on:
    row = -1;
    for (i=0; i<m_tasks.count(); i++) {
        if (m_tasks.at(i).taskID==ID) row = i;
    }
    if (row == -1) {
        qWarning("CTaskModel::reallocate(): Could not find task ID!\n");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "reallocate(): Could not find task ID!" << endl;
    }

    // Get seconds to be reallocated:
    sec_to_allocate = 0;
    it = m_tasks.at(row).timelog.begin();
    // Iterate over all daily entries:
    for (n = 0; n < m_tasks.at(row).timelog.size(); n++) {
        // If entry corresponds to queried date, get its time:
        if (it.key() == date) {
            sec_to_allocate = it.value().elapsedSeconds;
        }
        it++;
    }

    // First iteration over all tasks: Count number of targets
    no_of_targets = 0;
    for (i=0; i<m_tasks.count(); i++) {
        if (m_tasks.at(i).allocateTime==1) {
            no_of_targets++;

            // Save the current time stored in each target task (for weighted allocation):
            it = m_tasks.at(i).timelog.begin();
            for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
                if (it.key() == date) {
                    targetSeconds.append(it.value().elapsedSeconds);
                }
                it++;
            }
        }
    }

    // Calculate sum of current times stored in target tasks:
    targetSecondsSum = 0;
    for (i=0; i<targetSeconds.count(); i++) {
        targetSecondsSum += targetSeconds.at(i);
    }
    //qInfo("targetSecondsSum = %d\n",targetSecondsSum);

    if (targetSecondsSum==0) {
        // Force equal reallocation if all target tasks have 0 time:
        weighted = 0;
    }

    if (no_of_targets==0) {
        qWarning("CTaskModel::reallocate(%d): No targets to allocate to!\n",row);
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "reallocate(): No targets to allocate to!" << endl;
        sec_to_allocate = 0;
    }
    else {
        // Remove time from source task:
        newTime.elapsedSeconds = 0;
        newTime.Hours          = 0;
        newTime.Minutes        = 0;
        newTime.Seconds        = 0;

        updateEntry2(row, date, newTime);


        // Second iteration over all tasks: Distribute time
        pos = 0;
        for (i=0; i<m_tasks.count(); i++) {
            elapsedSeconds = 0;

            if (m_tasks.at(i).allocateTime==1) {

                // Iterate over all daily entries:
                it = m_tasks.at(i).timelog.begin();
                for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
                    // If entry corresponds to queried day, get its time:
                    if (it.key() == date) {
                        elapsedSeconds = it.value().elapsedSeconds;
                    }
                    it++;
                }

                // Allocate time to target task:
                // Weighted:
                if (weighted >= 1) {
                    fraction = lround(qreal(targetSeconds.at(pos)) / qreal(targetSecondsSum) * sec_to_allocate);
                    //qInfo("pos = %d    targetSeconds.at(pos) = %d   fraction = %d\n",pos,targetSeconds.at(pos),fraction);
                    elapsedSeconds         += fraction;
                    newTime.elapsedSeconds =  elapsedSeconds;
                    newTime.Hours          =  newTime.elapsedSeconds / 3600;
                    newTime.Minutes        = (newTime.elapsedSeconds-newTime.Hours*3600) / 60;
                    newTime.Seconds        =  newTime.elapsedSeconds-(newTime.Hours*3600 + newTime.Minutes*60);
                }
                else {
                    // Equal:
                    fraction = lround(qreal(sec_to_allocate) / qreal(no_of_targets));
                    //qInfo("sec_to_allocate = %d  no_of_targets = %d   fraction = %d\n ",sec_to_allocate,no_of_targets,fraction);
                    sec_to_allocate -= fraction;
                    no_of_targets   -= 1;
                    elapsedSeconds         += fraction;
                    newTime.elapsedSeconds =  elapsedSeconds;
                    newTime.Hours          =  newTime.elapsedSeconds / 3600;
                    newTime.Minutes        = (newTime.elapsedSeconds-newTime.Hours*3600) / 60;
                    newTime.Seconds        =  newTime.elapsedSeconds-(newTime.Hours*3600 + newTime.Minutes*60);
                }

                updateEntry2(i, date, newTime);

                pos++;
            }
        }

    }    // if (no_of_targets==0)


    // Update total time over all tasks in main window:
    updateTotals();
    // Update the daily report (this function cannot be called from monthly and yearly reports):
    updateDailyList(day);

}


void CTaskModel::reallocateAll(int ID, qint16 day, quint16 weighted)
{
//  Reallocate time of taskID "ID" for all days to targets marked for time allocation
//  Weighted reallocation is performed for each single day, i.e. only the target tasks' time for each day is considered in the weighting
//  ID: Source Task ID in current list
//  day: Day currently displayed (relative days from current date) (only used for update of display)
//  weighted: Reallocate time weighted by time on target task (2) or equally (0) ("2" due to CheckBox properties)
    int i, k, row, pos;
    QDate date;
    quint32 sec_to_allocate;
    quint32 fraction;
    quint16 no_of_targets;
    sTime   newTime;
    quint32 elapsedSeconds, targetSecondsSum;
    QList<quint32> targetSeconds;
    QMap<QDate, sTime>::const_iterator it1, it2, it3;


    // Find row (task) to work on:
    row = -1;
    for (i=0; i<m_tasks.count(); i++) {
        if (m_tasks.at(i).taskID==ID) row = i;
    }
    if (row == -1) {
        qWarning("CTaskModel::reallocateAll(): Could not find task ID!\n");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "reallocateAll(): Could not find task ID!" << endl;
    }

    // Get seconds to be reallocated:
    sec_to_allocate = 0;
    // Iterate over all daily entries:
    //qInfo("timelog.size() = %d",m_tasks.at(row).timelog.size());
    for (it1 = m_tasks.at(row).timelog.constBegin(); it1 != m_tasks.at(row).timelog.constEnd(); ++it1) {
        date = it1.key();
        sec_to_allocate = it1.value().elapsedSeconds;

        //qInfo("Date: %s",date.toString("dd.MM.yyyy").toUtf8().data());

        // First iteration over all tasks: Count number of targets
        targetSeconds.clear();
        no_of_targets = 0;
        for (k=0; k<m_tasks.count(); k++) {
            if (m_tasks.at(k).allocateTime==1) {
                no_of_targets++;

                // Save the current time stored in each target task (for weighted allocation):
                for (it2 = m_tasks.at(k).timelog.constBegin(); it2 != m_tasks.at(k).timelog.constEnd(); ++it2) {
                    if (it2.key() == date) {
                        targetSeconds.append(it2.value().elapsedSeconds);
                    }
                }
                if (targetSeconds.isEmpty()) {
                    // Date does not yet have an entry in this task - append with 0 time
                    //qInfo("No entry found for target task on this date...");
                    targetSeconds.append(0);
                }

            }
        }

        // Calculate sum of current times stored in target tasks:
        targetSecondsSum = 0;
        for (i=0; i<targetSeconds.count(); i++) {
            targetSecondsSum += targetSeconds.at(i);
        }
        //qInfo("targetSecondsSum = %d",targetSecondsSum);

        if (targetSecondsSum==0) {
            // Force equal reallocation if all target tasks have 0 time:
            weighted = 0;
        }

        if (no_of_targets==0) {
            qWarning("CTaskModel::reallocateAll(%d): No targets to allocate to!",row);
            logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "reallocateAll(): No targets to allocate to!" << endl;
            sec_to_allocate = 0;
        }
        else {
            // Remove time from source task:
            newTime.elapsedSeconds = 0;
            newTime.Hours          = 0;
            newTime.Minutes        = 0;
            newTime.Seconds        = 0;

            updateEntry2(row, date, newTime);

            // Second iteration over all tasks: Distribute time
            pos = 0;
            for (i=0; i<m_tasks.count(); i++) {
                elapsedSeconds = 0;

                if (m_tasks.at(i).allocateTime==1) {

                    // Iterate over all daily entries:
                    for (it3 = m_tasks.at(i).timelog.constBegin(); it3 != m_tasks.at(i).timelog.constEnd(); ++it3) {
                        // If entry corresponds to queried day, get its time:
                        if (it3.key() == date) {
                            elapsedSeconds = it3.value().elapsedSeconds;
                        }
                    }

                    // Allocate time to target task:
                    // Weighted:
                    if (weighted >= 1) {
                        fraction = lround(qreal(targetSeconds.at(pos)) / qreal(targetSecondsSum) * sec_to_allocate);
                        //qInfo("pos = %d    sec_to_allocate = %d   targetSeconds.at(pos) = %d   fraction = %d",pos,sec_to_allocate,targetSeconds.at(pos),fraction);
                        elapsedSeconds         += fraction;
                        newTime.elapsedSeconds =  elapsedSeconds;
                        newTime.Hours          =  newTime.elapsedSeconds / 3600;
                        newTime.Minutes        = (newTime.elapsedSeconds-newTime.Hours*3600) / 60;
                        newTime.Seconds        =  newTime.elapsedSeconds-(newTime.Hours*3600 + newTime.Minutes*60);
                    }
                    else {
                        // Equal:
                        fraction = lround(qreal(sec_to_allocate) / qreal(no_of_targets));
                        //qInfo("pos = %d    sec_to_allocate = %d   no_of_targets = %d   fraction = %d",pos,sec_to_allocate,no_of_targets,fraction);
                        sec_to_allocate -= fraction;
                        no_of_targets   -= 1;
                        elapsedSeconds         += fraction;
                        newTime.elapsedSeconds =  elapsedSeconds;
                        newTime.Hours          =  newTime.elapsedSeconds / 3600;
                        newTime.Minutes        = (newTime.elapsedSeconds-newTime.Hours*3600) / 60;
                        newTime.Seconds        =  newTime.elapsedSeconds-(newTime.Hours*3600 + newTime.Minutes*60);
                    }

                    updateEntry2(i, date, newTime);

                    //qInfo("pos = %d - Finished\n",pos);

                    pos++;
                }
            }

        }    // if (no_of_targets==0)

    }


    // Update total time over all tasks in main window:
    updateTotals();
    // Update the daily report (this function cannot be called from monthly and yearly reports):
    updateDailyList(day);

}


void CTaskModel::add_time(int row, qint16 day, int hours, int minutes, int seconds)
{
//  Add time to task in "row" at "day"
//  row: Task in current list
//  day: Relative days from current date
//  hours: Hours to add
//  minutes: Minutes to add
//  seconds: Seconds to add
    int n;
    QDate date;
    sTime   newTime;
    qint32 elapsedSeconds;
    QMap<QDate, sTime>::const_iterator it;


    // Determine the date on which we work:
    date = QDate::currentDate();
    date = date.addDays(day);


    // Calculate the new time for that date:
    newTime.elapsedSeconds = 0;
    it = m_tasks.at(row).timelog.begin();
    // Iterate over all daily entries:
    for (n = 0; n < m_tasks.at(row).timelog.size(); n++) {
        // If entry corresponds to queried date, get its time:
        if (it.key() == date) {
            newTime.elapsedSeconds = it.value().elapsedSeconds;
        }
        it++;
    }

    elapsedSeconds = newTime.elapsedSeconds;   // Cast to a signed integer so we can compare to 0
    if ((elapsedSeconds + (hours*3600 + minutes*60 + seconds)) < 0) {
        // Limit to 0:
        newTime.elapsedSeconds = 0;
    }
    else {
        newTime.elapsedSeconds += hours*3600 + minutes*60 + seconds;
    }
    newTime.Hours   =  newTime.elapsedSeconds / 3600;
    newTime.Minutes = (newTime.elapsedSeconds-newTime.Hours*3600) / 60;
    newTime.Seconds =  newTime.elapsedSeconds-(newTime.Hours*3600 + newTime.Minutes*60);

    updateEntry2(row, date, newTime);

    // Update total time over all tasks in main window:
    updateTotals();
    // Update the daily report (this function cannot be called from monthly and yearly reports):
    updateDailyList(day);

}


void CTaskModel::updateTotals()
{
// Update total time today / this month / this year (main window)
    int i, n;
    QMap<QDate, sTime>::const_iterator it;

    totalTimeToday.elapsedSeconds     = 0;
    totalTimeThisMonth.elapsedSeconds = 0;
    totalTimeThisYear.elapsedSeconds  = 0;
    // Iterate over all tasks:
    for (i=0; i<m_tasks.count(); i++) {
        it = m_tasks.at(i).timelog.begin();
        // Iterate over all daily entries:
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            // If entry corresponds to queried date, add its time:
            if (it.key() == QDate::currentDate()) {
                totalTimeToday.elapsedSeconds += it.value().elapsedSeconds;
            }
            if (it.key().month() == QDate::currentDate().month()) {
                totalTimeThisMonth.elapsedSeconds += it.value().elapsedSeconds;
            }
            if (it.key().year() == QDate::currentDate().year()) {
                totalTimeThisYear.elapsedSeconds += it.value().elapsedSeconds;
            }
            it++;
        }
    }

    totalTimeToday.Hours    = totalTimeToday.elapsedSeconds / 3600;
    totalTimeToday.Minutes  = (totalTimeToday.elapsedSeconds-totalTimeToday.Hours*3600) / 60;
    totalTimeToday.Seconds  = totalTimeToday.elapsedSeconds-(totalTimeToday.Hours*3600 + totalTimeToday.Minutes*60);
    m_TotalHoursToday   = totalTimeToday.Hours;
    m_TotalMinutesToday = totalTimeToday.Minutes;
    m_TotalSecondsToday = totalTimeToday.Seconds;
    m_TotalTimeToday    = QString("%1:%2:%3").arg(m_TotalHoursToday,2,10,QLatin1Char('0')).arg(m_TotalMinutesToday,2,10,QLatin1Char('0')).arg(m_TotalSecondsToday,2,10,QLatin1Char('0'));

    totalTimeThisMonth.Hours    = totalTimeThisMonth.elapsedSeconds / 3600;
    totalTimeThisMonth.Minutes  = (totalTimeThisMonth.elapsedSeconds-totalTimeThisMonth.Hours*3600) / 60;
    totalTimeThisMonth.Seconds  = totalTimeThisMonth.elapsedSeconds-(totalTimeThisMonth.Hours*3600 + totalTimeThisMonth.Minutes*60);
    m_TotalHoursThisMonth   = totalTimeThisMonth.Hours;
    m_TotalMinutesThisMonth = totalTimeThisMonth.Minutes;
    m_TotalSecondsThisMonth = totalTimeThisMonth.Seconds;
    m_TotalTimeThisMonth    = QString("%1:%2:%3").arg(m_TotalHoursThisMonth,2,10,QLatin1Char('0')).arg(m_TotalMinutesThisMonth,2,10,QLatin1Char('0')).arg(m_TotalSecondsThisMonth,2,10,QLatin1Char('0'));

    totalTimeThisYear.Hours    = totalTimeThisYear.elapsedSeconds / 3600;
    totalTimeThisYear.Minutes  = (totalTimeThisYear.elapsedSeconds-totalTimeThisYear.Hours*3600) / 60;
    totalTimeThisYear.Seconds  = totalTimeThisYear.elapsedSeconds-(totalTimeThisYear.Hours*3600 + totalTimeThisYear.Minutes*60);
    m_TotalHoursThisYear   = totalTimeThisYear.Hours;
    m_TotalMinutesThisYear = totalTimeThisYear.Minutes;
    m_TotalSecondsThisYear = totalTimeThisYear.Seconds;
    m_TotalTimeThisYear    = QString("%1:%2:%3").arg(m_TotalHoursThisYear,2,10,QLatin1Char('0')).arg(m_TotalMinutesThisYear,2,10,QLatin1Char('0')).arg(m_TotalSecondsThisYear,2,10,QLatin1Char('0'));

    emit TimeChanged();

}


void CTaskModel::updateYearlyList(quint16 year)
{
// Update hours per year for the given year
    int i, n, m, flag;
    quint32 elapsedSeconds;
    quint16 TotalHours, TotalMinutes, TotalSeconds;
    sTime   yearlyTime;
    QString yearlyTimeString;
    QMap<QDate, sTime>::const_iterator it;

    m_totalSecondsYearly = 0;
    m_daysWorkedYearly   = 0;

    // Iterate over all tasks:
    for (i=0; i<m_tasks.count(); i++) {
        // Reset yearly time to 0:
        elapsedSeconds = 0;

        // Iterate over all daily entries:
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            // If entry corresponds to queried year, add its time:
            if (it.key().year() == year) {
                elapsedSeconds += it.value().elapsedSeconds;
            }
            it++;
        }

        // Update timeYearly in the Task:
        yearlyTime.Hours   =  elapsedSeconds / 3600;
        yearlyTime.Minutes = (elapsedSeconds-yearlyTime.Hours*3600) / 60;
        yearlyTime.Seconds =  elapsedSeconds-(yearlyTime.Hours*3600 + yearlyTime.Minutes*60);
        yearlyTime.elapsedSeconds = elapsedSeconds;
        yearlyTimeString = QString("%1:%2:%3").arg(yearlyTime.Hours,2,10,QLatin1Char('0')).arg(yearlyTime.Minutes,2,10,QLatin1Char('0')).arg(yearlyTime.Seconds,2,10,QLatin1Char('0'));

        // Sum up monthly times for monthly total:
        m_totalSecondsYearly += yearlyTime.elapsedSeconds;

        m_tasks.replace(i, { m_tasks.at(i).title, m_tasks.at(i).description, m_tasks.at(i).taskID, m_tasks.at(i).taskActive, m_tasks.at(i).allocateTime, m_tasks.at(i).timeTotal,
                             m_tasks.at(i).timeToday, m_tasks.at(i).timeTodayString, m_tasks.at(i).timeThisMonth, m_tasks.at(i).timeThisMonthString, m_tasks.at(i).timeThisYear, m_tasks.at(i).timeThisYearString,
                             m_tasks.at(i).timeDaily, m_tasks.at(i).timeDailyString, m_tasks.at(i).timeMonthly, m_tasks.at(i).timeMonthlyString, yearlyTime, yearlyTimeString, m_tasks.at(i).timelog });
        dataChanged(index(i, 0), index(i, 0), { HoursYearlyRole, MinutesYearlyRole, SecondsYearlyRole, elapsedSecYearlyRole, YearlyStringRole });

    }

    // Compute number of days worked per month:
    for (m=1; m<=12; m++) {
        for (n=1; n<=31; n++) {
            flag = 0;
            for (i=0; i<m_tasks.count(); i++) {
                for (it = m_tasks.at(i).timelog.begin(); it != m_tasks.at(i).timelog.end(); it++) {
                    if ((it.key().month() == m)&&(it.key().year() == year)&&(it.key().day() == n)) {
                        if ((it.value().elapsedSeconds > 0)&&(flag == 0)) {
                            m_daysWorkedYearly += 1;
                            flag = 1;
                        }
                    }
                }
            }
        }
    }

    // Format yearly total:
    TotalHours   =  m_totalSecondsYearly / 3600;
    TotalMinutes = (m_totalSecondsYearly-TotalHours*3600) / 60;
    TotalSeconds =  m_totalSecondsYearly-(TotalHours*3600 + TotalMinutes*60);
    m_totalTimeYearlyString = QString("%1:%2:%3").arg(TotalHours,2,10,QLatin1Char('0')).arg(TotalMinutes,2,10,QLatin1Char('0')).arg(TotalSeconds,2,10,QLatin1Char('0'));
    emit TimeChanged();

}


QString CTaskModel::updateMonthlyList(qint8 month, quint16 year)
{
// Update hours per month for the given month
// (month = [1 .. 12])
// Returns the month to be displayed as QString
    int i, n, ind, flag;
    quint32 elapsedSeconds;
    quint16 TotalHours, TotalMinutes, TotalSeconds;
    sTime   monthlyTime;
    QString monthlyTimeString;
    QString values[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    QMap<QDate, sTime>::const_iterator it;


    m_totalSecondsMonthly = 0;
    m_daysWorkedMonthly   = 0;

    // Iterate over all tasks:
    for (i=0; i<m_tasks.count(); i++) {
        // Reset monthly time to 0:
        elapsedSeconds = 0;

        // Iterate over all daily entries:
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            // If entry corresponds to queried month, add its time:
            if ((it.key().month() == month)&&(it.key().year() == year)) {
                elapsedSeconds += it.value().elapsedSeconds;
            }
            it++;
        }

        // Update timeMonthly in the Task:
        monthlyTime.Hours   =  elapsedSeconds / 3600;
        monthlyTime.Minutes = (elapsedSeconds-monthlyTime.Hours*3600) / 60;
        monthlyTime.Seconds =  elapsedSeconds-(monthlyTime.Hours*3600 + monthlyTime.Minutes*60);
        monthlyTime.elapsedSeconds = elapsedSeconds;
        monthlyTimeString = QString("%1:%2:%3").arg(monthlyTime.Hours,2,10,QLatin1Char('0')).arg(monthlyTime.Minutes,2,10,QLatin1Char('0')).arg(monthlyTime.Seconds,2,10,QLatin1Char('0'));

        // Sum up monthly times for monthly total:
        m_totalSecondsMonthly += monthlyTime.elapsedSeconds;

        m_tasks.replace(i, { m_tasks.at(i).title, m_tasks.at(i).description, m_tasks.at(i).taskID, m_tasks.at(i).taskActive, m_tasks.at(i).allocateTime, m_tasks.at(i).timeTotal,
                             m_tasks.at(i).timeToday, m_tasks.at(i).timeTodayString, m_tasks.at(i).timeThisMonth, m_tasks.at(i).timeThisMonthString, m_tasks.at(i).timeThisYear, m_tasks.at(i).timeThisYearString,
                             m_tasks.at(i).timeDaily, m_tasks.at(i).timeDailyString, monthlyTime, monthlyTimeString, m_tasks.at(i).timeYearly, m_tasks.at(i).timeYearlyString, m_tasks.at(i).timelog });
        dataChanged(index(i, 0), index(i, 0), { HoursMonthlyRole, MinutesMonthlyRole, SecondsMonthlyRole, elapsedSecMonthlyRole, MonthlyStringRole });

    }

    // Compute number of days worked per month:
    for (n=1; n<=31; n++) {
        flag = 0;
        for (i=0; i<m_tasks.count(); i++) {
            for (it = m_tasks.at(i).timelog.begin(); it != m_tasks.at(i).timelog.end(); it++) {
                if ((it.key().month() == month)&&(it.key().year() == year)&&(it.key().day() == n)) {
                    if ((it.value().elapsedSeconds > 0)&&(flag == 0)) {
                        m_daysWorkedMonthly += 1;
                        flag = 1;
                    }
                }
            }
        }
    }

    // Format monthly total:
    TotalHours   =  m_totalSecondsMonthly / 3600;
    TotalMinutes = (m_totalSecondsMonthly-TotalHours*3600) / 60;
    TotalSeconds =  m_totalSecondsMonthly-(TotalHours*3600 + TotalMinutes*60);
    m_totalTimeMonthlyString = QString("%1:%2:%3").arg(TotalHours,2,10,QLatin1Char('0')).arg(TotalMinutes,2,10,QLatin1Char('0')).arg(TotalSeconds,2,10,QLatin1Char('0'));
    emit TimeChanged();

    if (month >= 1) {
        if (month <= 12) { ind = month - 1;}
        else             { ind = 11; }
    }
    else { ind = 0;}

    return values[ind];

}


QString CTaskModel::updateDailyList(qint16 day)
{
// Update hours per day for the given day
// Returns the date to be displayed as QString
    int i, n;
    QDate date;
    quint32 elapsedSeconds;
    quint16 TotalHours, TotalMinutes, TotalSeconds;
    sTime   dailyTime;
    QString dailyTimeString;
    QMap<QDate, sTime>::const_iterator it;


    date = QDate::currentDate();
    date = date.addDays(day);

    //printf("updateDailyList: date = %s\n",date.toString("dd.MM.yyyy").toUtf8().data());

    m_totalSecondsDaily = 0;

    // Iterate over all tasks:
    for (i=0; i<m_tasks.count(); i++) {
        // Reset daily time to 0:
        elapsedSeconds = 0;

        // Iterate over all daily entries:
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            // If entry corresponds to queried day, add its time:
            if (it.key() == date) {
                elapsedSeconds += it.value().elapsedSeconds;
            }
            it++;
        }

        // Update timeDaily in the Task:
        dailyTime.Hours   =  elapsedSeconds / 3600;
        dailyTime.Minutes = (elapsedSeconds-dailyTime.Hours*3600) / 60;
        dailyTime.Seconds =  elapsedSeconds-(dailyTime.Hours*3600 + dailyTime.Minutes*60);
        dailyTime.elapsedSeconds = elapsedSeconds;
        dailyTimeString = QString("%1:%2:%3").arg(dailyTime.Hours,2,10,QLatin1Char('0')).arg(dailyTime.Minutes,2,10,QLatin1Char('0')).arg(dailyTime.Seconds,2,10,QLatin1Char('0'));

        // Sum up daily times for daily total:
        m_totalSecondsDaily += dailyTime.elapsedSeconds;

        m_tasks.replace(i, { m_tasks.at(i).title, m_tasks.at(i).description, m_tasks.at(i).taskID, m_tasks.at(i).taskActive, m_tasks.at(i).allocateTime, m_tasks.at(i).timeTotal,
                             m_tasks.at(i).timeToday, m_tasks.at(i).timeTodayString, m_tasks.at(i).timeThisMonth, m_tasks.at(i).timeThisMonthString, m_tasks.at(i).timeThisYear, m_tasks.at(i).timeThisYearString,
                             dailyTime, dailyTimeString, m_tasks.at(i).timeMonthly, m_tasks.at(i).timeMonthlyString, m_tasks.at(i).timeYearly, m_tasks.at(i).timeYearlyString, m_tasks.at(i).timelog });
        dataChanged(index(i, 0), index(i, 0), { HoursDailyRole, MinutesDailyRole, SecondsDailyRole, elapsedSecDailyRole, DailyStringRole });

    }

    // Format daily total:
    TotalHours   =  m_totalSecondsDaily / 3600;
    TotalMinutes = (m_totalSecondsDaily-TotalHours*3600) / 60;
    TotalSeconds =  m_totalSecondsDaily-(TotalHours*3600 + TotalMinutes*60);
    m_totalTimeDailyString = QString("%1:%2:%3").arg(TotalHours,2,10,QLatin1Char('0')).arg(TotalMinutes,2,10,QLatin1Char('0')).arg(TotalSeconds,2,10,QLatin1Char('0'));
    emit TimeChanged();

    return date.toString();

}


void CTaskModel::remove(int row)
{
//  Remove an entry from the list

    if (row < 0 || row >= m_tasks.count())
        return;

    // Stop timer if it was running:
    if (m_tasks.at(row).taskID == activeID) {
        stopTimer(row);
    }

    // Remove task:
    beginRemoveRows(QModelIndex(), row, row);
    m_tasks.removeAt(row);
    endRemoveRows();

    // Update total time over all tasks:
    updateTotals();

}


void CTaskModel::removeAll()
{
// Remove all tasks from the list
    int i;

    // Stop any running task:
    for (i=0; i<m_tasks.count(); i++) {
        if (m_tasks.at(i).taskActive==1) {
            stopTimer(i);
        }
    }

    beginResetModel();
    m_tasks.clear();
    endResetModel();

    // Reset filename to prevent overwriting the original file:
    activeFile.SaveFileName     = "";
    activeFile.SaveFileNameFull = "";
    m_SaveFileName              = activeFile.SaveFileName;
    m_SaveFileNameFull          = activeFile.SaveFileNameFull;
    activeFile.magic_no         = 0;
    // Reset password:
    activeFile.encrypted        = 0;
    activeFile.Password         = "";
    memset(activeFile.PasswordHashRead, 0, 32);
    memset(activeFile.salt, 0, 16);

    m_FileEncrypted    = activeFile.encrypted;

    emit settingChanged();

    updateTotals();
}



void CTaskModel::resetToday(int row)
{
//  Reset today's logged time for task
    sTime   totalTime;
    sTime   todayTime;
    QString todayTimeString;
    sTime   thisMonthTime;
    QString thisMonthTimeString;
    sTime   thisYearTime;
    QString thisYearTimeString;

    // Find out how many seconds we logged today:
    todayTime.elapsedSeconds = m_tasks.at(row).timeToday.elapsedSeconds;
    // Correct total time on that task by that value:
    totalTime.elapsedSeconds = m_tasks.at(row).timeTotal.elapsedSeconds;
    totalTime.elapsedSeconds -= todayTime.elapsedSeconds;
    totalTime.Hours    = totalTime.elapsedSeconds / 3600;
    totalTime.Minutes  = (totalTime.elapsedSeconds-totalTime.Hours*3600) / 60;
    totalTime.Seconds  = totalTime.elapsedSeconds-(totalTime.Hours*3600 + totalTime.Minutes*60);
    // Correct this month's time on that task by that value:
    thisMonthTime.elapsedSeconds = m_tasks.at(row).timeThisMonth.elapsedSeconds;
    thisMonthTime.elapsedSeconds -= todayTime.elapsedSeconds;
    thisMonthTime.Hours    = thisMonthTime.elapsedSeconds / 3600;
    thisMonthTime.Minutes  = (thisMonthTime.elapsedSeconds-thisMonthTime.Hours*3600) / 60;
    thisMonthTime.Seconds  = thisMonthTime.elapsedSeconds-(thisMonthTime.Hours*3600 + thisMonthTime.Minutes*60);
    thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));
    // Correct this year's time on that task by that value:
    thisYearTime.elapsedSeconds = m_tasks.at(row).timeThisYear.elapsedSeconds;
    thisYearTime.elapsedSeconds -= todayTime.elapsedSeconds;
    thisYearTime.Hours    = thisYearTime.elapsedSeconds / 3600;
    thisYearTime.Minutes  = (thisYearTime.elapsedSeconds-thisYearTime.Hours*3600) / 60;
    thisYearTime.Seconds  = thisYearTime.elapsedSeconds-(thisYearTime.Hours*3600 + thisYearTime.Minutes*60);
    thisYearTimeString = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));

    // Reset today's time:
    todayTime.elapsedSeconds = 0;
    todayTime.Hours    = 0;
    todayTime.Minutes  = 0;
    todayTime.Seconds  = 0;
    todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));

    updateEntry(row, totalTime, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, thisYearTime, thisYearTimeString);


    // Update total time over all tasks:
    updateTotals();

}


void CTaskModel::resetTotal(int row)
{
//  Reset total logged time for task
    sTime t = {0, 0, 0, 0};
    QString tString = "00:00:00";
    QMap<QDate, sTime> entry;


    // Start a fresh new timelog:
    entry.insert(today, t);

    m_tasks.replace(row, { m_tasks.at(row).title, m_tasks.at(row).description, m_tasks.at(row).taskID, m_tasks.at(row).taskActive, m_tasks.at(row).allocateTime, t,t,tString,t,tString,t,tString,t,tString,t,tString,t,tString, entry });
    dataChanged(index(row, 0), index(row, 0), { taskActiveRole, HoursTodayRole, MinutesTodayRole, SecondsTodayRole, elapsedSecTodayRole, TodayStringRole,
                                                HoursThisMonthRole, MinutesThisMonthRole, SecondsThisMonthRole, ThisMonthStringRole, HoursThisYearRole, MinutesThisYearRole, SecondsThisYearRole, ThisYearStringRole,
                                                HoursDailyRole, MinutesDailyRole, SecondsDailyRole, elapsedSecDailyRole, DailyStringRole,
                                                HoursMonthlyRole, MinutesMonthlyRole, SecondsMonthlyRole, elapsedSecMonthlyRole, MonthlyStringRole,
                                                HoursYearlyRole, MinutesYearlyRole, SecondsYearlyRole, elapsedSecYearlyRole, YearlyStringRole });

    // Update total time over all tasks:
    updateTotals();

}


void CTaskModel::resetAll()
{
// Reset all logged times for all tasks
    int i;

    for (i=0; i<m_tasks.count(); i++) {
        resetTotal(i);
    }

}


void CTaskModel::savePosition(int x, int y, int width, int height)
{
//  Memorize position and size of main window upon closing

    m_windowPosXSave   = x;
    m_windowPosYSave   = y;
    m_windowWidthSave  = width;
    m_windowHeightSave = height;
    //qInfo("Window position saved.");

}


void CTaskModel::updatePosition()
{
//  Update the main window position when a change of primary screen is detected
    //qInfo("updatePosition");

    // We only need to emit the signal so the QML updates the position to the last saved one
    // m_windowPosX and m_windowPosY have not changed since reading the ini file
    emit windowPosChanged();
}


QString CTaskModel::csvWriter()
{
//  Export time data to CSV format
    QFile csvfile;
    QString exportfilename;
    QMap<QDate, sTime>::const_iterator it;
    QDate earliestEntry, latestEntry, DayToWrite;
    int   i, n;
    float decimalHours;

    if (!activeFile.SaveFileNameFull.isEmpty()) {
        // Use as base name the active filename up to the last dot:
        exportfilename = activeFile.SaveFileNameFull.section(".",0,-2);
    }
    else {
        // If no filename has been specified yet, use generic name:
        exportfilename = "export";
    }
    // Add a date/timestamp to the filename:
    exportfilename.append("-");
    exportfilename.append(QDate::currentDate().toString("yyyy-MM-dd").toUtf8().data());
    exportfilename.append("-");
    exportfilename.append(QTime::currentTime().toString("HHmmss").toUtf8().data());
    exportfilename.append(".csv");
    csvfile.setFileName(exportfilename);

    qInfo("Writing CSV file...");
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Writing CSV file..." << endl;

    if (!csvfile.open(QIODevice::WriteOnly)) {
        qWarning("Could not open CSV file: %s",exportfilename.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Could not open CSV file: " << exportfilename.toUtf8().data() << endl;
        return "";
    }
    QTextStream out(&csvfile);
    out.setNumberFlags(QTextStream::ForcePoint);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(3);

    // Find earliest entry in any of the tasks:
    earliestEntry = today;
    for (i=0; i<m_tasks.count(); i++) {
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            // If entry is earlier than earliestEntry, update earliestEntry:
            if (it.key() < earliestEntry) {
                earliestEntry = it.key();
            }
            it++;
        }
    }
    // Find latest entry in any of the tasks:
    latestEntry.setDate(1970,1,1);
    for (i=0; i<m_tasks.count(); i++) {
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            // If entry is later than latestEntry, update latestEntry:
            if (it.key() > latestEntry) {
                latestEntry = it.key();
            }
            it++;
        }
    }

    //qInfo("Earliest Entry: %s",earliestEntry.toString("dd.MM.yyyy").toUtf8().data());
    //qInfo("Latest Entry  : %s",latestEntry.toString("dd.MM.yyyy").toUtf8().data());

    // Write header:
    out << "Date";
    for (i=0; i<m_tasks.count(); i++) {
        out << "," << m_tasks.at(i).title.toUtf8().data();
    }
    out << endl;

    // Write entries to CSV file:
    DayToWrite = earliestEntry;
    while (DayToWrite <= latestEntry) {
        out << DayToWrite.toString("dd.MM.yyyy").toUtf8().data();
        for (i=0; i<m_tasks.count(); i++) {
            it = m_tasks.at(i).timelog.begin();
            decimalHours = 0.0;
            for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
                // If DayToWrite is found in this task's timelog, update decimalHours:
                if (it.key() == DayToWrite) {
                    decimalHours = it.value().elapsedSeconds/3600.0;
                }
                it++;
            }
            out << "," << decimalHours;
        }
        out << endl;
        DayToWrite = DayToWrite.addDays(1);
    }

    csvfile.close();

    return exportfilename.section("/",-1);
}


void CTaskModel::openHelp()
{
// Opens the PDF containing the help

    QUrl baseUrl = QUrl("file:"+QDir::currentPath()+"/");
    QUrl url = QUrl("TimekeeperUserGuide.pdf");
    qInfo("baseUrl = %s",qPrintable(baseUrl.toString()));
    //qInfo("QCoreApplication::applicationDirPath() = %s",qPrintable(QCoreApplication::applicationDirPath()));

    QDesktopServices::openUrl(baseUrl.resolved(url));

}

void CTaskModel::restoreBackup()
{
//  Restore a previously save backup (currently not used - use "Load file" instead)
    int  i;

    // Stop any running task:
    for (i=0; i<m_tasks.count(); i++) {
        if (m_tasks.at(i).taskActive==1) {
            stopTimer(i);
        }
    }

    // Clear task list:
    removeAll();

    // Restore backup:
    if (!readfile("save.dat.bak")) {
        qWarning("Could not restore backup save file!");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Could not restore backup save file!" << endl;
    }
    emit windowPosChanged();
    emit settingChanged();

    // Update monthly / yearly / total times in main window:
    updateTotals();

}


void CTaskModel::checkEntries(int row)
{
// Debug: Print all timelog entries for task in "row" to console
    int n;
    QMap<QDate, sTime>::const_iterator it;
    QDate date;

    qInfo("Entries for task '%s' (Task ID %d):",m_tasks.at(row).title.toUtf8().data(),m_tasks.at(row).taskID);
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Entries for task " << m_tasks.at(row).title.toUtf8().data() << " (Task ID " << m_tasks.at(row).taskID << "):" << endl;

    // Iterate over all daily entries:
    it = m_tasks.at(row).timelog.begin();
    for (n = 0; n < m_tasks.at(row).timelog.size(); n++) {
        date = it.key();
        qInfo("Date: %s   Time logged: %02d:%02d:%02d",date.toString("dd.MM.yyyy").toUtf8().data(),it.value().Hours,it.value().Minutes,it.value().Seconds);
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Date: " << date.toString("dd.MM.yyyy").toUtf8().data() << " Time logged: " << it.value().Hours << ":" << it.value().Minutes << ":"<< it.value().Seconds << endl;
        it++;
    }

}


void CTaskModel::Update()
{
//  Update the logged time of an active entry
//  Driven by timer with a 1 sec. interval
    int     i, n;
    sTime   totalTime;
    sTime   todayTime;
    QString todayTimeString;
    sTime   thisMonthTime;
    QString thisMonthTimeString;
    sTime   thisYearTime;
    QString thisYearTimeString;
    QMap<QDate, sTime>::const_iterator it, it2;


    // Find the row to update (in case there was a sorting operation in the meantime):
    for (i=0; i<m_tasks.count(); i++) {
        if (m_tasks.at(i).taskID==activeID) {
            activeRow = i;
        }
    }


    // Check if it's a new year (implies that it's a new month and a new day):
    if (thisYear != (quint16) QDate::currentDate().year()) {
        thisYear  = (quint16) QDate::currentDate().year();
        thisMonth = (quint8)  QDate::currentDate().month();
        today     =           QDate::currentDate();
        todayTime.elapsedSeconds = 0;
        todayTime.Hours          = 0;
        todayTime.Minutes        = 0;
        todayTime.Seconds        = 0;
        todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
        thisMonthTime.elapsedSeconds = 0;
        thisMonthTime.Hours      = 0;
        thisMonthTime.Minutes    = 0;
        thisMonthTime.Seconds    = 0;
        thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));
        thisYearTime.elapsedSeconds = 0;
        thisYearTime.Hours       = 0;
        thisYearTime.Minutes     = 0;
        thisYearTime.Seconds     = 0;
        thisYearTimeString  = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));
        for (i=0; i<m_tasks.count(); i++) {
            // Set all non-active tasks:
            if (m_tasks.at(i).taskID != activeID) {

                // Iterate over all daily entries to check if there were already entries for the new month/year:
                for (it = m_tasks.at(i).timelog.begin(); it != m_tasks.at(i).timelog.end(); ++it) {
                    // If entry corresponds to new month, add its time:
                    if (it.key().month() == QDate::currentDate().month()) {
                        thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
                    }
                    // If entry corresponds to new year, add its time:
                    if (it.key().year() == QDate::currentDate().year()) {
                        thisYearTime.elapsedSeconds += it.value().elapsedSeconds;
                    }
                }
                thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));
                thisYearTimeString  = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));

                // Check if there is already an entry on this day:
                it2 = m_tasks.at(i).timelog.find(today);
                if (it2 != m_tasks.at(i).timelog.end()) {
                    // There is already an entry here - use its value
                    todayTime = it2.value();
                    todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                }  // Otherwise keep time at 0.

                updateEntry(i, m_tasks.at(i).timeTotal, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, thisYearTime, thisYearTimeString);
            }
        }
    }
    else {
        // Check if it's a new month (implies that it's a new day):
        if (thisMonth != (quint8) QDate::currentDate().month()) {
            thisMonth  = (quint8) QDate::currentDate().month();
            today      =          QDate::currentDate();
            todayTime.elapsedSeconds = 0;
            todayTime.Hours    = 0;
            todayTime.Minutes  = 0;
            todayTime.Seconds  = 0;
            todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
            thisMonthTime.elapsedSeconds = 0;
            thisMonthTime.Hours    = 0;
            thisMonthTime.Minutes  = 0;
            thisMonthTime.Seconds  = 0;
            thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));
            for (i=0; i<m_tasks.count(); i++) {
                // Set all non-active tasks:
                if (m_tasks.at(i).taskID != activeID) {

                    // Iterate over all daily entries to check if there were already entries for the new month:
                    for (it = m_tasks.at(i).timelog.begin(); it != m_tasks.at(i).timelog.end(); ++it) {
                        // If entry corresponds to new month, add its time:
                        if (it.key().month() == QDate::currentDate().month()) {
                            thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
                        }
                    }
                    thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));

                    // Check if there is already an entry on this day:
                    it2 = m_tasks.at(i).timelog.find(today);
                    if (it2 != m_tasks.at(i).timelog.end()) {
                        // There is already an entry here - use its value
                        todayTime = it2.value();
                        todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                    }  // Otherwise keep time at 0.

                    updateEntry(i, m_tasks.at(i).timeTotal, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, m_tasks.at(i).timeThisYear, m_tasks.at(i).timeThisYearString);
                }
            }
        }
        else {
            // Check if it's a new day:
            if (today != QDate::currentDate()) {
                today  = QDate::currentDate();
                todayTime.elapsedSeconds = 0;
                todayTime.Hours    = 0;
                todayTime.Minutes  = 0;
                todayTime.Seconds  = 0;
                todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                for (i=0; i<m_tasks.count(); i++) {
                    // Set all non-active tasks:
                    if (m_tasks.at(i).taskID != activeID) {

                        // Check if there is already an entry on this day:
                        it2 = m_tasks.at(i).timelog.find(today);
                        if (it2 != m_tasks.at(i).timelog.end()) {
                            // There is already an entry here - use its value
                            todayTime = it2.value();
                            todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                        }  // Otherwise keep time at 0.

                        updateEntry(i, m_tasks.at(i).timeTotal, todayTime, todayTimeString, m_tasks.at(i).timeThisMonth, m_tasks.at(i).timeThisMonthString, m_tasks.at(i).timeThisYear, m_tasks.at(i).timeThisYearString);
                    }
                }
            }
            else {
                todayTime.elapsedSeconds = m_tasks.at(activeRow).timeToday.elapsedSeconds;
            }
        }
    }

    // Update today's time for active task:
    it = m_tasks.at(activeRow).timelog.find(today);
    if (it != m_tasks.at(activeRow).timelog.end()) {
        // There is already an entry here - use its value
        todayTime = it.value();
    }
    todayTime.elapsedSeconds++;
    todayTime.Hours    =  todayTime.elapsedSeconds / 3600;
    todayTime.Minutes  = (todayTime.elapsedSeconds-todayTime.Hours*3600) / 60;
    todayTime.Seconds  =  todayTime.elapsedSeconds-(todayTime.Hours*3600 + todayTime.Minutes*60);
    todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));



    // Update this month's time for active task:
    thisMonthTime.elapsedSeconds = 0;
    // Iterate over all daily entries:
    it = m_tasks.at(activeRow).timelog.begin();
    for (n = 0; n < m_tasks.at(activeRow).timelog.size(); n++) {
        // If entry corresponds to queried month, add its time:
        if (it.key().month() == QDate::currentDate().month()) {
            thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
        }
        it++;
    }
    thisMonthTime.elapsedSeconds++;    // Account for the additional second that is not yet added to the task
    thisMonthTime.Hours    =  thisMonthTime.elapsedSeconds / 3600;
    thisMonthTime.Minutes  = (thisMonthTime.elapsedSeconds-thisMonthTime.Hours*3600) / 60;
    thisMonthTime.Seconds  =  thisMonthTime.elapsedSeconds-(thisMonthTime.Hours*3600 + thisMonthTime.Minutes*60);
    thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));


    // Update this year's time for active task:
    thisYearTime.elapsedSeconds = 0;
    // Iterate over all daily entries:
    it = m_tasks.at(activeRow).timelog.begin();
    for (n = 0; n < m_tasks.at(activeRow).timelog.size(); n++) {
        // If entry corresponds to queried year, add its time:
        if (it.key().year() == QDate::currentDate().year()) {
            thisYearTime.elapsedSeconds += it.value().elapsedSeconds;
        }
        it++;
    }
    thisYearTime.elapsedSeconds++;   // Account for the additional second that is not yet added to the task
    thisYearTime.Hours    =  thisYearTime.elapsedSeconds / 3600;
    thisYearTime.Minutes  = (thisYearTime.elapsedSeconds-thisYearTime.Hours*3600) / 60;
    thisYearTime.Seconds  =  thisYearTime.elapsedSeconds-(thisYearTime.Hours*3600 + thisYearTime.Minutes*60);
    thisYearTimeString = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));



    // Update total time for active task:
    totalTime.elapsedSeconds = 0;
    // Iterate over all daily entries:
    it = m_tasks.at(activeRow).timelog.begin();
    for (n = 0; n < m_tasks.at(activeRow).timelog.size(); n++) {
        totalTime.elapsedSeconds += it.value().elapsedSeconds;
        it++;
    }
    totalTime.elapsedSeconds++;    // Account for the additional second that is not yet added to the task
    totalTime.Hours    =  totalTime.elapsedSeconds / 3600;
    totalTime.Minutes  = (totalTime.elapsedSeconds-totalTime.Hours*3600) / 60;
    totalTime.Seconds  =  totalTime.elapsedSeconds-(totalTime.Hours*3600 + totalTime.Minutes*60);


    // Update the active entry:
    updateEntry(activeRow, totalTime, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, thisYearTime, thisYearTimeString);
    // updateEntry2 does not not consider daily / monthly / yearly wrap-over yet

    // Update daily / monthly / yearly total times:
    updateTotals();

}


void CTaskModel::UpdateAll()
{
//  Update the logged time of all entries without increasing the time
// (Call e.g. after loading a new file to properly initialize all displays)
    int     t, n;
    sTime   totalTime;
    sTime   todayTime;
    QString todayTimeString;
    sTime   thisMonthTime;
    QString thisMonthTimeString;
    sTime   thisYearTime;
    QString thisYearTimeString;
    bool    newYear, newMonth, newDay;
    QMap<QDate, sTime>::const_iterator it, it2;

    // Set flags indicating if it's a new year, month or day:
    // (If we do this in the task loop, it will only work for the first task)
    newYear  = false;
    newMonth = false;
    newDay   = false;

    // Check if it's a new year (implies that it's a new month and a new day):
    if (thisYear != (quint16) QDate::currentDate().year()) {
        thisYear  = (quint16) QDate::currentDate().year();
        thisMonth = (quint8)  QDate::currentDate().month();
        today     =           QDate::currentDate();
        newYear   = true;
        newMonth  = true;
        newDay    = true;
    }
    else {
        // Check if it's a new month (implies that it's a new day):
        if (thisMonth != (quint8) QDate::currentDate().month()) {
            thisMonth  = (quint8) QDate::currentDate().month();
            today      =          QDate::currentDate();
            newMonth   = true;
            newDay     = true;
        }
        else {
            // Check if it's a new day:
            if (today != QDate::currentDate()) {
                today  = QDate::currentDate();
                newDay = true;
            }
        }
    }

    // Iterate over all tasks
    for (t=0; t<m_tasks.count(); t++) {

        // Check if it's a new year (implies that it's a new month and a new day):
        if (newYear) {
            todayTime.elapsedSeconds = 0;
            todayTime.Hours          = 0;
            todayTime.Minutes        = 0;
            todayTime.Seconds        = 0;
            todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
            thisMonthTime.elapsedSeconds = 0;
            thisMonthTime.Hours      = 0;
            thisMonthTime.Minutes    = 0;
            thisMonthTime.Seconds    = 0;
            thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));
            thisYearTime.elapsedSeconds = 0;
            thisYearTime.Hours       = 0;
            thisYearTime.Minutes     = 0;
            thisYearTime.Seconds     = 0;
            thisYearTimeString = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));

            // Iterate over all daily entries:
            for (it = m_tasks.at(t).timelog.begin(); it != m_tasks.at(t).timelog.end(); ++it) {
                // If entry corresponds to new month, add its time:
                if (it.key().month() == QDate::currentDate().month()) {
                    thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
                }
                // If entry corresponds to new year, add its time:
                if (it.key().year() == QDate::currentDate().year()) {
                    thisYearTime.elapsedSeconds += it.value().elapsedSeconds;
                }
            }
            thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));
            thisYearTimeString  = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));

            // Check if there is already an entry on this day:
            it2 = m_tasks.at(t).timelog.find(today);
            if (it2 != m_tasks.at(t).timelog.end()) {
                // There is already an entry here - use its value
                todayTime = it2.value();
                todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
            }  // Otherwise keep time at 0.

            updateEntry(t, m_tasks.at(t).timeTotal, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, thisYearTime, thisYearTimeString);
        }
        else {
            // Check if it's a new month (implies that it's a new day):
            if (newMonth) {
                todayTime.elapsedSeconds = 0;
                todayTime.Hours          = 0;
                todayTime.Minutes        = 0;
                todayTime.Seconds        = 0;
                todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                thisMonthTime.elapsedSeconds = 0;
                thisMonthTime.Hours      = 0;
                thisMonthTime.Minutes    = 0;
                thisMonthTime.Seconds    = 0;
                thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));

                // Iterate over all daily entries:
                for (it = m_tasks.at(t).timelog.begin(); it != m_tasks.at(t).timelog.end(); ++it) {
                    // If entry corresponds to new month, add its time:
                    if (it.key().month() == QDate::currentDate().month()) {
                        thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
                    }
                }
                thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));

                // Check if there is already an entry on this day:
                it2 = m_tasks.at(t).timelog.find(today);
                if (it2 != m_tasks.at(t).timelog.end()) {
                    // There is already an entry here - use its value
                    todayTime = it2.value();
                    todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                }  // Otherwise keep time at 0.

                updateEntry(t, m_tasks.at(t).timeTotal, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, m_tasks.at(t).timeThisYear, m_tasks.at(t).timeThisYearString);
            }
            else {
                // Check if it's a new day:
                if (newDay) {
                    todayTime.elapsedSeconds = 0;
                    todayTime.Hours          = 0;
                    todayTime.Minutes        = 0;
                    todayTime.Seconds        = 0;
                    todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                    // Check if there is already an entry on this day:
                    it2 = m_tasks.at(t).timelog.find(today);
                    if (it2 != m_tasks.at(t).timelog.end()) {
                        // There is already an entry here - use its value
                        todayTime = it2.value();
                        todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));
                    }  // Otherwise keep time at 0.

                    updateEntry(t, m_tasks.at(t).timeTotal, todayTime, todayTimeString, m_tasks.at(t).timeThisMonth, m_tasks.at(t).timeThisMonthString, m_tasks.at(t).timeThisYear, m_tasks.at(t).timeThisYearString);
                }
                else {
                    todayTime.elapsedSeconds = m_tasks.at(t).timeToday.elapsedSeconds;
                }
            }
        }

        // Update today's time for this task:
        todayTime.Hours    =  todayTime.elapsedSeconds / 3600;
        todayTime.Minutes  = (todayTime.elapsedSeconds-todayTime.Hours*3600) / 60;
        todayTime.Seconds  =  todayTime.elapsedSeconds-(todayTime.Hours*3600 + todayTime.Minutes*60);
        todayTimeString = QString("%1:%2:%3").arg(todayTime.Hours,2,10,QLatin1Char('0')).arg(todayTime.Minutes,2,10,QLatin1Char('0')).arg(todayTime.Seconds,2,10,QLatin1Char('0'));


        // Update this month's time for this task:
        thisMonthTime.elapsedSeconds = 0;
        // Iterate over all daily entries:
        it = m_tasks.at(t).timelog.begin();
        for (n = 0; n < m_tasks.at(t).timelog.size(); n++) {
            // If entry corresponds to queried month, add its time:
            if (it.key().month() == QDate::currentDate().month()) {
                thisMonthTime.elapsedSeconds += it.value().elapsedSeconds;
            }
            it++;
        }
        thisMonthTime.Hours    =  thisMonthTime.elapsedSeconds / 3600;
        thisMonthTime.Minutes  = (thisMonthTime.elapsedSeconds-thisMonthTime.Hours*3600) / 60;
        thisMonthTime.Seconds  =  thisMonthTime.elapsedSeconds-(thisMonthTime.Hours*3600 + thisMonthTime.Minutes*60);
        thisMonthTimeString = QString("%1:%2:%3").arg(thisMonthTime.Hours,2,10,QLatin1Char('0')).arg(thisMonthTime.Minutes,2,10,QLatin1Char('0')).arg(thisMonthTime.Seconds,2,10,QLatin1Char('0'));


        // Update this year's time for this task:
        thisYearTime.elapsedSeconds = 0;
        // Iterate over all daily entries:
        it = m_tasks.at(t).timelog.begin();
        for (n = 0; n < m_tasks.at(t).timelog.size(); n++) {
            // If entry corresponds to queried year, add its time:
            if (it.key().year() == QDate::currentDate().year()) {
                thisYearTime.elapsedSeconds += it.value().elapsedSeconds;
            }
            it++;
        }
        thisYearTime.Hours    =  thisYearTime.elapsedSeconds / 3600;
        thisYearTime.Minutes  = (thisYearTime.elapsedSeconds-thisYearTime.Hours*3600) / 60;
        thisYearTime.Seconds  =  thisYearTime.elapsedSeconds-(thisYearTime.Hours*3600 + thisYearTime.Minutes*60);
        thisYearTimeString = QString("%1:%2:%3").arg(thisYearTime.Hours,2,10,QLatin1Char('0')).arg(thisYearTime.Minutes,2,10,QLatin1Char('0')).arg(thisYearTime.Seconds,2,10,QLatin1Char('0'));



        // Update total time for this task:
        totalTime.elapsedSeconds = 0;
        // Iterate over all daily entries:
        it = m_tasks.at(t).timelog.begin();
        for (n = 0; n < m_tasks.at(t).timelog.size(); n++) {
            totalTime.elapsedSeconds += it.value().elapsedSeconds;
            it++;
        }
        totalTime.Hours    =  totalTime.elapsedSeconds / 3600;
        totalTime.Minutes  = (totalTime.elapsedSeconds-totalTime.Hours*3600) / 60;
        totalTime.Seconds  =  totalTime.elapsedSeconds-(totalTime.Hours*3600 + totalTime.Minutes*60);


        // Update the active entry:
        updateEntry(t, totalTime, todayTime, todayTimeString, thisMonthTime, thisMonthTimeString, thisYearTime, thisYearTimeString);

    }


    // Update daily / monthly / yearly total times:
    updateTotals();

}


bool CTaskModel::readIniFile()
{
// Read data from ini file
    QFile inifile("Timekeeper.ini");
    QString line;
    QString param;


    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Reading ini file: Timekeeper.ini" << endl;

    if (!inifile.open(QIODevice::ReadOnly)) {
        qWarning("Could not read ini file - Timekeeper.ini!");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "Could not read ini file: Timekeeper.ini!" << endl;
        return false;
    }
    QTextStream in(&inifile);

    while (!in.atEnd()) {
        line = in.readLine();
        param = line.section("=",0,0);
        if      (param=="WindowPosX")   m_windowPosX   = line.section("=",1,1).toInt();
        else if (param=="WindowPosY")   m_windowPosY   = line.section("=",1,1).toInt();
        else if (param=="WindowWidth")  m_windowWidth  = line.section("=",1,1).toInt();
        else if (param=="WindowHeight") m_windowHeight = line.section("=",1,1).toInt();
        else if (param=="LastSelectedDate")     m_settingLastSelectedDate     = line.section("=",1,1).toInt();
        else if (param=="WeightedReallocation") m_settingWeightedReallocation = line.section("=",1,1).toInt();
        else if (param=="Autosave")             m_settingAutosave             = line.section("=",1,1).toInt();
        else if (param=="MinimizeToTray")       m_settingMinimizeToTray       = line.section("=",1,1).toInt();
        else if (param=="StartingView")         m_settingStartingView         = line.section("=",1,1).toInt();
        else if (param=="ColorScheme")          m_settingColorScheme          = line.section("=",1,1).toInt();
        else if (param=="LastSaveFile")         newFile.SaveFileNameFull      = line.section("=",1,1);
    }
    inifile.close();

    // Make sure the window coordinates fit on the current screen:
    if (m_windowPosX > ScreenSize.width()-m_windowWidth) {
        m_windowPosX = ScreenSize.width()-m_windowWidth;
    }
    if (m_windowPosY > ScreenSize.height()-m_windowHeight-50) {
        m_windowPosY = ScreenSize.height()-m_windowHeight-50;
    }

    // Make sure checkbox settings are in the right format for QML:
    if (m_settingLastSelectedDate >= 1)     m_settingLastSelectedDate = 2;      // 0: Unchecked, 1: Partially Checked, 2: Checked
    if (m_settingWeightedReallocation >= 1) m_settingWeightedReallocation = 2;  // 0: Unchecked, 1: Partially Checked, 2: Checked
    if (m_settingAutosave >= 1)             m_settingAutosave = 2;              // 0: Unchecked, 1: Partially Checked, 2: Checked
    if (m_settingMinimizeToTray >= 1)       m_settingMinimizeToTray = 2;        // 0: Unchecked, 1: Partially Checked, 2: Checked

    newFile.SaveFileName     = newFile.SaveFileNameFull.section("/",-1);

    if (!newFile.SaveFileNameFull.isEmpty()) {
        m_defaultFile   = 1;
        checkFileType(newFile.SaveFileNameFull);
    }


//    qInfo("m_windowPosX = %d",m_windowPosX);
//    qInfo("m_windowPosY = %d",m_windowPosY);
//    qInfo("m_windowWidth = %d",m_windowWidth);
//    qInfo("m_windowHeight = %d",m_windowHeight);
//    qInfo("m_settingLastSelectedDate = %d",m_settingLastSelectedDate);
//    qInfo("m_settingWeightedReallocation = %d",m_settingWeightedReallocation);
//    qInfo("m_settingAutosave = %d",m_settingAutosave);
//    qInfo("m_settingMinimizeToTray = %d",m_settingMinimizeToTray);
//    qInfo("m_settingStartingView = %d",m_settingStartingView);
//    qInfo("m_settingColorScheme  = %d",m_settingColorScheme);
//    qInfo("SaveFileName = %s   SaveFileNameFull = %s",newFile.SaveFileName.toUtf8().data(),newFile.SaveFileNameFull.toUtf8().data());
//    qInfo("m_defaultFile = %d",m_defaultFile);
//    qInfo("read_encrypted = %d",read_encrypted);

    emit windowPosChanged();
    emit settingChanged();

    return true;
}


bool CTaskModel::readfile(QString filename)
{
//  Read data from unencrypted saved file into m_tasks list
//  ToDo: Sanity check of file
    int i, n, row, n_days, flag;
    quint8  dummy;
    quint32 dummy32;
    Task t;
    qint64 day, daySaved;
    QDate Date;
    sTime Time;
    QMap<QDate, sTime> log;
    QFile readfile;

    qInfo("Reading unencrypted save file: %s",filename.toUtf8().data());
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfile(): Reading unencrypted save file: " << filename.toUtf8().data() << endl;

    // Clear the list first:
    removeAll();

    readfile.setFileName(filename);

    if (!readfile.open(QIODevice::ReadOnly)) {
        qWarning("Could not read save file - %s!",filename.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfile(): Could not read save file: " << filename.toUtf8().data() << endl;
        return false;
    };
    QDataStream in(&readfile);
    in.setVersion(QDataStream::Qt_5_0);

    // Magic number
    in >> magic_no;       //qInfo("Magic no. = %X",magic_no);
    if (magic_no != 0x051076A0) {
        qWarning("Bad save file format - %s!",filename.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfile(): Bad save file format: " << filename.toUtf8().data() << endl;
        return false;
    }
    // Version number
    in >> version_no;       //qInfo("Version no. = %d",version_no);
    if (version_no != 100) {
        qWarning("Bad save file version - %s!",filename.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfile(): Bad save file version: " << filename.toUtf8().data() << endl;
        return false;
    }

    // 16 Byte Salt & 32 Byte hashed encryption key (dummy entries)
    for (i=0; i<16; i++) {
        in >> dummy;
    }
    for (i=0; i<32; i++) {
        in >> dummy;
    }

    while(!in.atEnd()) {
        in >> row;                  //qInfo("row: %d",row);
        in >> t.title;              //qInfo("title: %s",t.title.toUtf8().data());
        in >> t.description;        //qInfo("description: %s",t.description.toUtf8().data());
        in >> t.taskID;             //qInfo("taskID: %u",t.taskID);
              t.taskActive   = 0;
              t.allocateTime = 0;
        // timeTotal
        in >> t.timeTotal.Hours;
        in >> t.timeTotal.Minutes;
        in >> t.timeTotal.Seconds;
        in >> t.timeTotal.elapsedSeconds;
        // timeToday
        in >> daySaved;
        in >> t.timeToday.Hours;
        in >> t.timeToday.Minutes;
        in >> t.timeToday.Seconds;
        in >> t.timeToday.elapsedSeconds;
        if (daySaved != today.toJulianDay()) {
            // If timeToday was saved on another day, reset it:
            t.timeToday.Hours   = 0;
            t.timeToday.Minutes = 0;
            t.timeToday.Seconds = 0;
            t.timeToday.elapsedSeconds = 0;
        }
        t.timeTodayString = QString("%1:%2:%3").arg(t.timeToday.Hours,2,10,QLatin1Char('0')).arg(t.timeToday.Minutes,2,10,QLatin1Char('0')).arg(t.timeToday.Seconds,2,10,QLatin1Char('0'));  //qInfo("timeTodayString: %s\n",t.timeTodayString.toUtf8().data());
        // timeThisMonth
        in >> thisMonthSaved;
        in >> t.timeThisMonth.Hours;
        in >> t.timeThisMonth.Minutes;
        in >> t.timeThisMonth.Seconds;
        in >> t.timeThisMonth.elapsedSeconds;
        if (thisMonthSaved != thisMonth) {
            // If timeThisMonth was saved in another month, reset it:
            t.timeThisMonth.Hours   = 0;
            t.timeThisMonth.Minutes = 0;
            t.timeThisMonth.Seconds = 0;
            t.timeThisMonth.elapsedSeconds = 0;
        }
        t.timeThisMonthString = QString("%1:%2:%3").arg(t.timeThisMonth.Hours,2,10,QLatin1Char('0')).arg(t.timeThisMonth.Minutes,2,10,QLatin1Char('0')).arg(t.timeThisMonth.Seconds,2,10,QLatin1Char('0'));  //qInfo("timeThisMonth: %s\n",t.timeThisMonthString.toUtf8().data());
        // timeThisYear
        in >> thisYearSaved;          //qInfo("thisYearSaved: %d",thisYearSaved);
        in >> t.timeThisYear.Hours;
        in >> t.timeThisYear.Minutes;
        in >> t.timeThisYear.Seconds;
        in >> t.timeThisYear.elapsedSeconds;
        if (thisYearSaved != thisYear) {
            // If timeThisYear was saved in another year, reset it:
            t.timeThisYear.Hours   = 0;
            t.timeThisYear.Minutes = 0;
            t.timeThisYear.Seconds = 0;
            t.timeThisYear.elapsedSeconds = 0;
        }
        t.timeThisYearString = QString("%1:%2:%3").arg(t.timeThisYear.Hours,2,10,QLatin1Char('0')).arg(t.timeThisYear.Minutes,2,10,QLatin1Char('0')).arg(t.timeThisYear.Seconds,2,10,QLatin1Char('0'));  //qInfo("timeThisMonth: %s\n",t.timeThisYearString.toUtf8().data());
        // timeDaily
        in >> t.timeDaily.Hours;
        in >> t.timeDaily.Minutes;
        in >> t.timeDaily.Seconds;
        in >> t.timeDaily.elapsedSeconds;
        // timeMonthly
        in >> t.timeMonthly.Hours;
        in >> t.timeMonthly.Minutes;
        in >> t.timeMonthly.Seconds;
        in >> t.timeMonthly.elapsedSeconds;
        // timeYearly
        in >> t.timeYearly.Hours;
        in >> t.timeYearly.Minutes;
        in >> t.timeYearly.Seconds;
        in >> t.timeYearly.elapsedSeconds;
        // timelog
        flag = 0;
        in >> n_days;              //printf("n_days: %d\n",n_days);
        for (n=0; n<n_days; n++) {
            in >> day;   // Julian day
            Date = QDate::fromJulianDay(day);        //printf("Date: %s\n",Date.toString().toUtf8().data());
            in >> Time.Hours;
            in >> Time.Minutes;
            in >> Time.Seconds;
            in >> Time.elapsedSeconds;               //printf("elapsedSeconds: %d\n",Time.elapsedSeconds);
            log.insert(Date, Time);
            if (Date == today) flag = 1;   // Flag if we already have an entry for today, otherwise we need to create it
        }
        if (flag == 0) {
            // Insert an empty entry for today if it does not exist yet
            Time.Hours          = 0;
            Time.Minutes        = 0;
            Time.Seconds        = 0;
            Time.elapsedSeconds = 0;
            log.insert(today, Time);
        }
        t.timelog = log;
        beginInsertRows(QModelIndex(), m_tasks.count(), m_tasks.count());
        m_tasks.append(t);
        endInsertRows();
        in >> dummy32;
        log.clear();
    }

    readfile.close();

    return true;
}



bool CTaskModel::readfileEncrypted(QString filename)
{
//  Read data from encrypted saved file into m_tasks list
//  ToDo: Sanity check of file
    int n, row, n_days, flag;
    int size_cipher, num_blocks, idx, ret;
    char title[32];
    char description[128];
    Task t;
    qint64 day, daySaved;
    QDate Date;
    sTime Time;
    QMap<QDate, sTime> log;
    QFile readfile;
    uint8_t  *plaintext;
    uint8_t  *ciphertext;
    char     *buffer;

    qInfo("Reading encrypted save file: %s",filename.toUtf8().data());
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfileEncrypted(): Reading encrypted save file: " << filename.toUtf8().data() << endl;

    // Clear the list first:
    removeAll();

    readfile.setFileName(filename);

    if (!readfile.open(QIODevice::ReadOnly)) {
        qWarning("Could not read save file - %s!",filename.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfileEncrypted(): Could not read save file: " << filename.toUtf8().data() << endl;
        return false;
    };
    QDataStream in(&readfile);
    in.setVersion(QDataStream::Qt_5_0);


    readfile.skip(54);   // Skip magic no., version no., salt & hash (read in load_data() already)

    // Read encrypted data into buffer
    size_cipher = readfile.size() - 6 - 48;  // File size minus magic no., version no., salt & hash
    num_blocks  = size_cipher/16;            // writefileEncrypted makes sure to always write multiples of 16 Bytes
    //qInfo("size_cipher = %d  num_blocks = %d",size_cipher,num_blocks);

    buffer     = new char[size_cipher];
    ciphertext = new uint8_t[size_cipher];
    plaintext =  new uint8_t[size_cipher];
    memset(plaintext, 0, size_cipher);

    // Read encrypted data from file
    ret = in.readRawData(&buffer[0], size_cipher);
    if (ret == -1) {
        qWarning("Error reading data from %s!",filename.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "readfileEncrypted(): Error reading data from " << filename.toUtf8().data() << endl;
    }
    //qInfo("ret = %d",ret);

    // Decrypt
    memcpy(&ciphertext[0],&buffer[0],size_cipher);
    cbc.decrypt(&plaintext[0], &ciphertext[0], size_cipher);

    // Write decrypted data into variables
    idx = 16;   // First block is unusable

    // while-condition considers possible padding to 16-byte blocks at the end
    while(idx < size_cipher-16) {
        memcpy(&row, &plaintext[idx],4);  idx += 4;                    //qInfo("row: %d",row);
        memcpy(&title[0], &plaintext[idx],32);  idx += 32;
        t.title = title;                                               //qInfo("title: %s",t.title.toUtf8().data());
        memcpy(&description[0], &plaintext[idx],128);  idx += 128;
        t.description = description;                                   //qInfo("description: %s",t.description.toUtf8().data());

        memcpy(&(t.taskID), &plaintext[idx],4);  idx += 4;             //qInfo("taskID: %d",t.taskID);
        t.taskActive   = 0;
        t.allocateTime = 0;
        // timeTotal
        memcpy(&(t.timeTotal.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeTotal.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeTotal.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeTotal.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        // timeToday
        memcpy(&daySaved, &plaintext[idx],8);  idx += 8;
        memcpy(&(t.timeToday.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeToday.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeToday.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeToday.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        if (daySaved != today.toJulianDay()) {
            // If timeToday was saved on another day, reset it:
            t.timeToday.Hours   = 0;
            t.timeToday.Minutes = 0;
            t.timeToday.Seconds = 0;
            t.timeToday.elapsedSeconds = 0;
        }
        t.timeTodayString = QString("%1:%2:%3").arg(t.timeToday.Hours,2,10,QLatin1Char('0')).arg(t.timeToday.Minutes,2,10,QLatin1Char('0')).arg(t.timeToday.Seconds,2,10,QLatin1Char('0'));  //qInfo("timeTodayString: %s\n",t.timeTodayString.toUtf8().data());
        // timeThisMonth
        memcpy(&thisMonthSaved, &plaintext[idx],1);  idx += 1;
        memcpy(&(t.timeThisMonth.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisMonth.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisMonth.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisMonth.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        if (thisMonthSaved != thisMonth) {
            // If timeThisMonth was saved in another month, reset it:
            t.timeThisMonth.Hours   = 0;
            t.timeThisMonth.Minutes = 0;
            t.timeThisMonth.Seconds = 0;
            t.timeThisMonth.elapsedSeconds = 0;
        }
        t.timeThisMonthString = QString("%1:%2:%3").arg(t.timeThisMonth.Hours,2,10,QLatin1Char('0')).arg(t.timeThisMonth.Minutes,2,10,QLatin1Char('0')).arg(t.timeThisMonth.Seconds,2,10,QLatin1Char('0'));  //qInfo("timeThisMonth: %s\n",t.timeThisMonthString.toUtf8().data());
        // timeThisYear
        memcpy(&thisYearSaved, &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisYear.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisYear.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisYear.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeThisYear.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        if (thisYearSaved != thisYear) {
            // If timeThisYear was saved in another year, reset it:
            t.timeThisYear.Hours   = 0;
            t.timeThisYear.Minutes = 0;
            t.timeThisYear.Seconds = 0;
            t.timeThisYear.elapsedSeconds = 0;
        }
        t.timeThisYearString = QString("%1:%2:%3").arg(t.timeThisYear.Hours,2,10,QLatin1Char('0')).arg(t.timeThisYear.Minutes,2,10,QLatin1Char('0')).arg(t.timeThisYear.Seconds,2,10,QLatin1Char('0'));  //qInfo("timeThisYear: %s\n",t.timeThisYearString.toUtf8().data());
        // timeDaily
        memcpy(&(t.timeDaily.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeDaily.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeDaily.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeDaily.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        // timeMonthly
        memcpy(&(t.timeMonthly.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeMonthly.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeMonthly.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeMonthly.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        // timeYearly
        memcpy(&(t.timeYearly.Hours), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeYearly.Minutes), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeYearly.Seconds), &plaintext[idx],2);  idx += 2;
        memcpy(&(t.timeYearly.elapsedSeconds), &plaintext[idx],4);  idx += 4;
        // timelog
        flag = 0;
        memcpy(&n_days, &plaintext[idx],4);  idx += 4;              //printf("n_days: %d\n",n_days);
        for (n=0; n<n_days; n++) {
            memcpy(&day, &plaintext[idx],8);  idx += 8;   // Julian day
            Date = QDate::fromJulianDay(day);        //printf("Date: %s\n",Date.toString().toUtf8().data());
            memcpy(&(Time.Hours),   &plaintext[idx],2);  idx += 2;
            memcpy(&(Time.Minutes), &plaintext[idx],2);  idx += 2;
            memcpy(&(Time.Seconds), &plaintext[idx],2);  idx += 2;
            memcpy(&(Time.elapsedSeconds), &plaintext[idx],4);  idx += 4;               //printf("elapsedSeconds: %d\n",Time.elapsedSeconds);
            log.insert(Date, Time);
            if (Date == today) flag = 1;   // Flag if we already have an entry for today, otherwise we need to create it
        }
        if (flag == 0) {
            // Insert an empty entry for today if it does not exist yet
            Time.Hours          = 0;
            Time.Minutes        = 0;
            Time.Seconds        = 0;
            Time.elapsedSeconds = 0;
            log.insert(today, Time);
        }
        t.timelog = log;
        beginInsertRows(QModelIndex(), m_tasks.count(), m_tasks.count());
        m_tasks.append(t);
        endInsertRows();
        idx += 4;       //Last 4 Bytes unused (formerly nextID)
        log.clear();

        //qInfo("idx = %d",idx);
    }


    readfile.close();
    if (buffer != NULL)     delete[] buffer;
    if (ciphertext != NULL) delete[] ciphertext;
    if (plaintext != NULL)  delete[] plaintext;

    return true;
}


int CTaskModel::writefile()
{
//  Write all necessary data to file in plaintext format
    int i, n;
    QFile savefiledat;
    QMap<QDate, sTime>::const_iterator it;

    qInfo("writefile(): Writing unencrypted save file: %s",activeFile.SaveFileNameFull.toUtf8().data());
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "writefile(): Writing unencrypted save file: " << activeFile.SaveFileNameFull.toUtf8().data() << endl;

    savefiledat.setFileName(activeFile.SaveFileNameFull);


    if (!savefiledat.open(QIODevice::WriteOnly)) {
        qWarning("writefile(): Could not open save file: %s",activeFile.SaveFileNameFull.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "writefile(): Could not open save file: " << activeFile.SaveFileNameFull.toUtf8().data() << endl;
        return 1;
    }
    QDataStream out(&savefiledat);
    out.setVersion(QDataStream::Qt_5_0);


    // Magic number to verify file format
    out << (quint32)0x051076A0;
    // Version number
    out << (quint16) 100;

    // 16 Byte Salt & 32 Byte hashed encryption key (not used)
    for (i=0; i<16; i++) {
        out << (quint8) i;
    }
    for (i=0; i<32; i++) {
        out << (quint8) i;
    }

    for(i=0; i<m_tasks.count(); i++) {
        out << (qint32) i;
        out << (QString) m_tasks.at(i).title;
        out << (QString) m_tasks.at(i).description;
        out << (quint32) m_tasks.at(i).taskID;
        // taskActive is not saved
        // allocateTime is not saved
        // timeTotal
        out << (quint16) m_tasks.at(i).timeTotal.Hours;
        out << (quint16) m_tasks.at(i).timeTotal.Minutes;
        out << (quint16) m_tasks.at(i).timeTotal.Seconds;
        out << (quint32) m_tasks.at(i).timeTotal.elapsedSeconds;
        // timeToday
        out << (qint64)  today.toJulianDay();
        out << (quint16) m_tasks.at(i).timeToday.Hours;
        out << (quint16) m_tasks.at(i).timeToday.Minutes;
        out << (quint16) m_tasks.at(i).timeToday.Seconds;
        out << (quint32) m_tasks.at(i).timeToday.elapsedSeconds;
        // timeThisMonth
        out << (quint8)  thisMonth;
        out << (quint16) m_tasks.at(i).timeThisMonth.Hours;
        out << (quint16) m_tasks.at(i).timeThisMonth.Minutes;
        out << (quint16) m_tasks.at(i).timeThisMonth.Seconds;
        out << (quint32) m_tasks.at(i).timeThisMonth.elapsedSeconds;
        // timeThisYear
        out << (quint16) thisYear;
        out << (quint16) m_tasks.at(i).timeThisYear.Hours;
        out << (quint16) m_tasks.at(i).timeThisYear.Minutes;
        out << (quint16) m_tasks.at(i).timeThisYear.Seconds;
        out << (quint32) m_tasks.at(i).timeThisYear.elapsedSeconds;
        // timeDaily
        out << (quint16) m_tasks.at(i).timeDaily.Hours;
        out << (quint16) m_tasks.at(i).timeDaily.Minutes;
        out << (quint16) m_tasks.at(i).timeDaily.Seconds;
        out << (quint32) m_tasks.at(i).timeDaily.elapsedSeconds;
        // timeMonthly
        out << (quint16) m_tasks.at(i).timeMonthly.Hours;
        out << (quint16) m_tasks.at(i).timeMonthly.Minutes;
        out << (quint16) m_tasks.at(i).timeMonthly.Seconds;
        out << (quint32) m_tasks.at(i).timeMonthly.elapsedSeconds;
        // timeYearly
        out << (quint16) m_tasks.at(i).timeYearly.Hours;
        out << (quint16) m_tasks.at(i).timeYearly.Minutes;
        out << (quint16) m_tasks.at(i).timeYearly.Seconds;
        out << (quint32) m_tasks.at(i).timeYearly.elapsedSeconds;
       // timelog
        out << (qint32) m_tasks.at(i).timelog.size();         //printf("Writing timelog.size(): %d\n",m_tasks.at(i).timelog.size());
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            out << (qint64)  it.key().toJulianDay();
            out << (quint16) it.value().Hours;
            out << (quint16) it.value().Minutes;
            out << (quint16) it.value().Seconds;
            out << (quint32) it.value().elapsedSeconds;
            it++;
        }
        //
        out << (quint32) 0;             //Unused (formerly nextID)
    }


    savefiledat.close();

    return 0;
}


int CTaskModel::writefileEncrypted()
{
//  Write all necessary data to an encrypted file
//
//  The magic no. (file format) and version no. are saved unencrypted,
//  to allow distinguishing between an encrypted and unencrypted file on startup.
//  All task- and time-related data is encrypted.
//  Encryption uses AES256 in CBC mode, using explicit initialization vectors. IVs are randomly generated on every startup.
//  The actual data is prepended by a block of 16 Bytes which is discarded on decryption.
    int i, k, n, idx, len;
    quint32 random_number;
    qint32 timelog_size;
    qint64 JulianDay;
    int size, size_cipher, num_blocks;
    QFile savefile;
    QMap<QDate, sTime>::const_iterator it;
    uint8_t  *pass;
    uint8_t  *plaintext;
    uint8_t  *ciphertext;

    qInfo("Writing encrypted save file: %s",activeFile.SaveFileNameFull.toUtf8().data());
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "writefileEncrypted(): Writing encrypted save file: " << activeFile.SaveFileNameFull.toUtf8().data() << endl;

    savefile.setFileName(activeFile.SaveFileNameFull);

//    if (activeFile.Password.isEmpty()) {
//        qInfo("Warning: Password is empty!");
//    }

    // Set the encryption key:
    memset(&key[0], 0, 32);
    len = 32;
    if (activeFile.Password.length()<32) len = activeFile.Password.length();

    pass = new uint8_t[len];
    memset(pass, 0, len);
    memcpy(pass, activeFile.Password.toUtf8().data(), len);
    pkcs5_pbkdf2(pass, len, activeFile.salt, 16, &key[0], 32, 4096);
    cbc.setKey(key, 32);

    // Set the IV
    for (i=0; i<16; i+=4) {
        random_number = QRandomGenerator::global()->generate();
        memcpy(&iv[i], &random_number, 4);
    }
    cbc.setIV(iv, 16);
    //qInfo("iv = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",iv[0],iv[1],iv[2],iv[3],iv[4],iv[5],iv[6],iv[7],iv[8],iv[9],iv[10],iv[11],iv[12],iv[13],iv[14],iv[15]);
    //qInfo("salt = %02x%02x%02x%02x%02x%02x%02x%02x",activeFile.salt[0],activeFile.salt[1],activeFile.salt[2],activeFile.salt[3],activeFile.salt[4],activeFile.salt[5],activeFile.salt[6],activeFile.salt[7]);
    //qInfo("key = %s",key);

    // Hash the password (unsalted) for storage:
    sha256.clear();
    sha256.update(activeFile.Password.toUtf8().data(), len);
    sha256.finalize(&PasswordHash[0], 32);
    //qInfo("PasswordHash:  %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    //      PasswordHash[0],PasswordHash[1],PasswordHash[2],PasswordHash[3],PasswordHash[4],PasswordHash[5],PasswordHash[6],PasswordHash[7],PasswordHash[8],PasswordHash[9],PasswordHash[10],PasswordHash[11],PasswordHash[12],PasswordHash[13],PasswordHash[14],PasswordHash[15]);

    if (!savefile.open(QIODevice::WriteOnly)) {
        qWarning("Could not open save file: %s",activeFile.SaveFileNameFull.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "writefileEncrypted(): Could not open save file: " << activeFile.SaveFileNameFull.toUtf8().data() << endl;
        return 1;
    }
    QDataStream out(&savefile);
    out.setVersion(QDataStream::Qt_5_0);


    // Calculate size of data to encrypt:
    size = 16;  // 1 block of leading data for CBC block chaining
    for(i=0; i<m_tasks.count(); i++) {
        size += 97 + 32 + 128 + m_tasks.at(i).timelog.size() * 18;  // Per-task time data + title + description + (no. of days * per-day time data)
    }
    //qInfo("size = %d",size);
    plaintext  = new uint8_t[size];

    // Buffer for ciphertext (must be multiple of 16 Bytes)
    num_blocks  = size/16 + 1;
    size_cipher = num_blocks*16;
    //qInfo("size_cipher = %d",size_cipher);

    ciphertext = new uint8_t[size_cipher];
    memset(ciphertext, 0, size_cipher);


    // Magic no., version no., salt & password hash are written unencrypted:
    // Magic number to verify file format
    out << (quint32)0x051076B0;
    // Version number
    out << (quint16) 100;
    // 16 Byte Salt & 32 Byte hashed password
    for (i=0; i<16; i++) {
        out << (quint8) activeFile.salt[i];
    }
    for (i=0; i<32; i++) {
        out << (quint8) PasswordHash[i];
    }


    // Put into buffer for encryption
    idx = 0;
    memset(&plaintext[idx], 0, 16);  idx += 16;  // 1 block of leading data for CBC block chaining
    for(i=0; i<m_tasks.count(); i++) {
        memcpy(&plaintext[idx], &i, 4);  idx += 4;
        memset(&plaintext[idx], 0, 32);
        memcpy(&plaintext[idx], &(m_tasks.at(i).title.toUtf8().data()[0]), m_tasks.at(i).title.size());  // Max. 32 bytes
        idx += 32;
        memset(&plaintext[idx], 0, 128);
        memcpy(&plaintext[idx], &(m_tasks.at(i).description.toUtf8().data()[0]), m_tasks.at(i).description.size());  // Max. 128 bytes
        idx += 128;
        memcpy(&plaintext[idx], &(m_tasks.at(i).taskID), 4);  idx += 4;
        // taskActive is not saved
        // allocateTime is not saved
        // timeTotal
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeTotal.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeTotal.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeTotal.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeTotal.elapsedSeconds), 4);  idx += 4;
        // timeToday
        JulianDay = today.toJulianDay();
        memcpy(&plaintext[idx], &JulianDay, 8);  idx += 8;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeToday.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeToday.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeToday.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeToday.elapsedSeconds), 4);  idx += 4;
        // timeThisMonth
        memcpy(&plaintext[idx], &thisMonth, 1);  idx += 1;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisMonth.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisMonth.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisMonth.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisMonth.elapsedSeconds), 2);  idx += 4;
        // timeThisYear
        memcpy(&plaintext[idx], &thisYear, 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisYear.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisYear.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisYear.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeThisYear.elapsedSeconds), 4);  idx += 4;
        // timeDaily
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeDaily.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeDaily.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeDaily.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeDaily.elapsedSeconds), 2);  idx += 4;
        // timeMonthly
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeMonthly.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeMonthly.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeMonthly.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeMonthly.elapsedSeconds), 2);  idx += 4;
        // timeYearly
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeYearly.Hours), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeYearly.Minutes), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeYearly.Seconds), 2);  idx += 2;
        memcpy(&plaintext[idx], &(m_tasks.at(i).timeYearly.elapsedSeconds), 2);  idx += 4;
        // timelog
        timelog_size = m_tasks.at(i).timelog.size();
        memcpy(&plaintext[idx], &timelog_size, 4);  idx += 4;         //printf("Writing timelog.size(): %d\n",m_tasks.at(i).timelog.size());
        it = m_tasks.at(i).timelog.begin();
        for (n = 0; n < m_tasks.at(i).timelog.size(); n++) {
            JulianDay = it.key().toJulianDay();
            memcpy(&plaintext[idx], &JulianDay, 8);  idx += 8;
            memcpy(&plaintext[idx], &(it.value().Hours), 2);  idx += 2;
            memcpy(&plaintext[idx], &(it.value().Minutes), 2);  idx += 2;
            memcpy(&plaintext[idx], &(it.value().Seconds), 2);  idx += 2;
            memcpy(&plaintext[idx], &(it.value().elapsedSeconds), 4);  idx += 4;
            it++;
        }
        // nextID
        memset(&plaintext[idx], 0, 4);   idx += 4;            //Unused (formerly nextID)
    }
    //qInfo("idx = %d",idx);


    // Encrypt
    cbc.encrypt(&ciphertext[0], &plaintext[0], size_cipher);

    // Write to file
    for (k=0; k<size_cipher; k++) {
        out << ciphertext[k];
    }

    // Clean up
    savefile.close();
    if (pass != NULL)       delete[] pass;
    if (plaintext != NULL)  delete[] plaintext;
    if (ciphertext != NULL) delete[] ciphertext;

    return 0;
}


void CTaskModel::writeIniFile()
{
//  Write all necessary data to ini file in ASCII format
    QFile savefileini("Timekeeper.ini");

    qInfo("writeIniFile(): Writing ini file: Timekeeper.ini");
    logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "writeIniFile(): Writing ini file: Timekeeper.ini" << endl;

    if (!savefileini.open(QIODevice::WriteOnly)) {
        qWarning("Could not open ini file!");
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "writeIniFile(): Could not open ini file!" << endl;
       return;
    }
    QTextStream out(&savefileini);

    out << "[Geometry]" << endl;
    // Configuration settings
    out << "WindowPosX=" << (qint16) m_windowPosXSave << endl;     //qInfo("Saving - m_windowPosXSave = %d    m_windowPosYSave = %d",m_windowPosXSave,m_windowPosYSave);
    out << "WindowPosY=" << (qint16) m_windowPosYSave << endl;     //qInfo("Saving - m_windowPosYSave = %d",m_windowPosYSave);
    out << "WindowWidth=" << (quint16) m_windowWidthSave << endl;    //qInfo("m_windowWidthSave = %d",m_windowWidthSave);
    out << "WindowHeight=" << (quint16) m_windowHeightSave << endl;   //qInfo("m_windowHeightSave = %d",m_windowHeightSave);
    out << "[Settings]" << endl;
    out << "LastSelectedDate=" << (qint16) m_settingLastSelectedDate << endl;
    out << "WeightedReallocation=" << (qint16) m_settingWeightedReallocation << endl;
    out << "Autosave=" << (qint16) m_settingAutosave << endl;
    out << "MinimizeToTray=" << (qint16) m_settingMinimizeToTray << endl;
    out << "StartingView=" << (qint16) m_settingStartingView << endl;
    out << "ColorScheme=" << (qint16) m_settingColorScheme << endl;
    out << "[Profile]" << endl;
    out << "LastSaveFile=" << activeFile.SaveFileNameFull << endl;

    savefileini.close();
}



void CTaskModel::backupfile()
{
//  Backup the current data file
    QFile savefile;
    QFile backupfile;
    QString newname;


    newname = activeFile.SaveFileNameFull;
    newname.append(".bak");
    backupfile.setFileName(newname);

    // Remove old backup:
    backupfile.remove();

    // Copy current save file to backup (does not overwrite an existing file):
    savefile.setFileName(activeFile.SaveFileNameFull);
    if (!savefile.copy(newname)) {
        qWarning("backupfile(): Could not create backup file: %s",newname.toUtf8().data());
        logOut << QDate::currentDate().toString("dd.MM.yyyy").toUtf8().data() << " - " << QTime::currentTime().toString("HH:mm:ss").toUtf8().data() << ": " << "backupfile(): Could not create backup file: " << newname.toUtf8().data() << endl;
    }

}


void CTaskModel::checkDayChange()
{
// Checks periodically if a new day has started and updates necessary data for display
// Only required when no task is active, in which case Update() does the job already

    if (!timer.isActive()) {
        // Only if no task is active:
        if (QDate::currentDate() != today) {
            UpdateAll();
        }
    }

}
