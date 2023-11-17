#include "errormessage.h"

void ErrorMessage(const QString& message)
{
    QMessageBox box;
    box.setWindowTitle("Ошибка");
    box.setText(message);
    box.exec();
}
