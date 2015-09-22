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

#ifndef STATICPAGE_H
#define STATICPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"
#include "scanedit.h"

namespace Ui {
    class StaticPage;
}

class StaticPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit StaticPage(QWidget *parent = 0);
    ~StaticPage();

    enum Page {
        Page_Base,
        Page_Quick,
        Page_Advanced,
        Page_Upload
    };

private:
    Ui::StaticPage *ui;

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

    ScanEdit *scanedit;

public slots:
    void loadSettings();
    void setCurrentPage(int pageIndex);
    void setCurrentSlot(int slot);

private slots:
    void connectPages();
    void connectHelpButtons();
    void keyFound(bool found, bool* featuresMatrix);

    void clearState();

    // Quick Page
    void resetQuickPage();
    void freezeQuickPage(bool freeze);

    void on_quickHideParams_clicked(bool checked);
    void on_quickStaticTxt_textEdited(const QString &txt);
    void on_quickStaticTxt_returnPressed();
    void on_quickInsertTabBtn_clicked();
    void on_quickClearBtn_clicked();

    void on_quickWriteConfigBtn_clicked();
    void on_quickStopBtn_clicked();

    void changeQuickConfigParams();
    void stopQuickConfigWritting();
    bool validateQuickSettings();
    void writeQuickConfig();
    void quickConfigWritten(bool written, const QString &msg);

    void quickUpdateResults(bool written, const QString &msg);


    // Advanced Page
    void resetAdvPage();
    void freezeAdvPage(bool freeze);

    void on_advProgramMulKeysBox_clicked(bool checked);
    void on_advConfigParamsCombo_currentIndexChanged(int index);

    void on_advStaticLen16Radio_clicked(bool checked);
    void on_advStaticLen32Radio_clicked(bool checked);
    void on_advStaticLenBox_valueChanged(int value);

    void enablePubId(bool enable);
    void on_advPubIdTxt_editingFinished();
    void on_advPubIdGenerateBtn_clicked();

    void on_advPvtIdTxt_editingFinished();
    void on_advPvtIdGenerateBtn_clicked();

    void on_advSecretKeyTxt_editingFinished();
    void on_advSecretKeyGenerateBtn_clicked();

    void on_advStrongPw1Check_stateChanged(int state);
    void on_advStrongPw2Check_stateChanged(int state);
    void on_advStrongPw3Check_stateChanged(int state);

    void on_advWriteConfigBtn_clicked();
    void on_advStopBtn_clicked();

    void changeAdvConfigParams();
    void stopAdvConfigWritting();
    bool validateAdvSettings();
    void writeAdvConfig();
    void advConfigWritten(bool written, const QString &msg);

    void advUpdateResults(bool written, const QString &msg);

    void on_quickScanCodesTxt_textEdited(const QString &text);

    void on_quickKeymapCmb_currentIndexChanged(int index);

signals:
    void showStatusMessage(const QString &text, int status = 0);
    void showHelp(int helpIndex);
};

#endif // STATICPAGE_H
