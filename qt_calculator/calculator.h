#ifndef calculator_H
#define calculator_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>


class QPushButton;
class QLineEdit;

class calculator : public QWidget
{
    Q_OBJECT

public:
    calculator(QWidget *parent = 0);


private slots:
    void numberClicked();
    void operatorClicked();
    void clear();

private:
     // creates button
     QPushButton *createButton(const QString &text, const char *member);
     // digits buttons from 1 to F
     QList<QPushButton *> numberButtons;
     QLineEdit *display;
     bool waitingForOperand;
     QString lastOperator;
     std::string leftOperand;
     // does the given operation(+, -, =)
     void doOperation(const QString &_operator);
     // does addition
     bool add(std::string* result, std::string a, std::string b);
     // does subtraction
     bool sub(std::string* result, std::string a, std::string b);
     // calculates the value of operation
     std::string calculate(const QString &_operator, std::string num1, std::string num2);
     int valueInMemory;

};
#endif // calculator_H
