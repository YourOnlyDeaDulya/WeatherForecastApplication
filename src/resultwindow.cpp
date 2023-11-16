#include "resultwindow.h"

ResultWindow::ResultWindow(QWidget *parent) :
    QDialog(parent)
{
}

ResultWindow::~ResultWindow()
{
}

QFont ResultWindow::CreateTextFont()
{
    QFont font;
    font.setPixelSize(30);

    return font;
}
