#ifndef QT_TEMPLATES_H
#define QT_TEMPLATES_H

#include "Qt_Header.h"


//template to copy data from vector to QbyteArray
template <class T>
bool copyVectortoByteArray(QByteArray &byteArray, T &vectorArray)
{
    for(int i = 0; i < vectorArray.size(); i++)
    {
        byteArray.push_back((uint8_t)vectorArray[i]);
    }
    return true;
}

//template to copy data from QbyteArray to vector
template <class T>
bool copyByteArraytoVector(T &vectorArray, QByteArray &byteArray)
{
    for(int i = 0; i < byteArray.size(); i++)
    {
        vectorArray.push_back((uint8_t)byteArray[i]);
    }
    return true;
}

#endif // QT_TEMPLATES_H
