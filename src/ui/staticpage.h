/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef STATICPAGE_H
#define STATICPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"

namespace Ui {
    class StaticPage;
}

class StaticPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit StaticPage(QWidget *parent = 0);
    ~StaticPage();

private:
    Ui::StaticPage *ui;

    enum Page {
        Page_Base,
        Page_Quick,
        Page_Advanced,
        Page_Upload
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

    void on_quickConfigProtectionCombo_currentIndexChanged(int index);
    void on_quickCurrentAccessCodeTxt_editingFinished();
    void on_quickNewAccessCodeTxt_editingFinished();

    void on_quickHideParams_clicked(bool checked);
    void on_quickStaticTxt_textChanged(const QString &txt);
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

    void on_advConfigProtectionCombo_currentIndexChanged(int index);
    void on_advCurrentAccessCodeTxt_editingFinished();
    void on_advNewAccessCodeTxt_editingFinished();

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

signals:
    void showStatusMessage(const QString &text, int status = 0);
};

#endif // STATICPAGE_H
