#include "customLineedit.h"

CustomLineEdit::CustomLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}

void CustomLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Plus) {
        insert("∨");
    }
    else if (event->key() == Qt::Key_Exclam) {
        insert("¬");
    }
    else if (event->key() == Qt::Key_Asterisk) {
        insert("∧");
    }
    else if (event->key() == Qt::Key_Period) {
        insert("⇒");
    }
    else if (event->key() == Qt::Key_Equal) {
        insert("⇔");
    }
    else {
        QLineEdit::keyPressEvent(event);
    }
}
