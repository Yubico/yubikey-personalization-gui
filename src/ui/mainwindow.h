/*
Copyright (C) 2011-2014 Yubico AB.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtimer.h"

class ToolPage;
class OtpPage;
class OathPage;
class StaticPage;
class ChalRespPage;
class SettingPage;
class AboutPage;

class Diagnostics;
class HelpBox;

class QStackedWidget;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum Page {
        Page_Otp,
        Page_Oath,
        Page_Static,
        Page_ChalResp,
        Page_Settings,
        Page_Tools,
        Page_About
    };

private:
    Ui::MainWindow *ui;

    int m_currentPage;

    OtpPage *m_otpPage;
    OathPage *m_oathPage;
    StaticPage *m_staticPage;
    ChalRespPage *m_chalRespPage;
    SettingPage *m_settingPage;
    ToolPage *m_toolPage;
    AboutPage *m_aboutPage;

    Diagnostics *m_diagnostics;
    HelpBox *m_help;

    QTimer *m_timer;

    QAction *animationAction;
    QAction *diagnosticsAction;

    void createPages();

private slots:
    void setCurrentPage(int pageIndex, int tab = 0, int slot = 0);

    // Device Widget
    void resetDeviceInfo();
    void keyFound(bool found, bool* featuresMatrix, int error);

    // Status Widget
    void showStatusMessage(const QString &text, int status = 1);
    void clearStatusMessage();

    void copyToClipboard(const QString &str);
    void on_serialNoDecCopyBtn_clicked();
    void on_serialNoHexCopyBtn_clicked();
    void on_serialNoModhexCopyBtn_clicked();

    void toggleAnimation(bool checked);
    void triggerDiagnostics();
    void triggerHelp(int index);

    void aboutToQuit();
};

#endif // MAINWINDOW_H
