/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
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

private:
    Ui::ChalRespPage *ui;

    enum Page {
        Page_Base,
        Page_Quick,     //Yubico OTP
        Page_Advanced   //HMAC-SHA1
    };
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

private slots:
    void connectPages();
    void connectHelpButtons();
    void setCurrentPage(int pageIndex);
    void helpBtn_pressed(int helpIndex);
    void keyFound(bool found, bool* featuresMatrix);

    void clearState();

    // Quick Page
    void resetQuickPage();
    void freezeQuickPage(bool freeze);

    void on_quickProgramMulKeysBox_clicked(bool checked);
    void on_quickConfigParamsCombo_currentIndexChanged(int index);

    void on_quickConfigProtectionCombo_currentIndexChanged(int index);
    void on_quickCurrentAccessCodeTxt_editingFinished();
    void on_quickNewAccessCodeTxt_editingFinished();

    void on_quickPvtIdCheck_stateChanged(int state);
    void on_quickPvtIdTxt_editingFinished();
    void on_quickPvtIdGenerateBtn_clicked();

    void on_quickSecretKeyTxt_editingFinished();
    void on_quickSecretKeyGenerateBtn_clicked();

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

    void on_advConfigProtectionCombo_currentIndexChanged(int index);
    void on_advCurrentAccessCodeTxt_editingFinished();
    void on_advNewAccessCodeTxt_editingFinished();

    void on_advSecretKeyTxt_editingFinished();
    void on_advSecretKeyGenerateBtn_clicked();

    void on_advWriteConfigBtn_clicked();
    void on_advStopBtn_clicked();

    void changeAdvConfigParams();
    void stopAdvConfigWritting();
    bool validateAdvSettings();
    void writeAdvConfig();
    void advConfigWritten(bool written, const QString &msg);

    void advUpdateResults(bool written, const QString &msg);

signals:
    void showStatusMessage(const QString &text, int status = 0);
};

#endif // CHALRESPPAGE_H
