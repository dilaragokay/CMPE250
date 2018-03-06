/*
Student Name:
Student Number:
Project Number:
Operating System:
Compile Status:
Program Status:
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include "BigInteger.h"
#include <algorithm>

/**
 * Constructs a BigInteger with given integer number. 
 * Numbers are in reversed order
 * @param number the integer number which the object will contain
 * @return BigInteger object
 */
BigInteger::BigInteger(int number) {
    num = new LinkedList;               //initializes num pointer
    //Adds digits of number to BigInteger, beginning from the last digit
    while(number/10>0){                 
        num->pushTail(number%10);
        number/=10;
    }
    num->pushTail(number);              //adds the first digit of number to BigInteger
}
/**
 * Constructs a BigInteger with given number inside the string. 
 * Numbers are in reversed order
 * @param bigInteger the string that contains a big integer inside
 * @return BigInteger object
 */
BigInteger::BigInteger(const string& bigInteger){
    num = new LinkedList;                               //initializes num pointer
    //Adds characters of bigInteger to BigInteger, beginning from the last character
    for(int i = bigInteger.size() - 1; i >= 0; i--){
        int n = bigInteger.at(i) - '0';                 //typecasts from char to int & assigns to n
        num->pushTail(n);
    }
}
/**
 * Adds up two big integers
 * Stores one digit of big integer in each node
 * @param list the big integer that will be added to this big integer
 * @return the addition of list and this big integer
 */
BigInteger BigInteger::operator+(const BigInteger &list){
    BigInteger ret;                                     //result of addition
    *(ret.num) = *(this->num) + *(list.num);            //adds up two LinkedLists regardless of # of digits in Nodes
    Node* ptr = ret.num->head;
    //Sends the quotient of 10 of each node to the next one, if exists
    while(ptr->next!= nullptr){
        int quotient = (ptr->data)/10;                 //stores the quotient
        ptr->data = ptr->data%10;                      //the data is now the remainder of 10
        ptr->next->data = ptr->next->data + quotient;  //adds the quotient of data to the data of next node
        ptr = ptr->next;
    }
    //If the quotient of last node is not 0, initializes a new Node pointer to that one
    if(ptr->data/10>0){
        int quotient = (ptr->data)/10;
        ptr->data = ptr->data%10;
        ptr->next = new Node(quotient);
    }
    return ret;
}
/**
 * Multiplies two big integers
 * Selects a pivot pointer that points to the head of the LinkedList of list
 * Multiplies this BigInteger with pivot node and adds up to result BigInteger
 * At each time pivot = pivot->next, according to # of stage, adds 0 to the head of LinkedList of intermediate big integer
 * (in other words, multiplies the intermediate big integer with 10^stage)
 * @param list the big integer that will be multiplied with this big integer
 * @return the multiplication of list and this big integer
 */
BigInteger BigInteger::operator*(const BigInteger &list){
    BigInteger ret(0);                              //the result of multiplication
    Node* ptr1 = list.num->head;                    //pivot pointer
    Node* ptr2 = this->num->head;
    int stage = 0;
    int quotient = 0;                               //quotient that will go to the next node
    while(ptr1!= nullptr){
        BigInteger intNum("");                      //initializes intermediate big integer
        //Multiplies two big integers with putting one digit to each node
        while(ptr2!= nullptr) {
            intNum.num->pushTail((ptr1->data * ptr2->data + quotient) % 10);
            quotient = (ptr1->data * ptr2->data + quotient) / 10;
            ptr2 = ptr2->next;
        }
        //Adds 0 to the head of intNum according to the # of stage
        for(int i = 0;i<stage;i++){
            intNum.num->pushHead(0);
        }
        stage++;
        if(quotient!=0){
            intNum.num->pushTail(quotient);
        }
        quotient = 0;                               //resets the quotient
        ret = intNum + ret;                         //adds up intermediate big integer and result (by now)
        ptr1 = ptr1->next;
        ptr2 = this->num->head;
    }
    return ret;
}
/**
 * Multiplies this BigInteger and an integer
 * @param i the integer
 * @return the multiplication of an integer and big integer
 */
BigInteger BigInteger::operator*(int i){
    BigInteger operand = BigInteger(i);             //turns the integer into big integer
    return BigInteger::operator*(operand);
}
/**
 * Copy constructor
 * Constructs a BigInteger the same as a given BigInteger
 * @param other given BigInteger
 * @return BigInteger object
 */
BigInteger::BigInteger(const BigInteger &other){
    num = new LinkedList;                   //initializes num
    Node* temp = other.num->head;
    while(temp!= nullptr){
        this->num->pushTail(temp->data);
        temp = temp->next;
    }
}
/**
 * Assigns the BigInteger in parameter(list) to this BigInteger
 * @param list the BigInteger to be assigned
 * @return this BigInteger, which has same data values as list
 */
BigInteger& BigInteger::operator=(const BigInteger &list){
    if(num) delete num;
    num = new LinkedList;
    this->num->head = 0;
    this->num->tail = 0;
    this->num->length = 0;
    Node *temp = list.num->head;
    this->num->pushHead(temp->data);
    temp = temp->next;
    while (temp != NULL) {
        this->num->pushTail(temp->data);
        temp = temp->next;
    }
    return *this;
}
/**
 * Destructor
 * Deletes num
 */
BigInteger::~BigInteger(){
    if(num) delete num;
}