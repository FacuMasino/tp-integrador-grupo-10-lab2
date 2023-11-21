#include <iostream>
#include "Income.h"

using namespace std;

 Income:: Income() :Transaction() {
    _saleId = -1;
}

 Income:: Income (int saleId,  int idTransaction,Date transDate,Time transTime,std::string description,float amount,std::string paymentMethod)
    : Transaction(idTransaction,transDate,transTime, description, amount,paymentMethod) {
    _saleId=saleId;
}
void Income::setSaleId( int saleId){_saleId=saleId;}


int   Income::getSaleId(){return _saleId;}

void Income::toVecString(std::string vStr[7]) {
    vStr[0] = std::to_string(Transaction::getIdTransaction());
    vStr[1] = std::to_string(_saleId);
    vStr[2] = Transaction::getDateTrans().toString();
    vStr[3] = Transaction::getTimeTrans().toString();
    vStr[4] = Transaction::getDescription();
    vStr[5] = Transaction::getAmount();
    vStr[6] = Transaction::getPaymentMehod();


}

/*
void Income::load (){



Transaction::load();

int saleId;
std::string method;

cout<<"Ingrese ID del ingreso";
cin>>saleId;
//TODO Ver si se manda por set o esto venia automatico de algun lado

cout<<"Ingrese Metodo de pago recibido:";
cin>>method;
setpaymentMethod(method);

}*/