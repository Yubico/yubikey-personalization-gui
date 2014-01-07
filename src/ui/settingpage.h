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

#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"

namespace Ui {
    class SettingPage;
}

class SettingPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit SettingPage(QWidget *parent = 0);
    ~SettingPage();
    void init();

    enum Page {
        Page_Base,
        Page_Update,
    };

private:
    Ui::SettingPage *ui;

    int m_currentPage;

    YubiKeyConfig *m_ykConfig;

public slots:
    void reloadSettings();
    void setCurrentPage(int pageIndex);

private slots:
    void connectHelpButtons();
    void restoreDefaults();
    void load();
    void save();
    void restore();

    void on_custPrefixCheck_stateChanged(int state);
    void on_custPrefixDecTxt_editingFinished();
    void on_custPrefixModhexTxt_editingFinished();
    void on_custPrefixHexTxt_editingFinished();

    void on_doUpdateBtn_clicked();
    void on_swapBtn_clicked();
    void updateConfigWritten(bool written, const QString &msg);
    void swapWritten(bool written, const QString &msg);

    void keyFound(bool found, bool* featuresMatrix);

    void custPrefixChanged(int type, QString src);
signals:
    void settingsChanged();
    void showStatusMessage(const QString &text, int status = 0);
    void showHelp(int helpIndex);
};

#endif // SETTINGPAGE_H
