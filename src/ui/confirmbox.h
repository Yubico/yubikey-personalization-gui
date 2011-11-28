/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef CONFIRMBOX_H
#define CONFIRMBOX_H

#include <QDialog>

namespace Ui {
    class ConfirmBox;
}

class ConfirmBox : public QDialog {
    Q_OBJECT

public:
    explicit ConfirmBox(QWidget *parent = 0);
    ~ConfirmBox();

    enum Confirm {
        Confirm_ConfigurationSlot,
        Confirm_PublicID,
        Confirm_ConfigurationProtection,
    };

    void setConfirmIndex(Confirm confirmIndex);

private slots:
    void on_dontaskCheck_stateChanged(int state);

private:
    Ui::ConfirmBox *ui;
    Confirm m_confirmIndex;
};

#endif // CONFIRMBOX_H
