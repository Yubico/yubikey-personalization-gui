/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui/otppage.h"
#include "ui/oathpage.h"
#include "ui/staticpage.h"
#include "ui/chalresppage.h"
#include "ui/settingpage.h"
#include "ui/toolpage.h"
#include "ui/aboutpage.h"
#include "qtimer.h"

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

    QTimer *m_timer;

    void createPages();

private slots:
    void setCurrentPage(int pageIndex);

    // Device Widget
    void resetDeviceInfo();
    void keyFound(bool found, bool* featuresMatrix);

    // Status Widget
    void showStatusMessage(const QString &text, int status = 1);
    void clearStatusMessage();

    void copyToClipboard(const QString &str);
    void on_serialNoDecCopyBtn_clicked();
    void on_serialNoHexCopyBtn_clicked();
    void on_serialNoModhexCopyBtn_clicked();
};

#endif // MAINWINDOW_H
