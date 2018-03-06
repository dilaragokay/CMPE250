#include <QApplication>
#include "calculator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    calculator calc;
    calc.show();
    return app.exec();
}
