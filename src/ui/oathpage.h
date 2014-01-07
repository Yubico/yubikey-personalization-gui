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

#ifndef OATHPAGE_H
#define OATHPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"
#include "common.h"

namespace Ui {
    class OathPage;
}

class OathPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit OathPage(QWidget *parent = 0);
    ~OathPage();

    enum Page {
        Page_Base,
        Page_Quick,
        Page_Advanced
    };

private:
    Ui::OathPage *ui;

    int m_currentPage;

    int m_customerPrefix;
    unsigned char m_pubId[2];
    unsigned int m_pubIdMUI;

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
    bool m_serialNumberSupported;

    bool m_keyPresent;

public slots:
    void loadSettings();
    QString getPublicId(bool bcd);
    void setCurrentPage(int pageIndex);
    void setCurrentSlot(int slot);

private slots:
    void connectPages();
    void connectHelpButtons();
    void keyFound(bool found, bool* featuresMatrix);

    void updatePrefix();
    void fixBCD(unsigned char *bp, int bcnt);
    void clearState();

    // Quick Page
    void resetQuickPage();
    void on_quickResetBtn_clicked();

    void on_quickHideParams_clicked(bool checked);
    void on_quickPubIdCheck_stateChanged(int state);
    void resetQuickPrefix();
    void updateQuickMUI();
    void on_quickMUITxt_editingFinished();
    void on_quickMUIGenerateBtn_clicked();
    void on_quickSecretKeyTxt_editingFinished();

    bool validateQuickSettings();
    void writeQuickConfig();
    void quickConfigWritten(bool written, const QString &msg);

    // Advanced Page
    void resetAdvPage();
    void freezeAdvPage(bool freeze);

    void on_advProgramMulKeysBox_clicked(bool checked);
    void on_advConfigParamsCombo_currentIndexChanged(int index);

    void on_advPubIdFormatCombo_currentIndexChanged(int index);
    void on_advPubIdCheck_stateChanged(int state);
    void updateAdvOMP(int index);
    void on_advOMPTxt_editingFinished();
    void updateAdvTT(int index);
    void on_advTTTxt_editingFinished();
    void updateAdvMUI(int index);
    void on_advMUITxt_editingFinished();
    void on_advMUIGenerateBtn_clicked();

    void hotpLen_clicked();

    void on_advMovingFactorSeedCombo_currentIndexChanged(int index);
    void on_advMovingFactorSeedTxt_editingFinished();

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

signals:
    void showStatusMessage(const QString &text, int status = 0);
    void showHelp(int helpIndex);
};

#endif // OATHPAGE_H
