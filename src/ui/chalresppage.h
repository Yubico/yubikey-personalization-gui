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

#ifndef CHALRESPPAGE_H
#define CHALRESPPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"

namespace Ui {
    class ChalRespPage;
}

class ChalRespPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit ChalRespPage(QWidget *parent = 0);
    ~ChalRespPage();

    enum Page {
        Page_Base,
        Page_Quick,     //Yubico OTP
        Page_Advanced   //HMAC-SHA1
    };

public slots:
    void loadSettings();
    void setCurrentPage(int pageIndex);
    void setCurrentSlot(int slot);

private:
    Ui::ChalRespPage *ui;

    int m_currentPage;

    enum State {
        State_Initial,
        State_Programming,
        State_Programming_Multiple,
        State_Programming_Multiple_Auto
    };
    State m_state;
    YubiKeyConfig *m_ykConfig;
    unsigned int m_keysProgrammedCtr;
    bool m_ready;

    bool m_keyPresent;

private slots:
    void connectPages();
    void connectHelpButtons();
    void keyFound(bool found, bool* featuresMatrix);

    void clearState();

    // Quick Page
    void resetQuickPage();
    void freezeQuickPage(bool freeze);

    void on_quickProgramMulKeysBox_clicked(bool checked);
    void on_quickConfigParamsCombo_currentIndexChanged(int index);

    void on_quickPvtIdCheck_stateChanged(int state);
    void on_quickPvtIdTxt_editingFinished();
    void on_quickPvtIdGenerateBtn_clicked();

    void on_quickSecretKeyTxt_editingFinished();
    void on_quickSecretKeyGenerateBtn_clicked();

    void on_quickWriteConfigBtn_clicked();
    void on_quickExportConfigBtn_clicked();
    void on_quickStopBtn_clicked();

    void changeQuickConfigParams();
    void stopQuickConfigWritting();
    bool validateQuickSettings();
    void writeQuickConfig(int mode);
    void quickConfigWritten(bool written, const QString &msg);
    void quickConfigExported(bool written, const QString &msg);

    void quickUpdateResults(bool written, const QString &msg);

    // Advanced Page
    void resetAdvPage();
    void freezeAdvPage(bool freeze);

    void on_advProgramMulKeysBox_clicked(bool checked);
    void on_advConfigParamsCombo_currentIndexChanged(int index);

    void on_advSecretKeyTxt_editingFinished();
    void on_advSecretKeyGenerateBtn_clicked();

    void on_advWriteConfigBtn_clicked();
    void on_advExportConfigBtn_clicked();
    void on_advStopBtn_clicked();

    void changeAdvConfigParams();
    void stopAdvConfigWritting();
    bool validateAdvSettings();
    void writeAdvConfig(int mode);
    void advConfigWritten(bool written, const QString &msg);
    void advConfigExported(bool written, const QString &msg);

    void advUpdateResults(bool written, const QString &msg);

    void requireInput_clicked(bool checked);
    void hmac_lt64_clicked();

signals:
    void showStatusMessage(const QString &text, int status = 0);
    void showHelp(int helpIndex);
};

#endif // CHALRESPPAGE_H
