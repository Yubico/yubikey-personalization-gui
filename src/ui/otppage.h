/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef OTPPAGE_H
#define OTPPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"

namespace Ui {
    class OtpPage;
}

class OtpPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit OtpPage(QWidget *parent = 0);
    ~OtpPage();

private:
    Ui::OtpPage *ui;

    enum Page {
        Page_Base,
        Page_Quick,
        Page_Advanced,
        Page_Upload
    };
    int m_currentPage;

    int m_customerPrefix;
    QString m_pubIdPrefix;

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

    QString m_uploadUrl;

public slots:
    void loadSettings();

private slots:
    void connectPages();
    void connectHelpButtons();
    void setCurrentPage(int pageIndex);
    void helpBtn_pressed(int helpIndex);
    void keyFound(bool found, bool* featuresMatrix);

    void clearState();

    // Quick Page
    void resetQuickPage();
    void on_quickResetBtn_clicked();

    void on_quickHideParams_clicked(bool checked);
    void on_quickPubIdTxt_editingFinished();
    void on_quickPvtIdTxt_editingFinished();
    void on_quickSecretKeyTxt_editingFinished();

    bool validateQuickSettings();
    void writeQuickConfig();
    void quickConfigWritten(bool written, const QString &msg);
    void uploadQuickConfig();

    // Advanced Page
    void resetAdvPage();
    void freezeAdvPage(bool freeze);

    void on_advProgramMulKeysBox_clicked(bool checked);
    void on_advConfigParamsCombo_currentIndexChanged(int index);

    void on_advConfigProtectionCombo_currentIndexChanged(int index);
    void on_advCurrentAccessCodeTxt_editingFinished();
    void on_advNewAccessCodeTxt_editingFinished();

    void on_advPubIdCheck_stateChanged(int state);
    void on_advPubIdTxt_editingFinished();
    void on_advPubIdGenerateBtn_clicked();
    void on_advPubIdLenBox_valueChanged(int value);

    void on_advPvtIdCheck_stateChanged(int state);
    void on_advPvtIdTxt_editingFinished();
    void on_advPvtIdGenerateBtn_clicked();

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

#endif // OTPPAGE_H
