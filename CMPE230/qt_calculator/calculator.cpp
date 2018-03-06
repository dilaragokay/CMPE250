#include <QtGui>
#include <QGridLayout>
#include <QSpacerItem>
#include <QMessageBox>
#include <iostream>
#include "calculator.h"
#include <limits.h>
#include <math.h>

calculator::calculator(QWidget *parent)
    : QWidget(parent)
{

    lastOperator = tr("=");                     // last operand is assumed to be =
    display = new QLineEdit("0");               // display screen begins with 0
    display->setAlignment(Qt::AlignRight);      // alignment of display number is right
    display->setReadOnly(true);
    display->setMaxLength(100);

    // number buttons are added to QList
    for (int i = 0; i < 16; i++) {
        if (i < 10)
            numberButtons.append(createButton(QString::number(i), SLOT(numberClicked())));
        else if(i == 10)
            numberButtons.append(createButton(tr("A"), SLOT(numberClicked())));
        else if(i == 11)
            numberButtons.append(createButton(tr("B"), SLOT(numberClicked())));
        else if(i == 12)
            numberButtons.append(createButton(tr("C"), SLOT(numberClicked())));
        else if(i == 13)
            numberButtons.append(createButton(tr("D"), SLOT(numberClicked())));
        else if(i == 14)
            numberButtons.append(createButton(tr("E"), SLOT(numberClicked())));
        else
            numberButtons.append(createButton(tr("F"), SLOT(numberClicked())));
    }

    // operator buttons are created
    QPushButton *plusButton = createButton(tr("+"), SLOT(operatorClicked()));
    QPushButton *equalButton = createButton(tr("="), SLOT(operatorClicked()));
    QPushButton *clearButton = createButton(tr("C"), SLOT(clear()));
    QPushButton *minusButton = createButton(tr("-"), SLOT(operatorClicked()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    // space between display screen and buttons are set
    QSpacerItem *space = new QSpacerItem(0,20);

    mainLayout->addWidget(display, 0, 0, 1, 5);
    mainLayout->addItem(space, 1, 0);
    mainLayout->addWidget(clearButton, 3, 4);

    // number buttons are added to layout
    for (int j = 0; j < 16; ++j) {
        int row = j / 4 + 4;
        int column = (j % 4) + 1;
        mainLayout->addWidget(numberButtons.at(j), row, column);
    }
    // operation buttons are added to layout
    mainLayout->addWidget(minusButton, 3, 1);
    mainLayout->addWidget(plusButton, 3, 2);
    mainLayout->addWidget(equalButton, 3, 3);
    setLayout(mainLayout);
    setWindowTitle(tr("Calculator"));
}

// if a number is clicked
void calculator::numberClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }

    QString strValue = clickedButton->text();       // number on button is converted into QString
    int digitValue = 0;                             // decimal value of the clicked number
    if(strValue == "A")
        digitValue = 10;
    else if(strValue == "B")
        digitValue = 11;
    else if(strValue == "C")
        digitValue = 12;
    else if(strValue == "D")
        digitValue = 13;
    else if(strValue == "E")
        digitValue = 14;
    else if(strValue == "F")
        digitValue = 15;
    else
        digitValue = strValue.toInt();

    const char *hexstring = display->text().toStdString().c_str();  // number is converted into char array
    long long int displayValue;
    displayValue = strtoll (hexstring, NULL, 16);                   // number is converted from hex to long long int
    // number is added to the displayed number (as value)
    long long int result = digitValue==0?displayValue*16  :displayValue*16 + digitValue*(digitValue / abs(digitValue));
    display->setText(QString::number(result, 16).toUpper());        // number is added to the displayed number (as display)
}
// when an operator is clicked
void calculator::operatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

    doOperation(clickedButton->text());
}
// clear the screen and display 0
void calculator::clear()
{
    display->setText(tr("0"));
}
// calculation process is initiated
void calculator::doOperation(const QString &_operator)
{
    if(!waitingForOperand)
    {
        if(lastOperator == "=")
        {
            leftOperand = display->text().toStdString();
        }
        else
        {
            std::string result = calculate(lastOperator, leftOperand, display->text().toStdString());
            leftOperand = result;
            display->setText(QString::fromStdString(result));
        }
        waitingForOperand = true;
    }
    lastOperator = _operator;
}
// calculation is done according to operator and operands
std::string calculator::calculate(const QString &_operator, std::string num1, std::string num2)
{
    std::string mResult;
    if(_operator == tr("+") && add(&mResult, num1, num2))
    {
        return mResult;
    }
    if(_operator == tr("-") && sub(&mResult, num1, num2))
    {
        return mResult;
    }
    QMessageBox::warning(this, tr("Error!"), tr("Long Long Integer overflow."), QMessageBox::Close);
    return num1;
}
// create button with QPushButton
QPushButton *calculator::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

// does addition
// calculation is done in decimal and then the result is converted into hex
bool calculator::add(std::string* result, std::string a, std::string b)
{
    const char *hexstring = a.c_str();          // number is converted into char array
    long long int aValue;
    aValue = strtoll (hexstring, NULL, 16);     // number is converted from hex to decimal
    const char *hexstring2 = b.c_str();         // same things are done for the second number
    long long int bValue;
    bValue = strtoll (hexstring2, NULL, 16);
    long long int sum = aValue + bValue;        // numbers are added
    *result = QString::number(sum, 16).toUpper().toStdString(); // summation is converted from decimal to hex and to std::string
    if(aValue > 0 && bValue > 0 && sum < 0)     // if the signs of calculation is mistaken, return false
        return false;
    if(aValue < 0 && bValue < 0 && sum > 0)
        return false;
    return true;
}

// does subtraction
// calculation is done add method
bool calculator::sub(std::string* result, std::string a, std::string b)
{
    const char *hexstring = a.c_str();      // number is converted into character array
    long long int aValue;
    aValue = strtoll (hexstring, NULL, 16); // value of number is converted from hex to decimal
    const char *hexstring2 = b.c_str();     // same things are done for the second number
    long long int bValue;
    bValue = strtoll (hexstring2, NULL, 16);
    // subtract smaller number from the larger
    if(bValue > aValue){
        a = '-' + a;
        bool ret = add(result, a, b );
        result->insert(0, "-");             // put - sign in front of it if the result should be negative
        return ret;
    }
    else{
        b = '-' + b;
        return add(result, a, b );
    }
}
