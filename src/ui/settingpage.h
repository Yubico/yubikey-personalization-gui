/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QWidget>

namespace Ui {
    class SettingPage;
}

class SettingPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingPage(QWidget *parent = 0);
    ~SettingPage();
    void init();

private:
    Ui::SettingPage *ui;

private slots:
    void connectHelpButtons();
    void helpBtn_pressed(int helpIndex);
    void restoreDefaults();
    void load();
    void save();
    void restore();

    void on_custPrefixCheck_stateChanged(int state);
    void on_custPrefixTxt_editingFinished();
    void on_logOutputCheck_stateChanged(int state);
    void on_browseBtn_clicked();

signals:
    void settingsChanged();
    void showStatusMessage(const QString &text, int status = 0);
};

#endif // SETTINGPAGE_H
