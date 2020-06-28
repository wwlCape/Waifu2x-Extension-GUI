﻿/*
    Copyright (C) 2020  Aaron Feng

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/AaronFeng753
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
online update for QRCode at donate tab
*/
int MainWindow::Donate_DownloadOnlineQRCode()
{
    //============================
    QString Github_OnlineQRCode = Current_Path+"/Online_Donate_QRCode_Github.jpg";
    QString Gitee_OnlineQRCode = Current_Path+"/Online_Donate_QRCode_Gitee.jpg";
    QFile::remove(Github_OnlineQRCode);
    QFile::remove(Gitee_OnlineQRCode);
    //===========================
    QString program = Current_Path+"/python_ext_waifu2xEX.exe";
    QProcess DownloadOnlineQRCode;
    DownloadOnlineQRCode.start("\""+program+"\" \""+Current_Path+"\" DownloadDonateQRCode");
    while(!DownloadOnlineQRCode.waitForStarted(500)&&!QProcess_stop) {}
    while(!DownloadOnlineQRCode.waitForFinished(500)&&!QProcess_stop) {}
    //========= 先检查github的文件是否下载成功 =================
    QFileInfo *Github_OnlineQRCode_QFileInfo = new QFileInfo(Github_OnlineQRCode);
    if(QFile::exists(Github_OnlineQRCode)&&(Github_OnlineQRCode_QFileInfo->size()>1000))
    {
        emit Send_Donate_ReplaceQRCode(Github_OnlineQRCode);
        return 1;
    }
    //========= 再检查gitee的文件是否下载成功 =================
    QFileInfo *Gitee_OnlineQRCode_QFileInfo = new QFileInfo(Gitee_OnlineQRCode);
    if(QFile::exists(Gitee_OnlineQRCode)&&(Gitee_OnlineQRCode_QFileInfo->size()>1000))
    {
        emit Send_Donate_ReplaceQRCode(Gitee_OnlineQRCode);
        return 1;
    }
    emit Send_Donate_ReplaceQRCode("");//下载失败,直接跳转
    return 0;
}

void MainWindow::Donate_ReplaceQRCode(QString QRCodePath)
{
    if(QRCodePath=="")
    {
        Donate_Count();
        return;
    }
    else
    {
        ui->label_DonateQRCode->setStyleSheet("image: url("+QRCodePath+");");
        Donate_Count();
        return;
    }
}

int MainWindow::Donate_Count()
{
    QString donate_ini = Current_Path+"/StartupStatistics_Waifu2xEX";
    if(!file_isFileExist(donate_ini))
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Description/.", "This file will store how many times you start this software, it's not for statistic, just for software to function normally, and won't be upload to the internet in any cases.");
        configIniWrite->setValue("/Donate/VERSION", VERSION);
        configIniWrite->setValue("/Donate/OpenCount_Current", 3);
        return 0;
    }
    else
    {
        QSettings *configIniRead_ver = new QSettings(donate_ini, QSettings::IniFormat);
        configIniRead_ver->setIniCodec(QTextCodec::codecForName("UTF-8"));
        QString DonateINI_VERSION = configIniRead_ver->value("/Donate/VERSION").toString();
        if(DonateINI_VERSION!=VERSION)
        {
            QFile::remove(donate_ini);
            QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
            configIniWrite->setValue("/Description/.", "This file will store how many times you start this software, it's not for statistic, just for software to function normally, and won't be upload to the internet in any cases.");
            configIniWrite->setValue("/Donate/VERSION", VERSION);
            configIniWrite->setValue("/Donate/OpenCount_Current", 3);
            return 0;
        }
    }
    QSettings *configIniRead = new QSettings(donate_ini, QSettings::IniFormat);
    //=======  读取打开次数  ======
    int OpenCount_Current = configIniRead->value("/Donate/OpenCount_Current").toInt();
    int OpenCount_Max = 5;
    //===
    if(OpenCount_Current<=0)OpenCount_Current=OpenCount_Max;
    //===
    int qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    int DonateTestNum =qrand()%1000;
    QSettings *configIniWrite_Test = new QSettings(donate_ini, QSettings::IniFormat);
    configIniWrite_Test->setValue("/Donate/Test", DonateTestNum);
    if(configIniRead->value("/Donate/Test").toInt()!=DonateTestNum)
    {
        OpenCount_Current=OpenCount_Max;
    }
    //===
    OpenCount_Current++;
    if(OpenCount_Current<OpenCount_Max)
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Donate/OpenCount_Current", OpenCount_Current);
        return 0;
    }
    else
    {
        QSettings *configIniWrite = new QSettings(donate_ini, QSettings::IniFormat);
        configIniWrite->setValue("/Donate/OpenCount_Current", 1);
        ui->tabWidget->setCurrentIndex(0);
        emit Send_SystemTray_NewMessage(tr("Please donate to support developers, so we can bring further update for this software, thank you! (｡･∀･)ﾉﾞ"));
        return 0;
    }
}
