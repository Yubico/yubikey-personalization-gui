/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "scanedit.h"
#include "qevent.h"
#include "qdebug.h"
#include "common.h"

#define	TAB     0x2b
#define	RETURN  0x28
#define	SHIFT   0x80

static const unsigned char key2usb[0x80] = {
    0x00,               /*   0 0x00 */
    0x00,               /*   1 0x01 */
    0x00,               /*   2 0x02 */
    0x00,               /*   3 0x03 */
    0x00,               /*   4 0x04 */
    0x00,               /*   5 0x05 */
    0x00,               /*   6 0x06 */
    0x00,               /*   7 0x07 */
    0x00,               /*   8 0x08 Backspace (0x2a) */
    0x00,               /*   9 0x09 Tab (0x2b) */
    0x00,               /*  10 0x0a Back Tab (0x2b | SHIFT) */
    0x00,               /*  11 0x0b Home (0x4a)*/
    0x00,               /*  12 0x0c Form Feed */
    0x28,               /*  13 0x0d Return */
    0x00,               /*  14 0x0e */
    0x00,               /*  15 0x0f */
    0x00,               /*  16 0x10 */
    0x00,               /*  17 0x11 */
    0x00,               /*  18 0x12 */
    0x00,               /*  19 0x13 */
    0x00,		/*  20 0x14 */
    0x00,		/*  21 0x15 */
    0x00,		/*  22 0x16 */
    0x00,		/*  23 0x17 */
    0x00,		/*  24 0x18 Cancel (0x9b) */
    0x00,		/*  25 0x19 */
    0x00,		/*  26 0x1a */
    0x00,               /*  27 0x1b Escape (0x29) */
    0x00,               /*  28 0x1c */
    0x00,		/*  29 0x1d */
    0x00,		/*  30 0x1e */
    0x00,               /*  31 0x1f */
    0x2c,		/*  32 0x20   */
    0x1e | SHIFT,   	/*  33 0x21 ! */
    0x34 | SHIFT,	/*  34 0x22 " */
    0x20 | SHIFT,	/*  35 0x23 # */
    0x21 | SHIFT,	/*  36 0x24 $ */
    0x22 | SHIFT,	/*  37 0x25 % */
    0x24 | SHIFT,	/*  38 0x26 & */
    0x34,		/*  39 0x27 ' */
    0x26 | SHIFT,	/*  40 0x28 ( */
    0x27 | SHIFT,	/*  41 0x29 ) */
    0x25 | SHIFT,	/*  42 0x2a * */
    0x2e | SHIFT,	/*  43 0x2b + */
    0x36,		/*  44 0x2c , */
    0x2d,		/*  45 0x2d - */
    0x37,		/*  46 0x2e . */
    0x38,		/*  47 0x2f / */
    0x27,		/*  48 0x30 0 */
    0x1e,		/*  49 0x31 1 */
    0x1f,		/*  50 0x32 2 */
    0x20,		/*  51 0x33 3 */
    0x21,		/*  52 0x34 4 */
    0x22,		/*  53 0x35 5 */
    0x23,		/*  54 0x36 6 */
    0x24,		/*  55 0x37 7 */
    0x25,		/*  56 0x38 8 */
    0x26,		/*  57 0x39 9 */
    0x33 | SHIFT,	/*  58 0x3a : */
    0x33,		/*  59 0x3b ; */
    0x36 | SHIFT,	/*  60 0x3c < */
    0x2e,		/*  61 0x3d = */
    0x37 | SHIFT,	/*  62 0x3e > */
    0x38 | SHIFT,	/*  63 0x3f ? */
    0x1f | SHIFT,	/*  64 0x40 @ */
    0x04 | SHIFT,	/*  65 0x41 A */
    0x05 | SHIFT,	/*  66 0x42 B */
    0x06 | SHIFT,	/*  67 0x43 C */
    0x07 | SHIFT,	/*  68 0x44 D */
    0x08 | SHIFT,	/*  69 0x45 E */
    0x09 | SHIFT,	/*  70 0x46 F */
    0x0a | SHIFT,	/*  71 0x47 G */
    0x0b | SHIFT,	/*  72 0x48 H */
    0x0c | SHIFT,	/*  73 0x49 I */
    0x0d | SHIFT,	/*  74 0x4a J */
    0x0e | SHIFT,	/*  75 0x4b K */
    0x0f | SHIFT,	/*  76 0x4c L */
    0x10 | SHIFT,	/*  77 0x4d M */
    0x11 | SHIFT,	/*  78 0x4e N */
    0x12 | SHIFT,	/*  79 0x4f O */
    0x13 | SHIFT,	/*  80 0x50 P */
    0x14 | SHIFT,	/*  81 0x51 Q */
    0x15 | SHIFT,	/*  82 0x52 R */
    0x16 | SHIFT,	/*  83 0x53 S */
    0x17 | SHIFT,	/*  84 0x54 T */
    0x18 | SHIFT,	/*  85 0x55 U */
    0x19 | SHIFT,	/*  86 0x56 V */
    0x1a | SHIFT,	/*  87 0x57 W */
    0x1b | SHIFT,	/*  88 0x58 X */
    0x1c | SHIFT,	/*  89 0x59 Y */
    0x1d | SHIFT,	/*  90 0x5a Z */
    0x2f,		/*  91 0x5b [ */
    0x32,		/*  92 0x5c \ */
    0x30,		/*  93 0x5d ] */
    0X23 | SHIFT,	/*  94 0x5e ^ */
    0x2d | SHIFT,	/*  95 0x5f _ */
    0x35,		/*  96 0x60 ` */
    0x04,		/*  97 0x61 a */
    0x05,		/*  98 0x62 b */
    0x06,		/*  99 0x63 c */
    0x07,		/* 100 0x64 d */
    0x08,		/* 101 0x65 e */
    0x09,		/* 102 0x66 f */
    0x0a,		/* 103 0x67 g */
    0x0b,		/* 104 0x68 h */
    0x0c,		/* 105 0x69 i */
    0x0d,		/* 106 0x6a j */
    0x0e,		/* 107 0x6b k */
    0x0f,		/* 108 0x6c l */
    0x10,		/* 109 0x6d m */
    0x11,		/* 110 0x6e n */
    0x12,		/* 111 0x6f o */
    0x13,		/* 112 0x70 p */
    0x14,		/* 113 0x71 q */
    0x15,		/* 114 0x72 r */
    0x16,		/* 115 0x73 s */
    0x17,		/* 116 0x74 t */
    0x18,		/* 117 0x75 u */
    0x19,		/* 118 0x76 v */
    0x1a,		/* 119 0x77 w */
    0x1b,		/* 120 0x78 x */
    0x1c,		/* 121 0x79 y */
    0x1d,		/* 122 0x7a z */
    0x2f | SHIFT,	/* 123 0x7b { */
    0x32 | SHIFT,	/* 124 0x7c | */
    0x30 | SHIFT,	/* 125 0x7d } */
    0x35 | SHIFT,	/* 126 0x7e ~ */
    0x00,		/* 127 0x7f  */
};

ScanEdit::ScanEdit(QWidget *parent) : QLineEdit(parent) {
    this->setContextMenuPolicy( Qt::NoContextMenu );
    clearScanCodeText();
}

void ScanEdit::injectText(const QString &txt) {
    setText(tr("%1%2").arg(text()).arg(txt));
}

int ScanEdit::len() const {
    QString txt = text();
    txt = txt.replace(QRegExp("\\\\[tn]"), QString("1"));
    return txt.length();
}

void ScanEdit::injectScanCode(unsigned char code) {
    if (!code) return;

    QString hexTxt = YubiKeyUtil::qstrHexEncode(&code, 1);
    m_scanCodeText.append(hexTxt);

    // Handle special characters...
    switch(code) {
    case TAB:
        injectText(tr("\\t"));
        break;
    case RETURN:
        injectText(tr("\\n"));
        break;
    }
}

void ScanEdit::clearScanCodeText() {
    m_scanCodeText.clear();
}

QString ScanEdit::scanCodeText() const {
    return m_scanCodeText;
}

void ScanEdit::keyPressEvent(QKeyEvent *event) {
    qDebug() << "key text:" << event->text();
    qDebug() << "key code:" << event->key();
    //qDebug() << "scan code:" << event->nativeScanCode();
    //qDebug() << "usb key code:" << trans[event->nativeScanCode() & 0x7f];

    // Check if text length exceeds max scan edit length
    if(len() >= MAX_SCAN_EDIT_SIZE) {
        event->ignore();
        return;
    }

    switch(event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        injectScanCode(RETURN);
        break;

    default:
        if(event->key() < SHIFT) {
            QChar ch = event->text().at(0).toAscii();
            int chValue = ch.toAscii();

            injectScanCode( key2usb[chValue] );
            injectText(event->text());
        } else {
            event->ignore();
            return;
        }
        break;
    }
}

void ScanEdit::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    setCursorPosition(text().length());
}
