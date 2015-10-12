#include <QCoreApplication>
#include <QDebug>

#include "cyclicbuffer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int s;
    CyclicBuffer * buffer = new CyclicBuffer(16, s);

    if(s==CyclicBuffer::BUFFER_OK)
        qDebug() << "Buffer allocation succeeded with size 16.";

    qDebug() << "Borders of buffer are set to:" << buffer->GetBottomIndex() << "and" << buffer->GetTopIndex();

    qDebug() << "Pushing characters A, B, C, D into buffer.";
    buffer->Push('A');
    buffer->Push('B');
    buffer->Push('C');
    buffer->Push('D');

    qDebug() << "Index of writing pointer is:" << buffer->GetPushIndex() << "and index of read pointer is:" << buffer->GetPopIndex();

    qDebug() << "Reading the firstly pushed character:" << (char)buffer->Pop();
    qDebug() << "Reading index is now:" << buffer->GetPopIndex();

    qDebug() << "Starting 5 cycles of Pop() calls:\n";
    unsigned char ch;
    for(int i=0; i<5; i++)
    {
        if((ch = buffer->Pop())!=NULL)
            qDebug() << "Cycle" << i << ":" << (char)ch;
        else
            qDebug() << "Cycle" << i << ":" << "Nothing to read.";
    }

    qDebug() << "\nSetting element at index 1 to value X.";
    if(buffer->SetValueAt(1, 'X')==CyclicBuffer::BUFFER_OK)
        qDebug() << "Value at index 1 was set to:" << (char)buffer->GetValueAt(1);
    else
        qDebug() << "Value could not be read.";

    qDebug() << "Forcing index back to index 1.";
    if(buffer->SetPopIndex(1)==CyclicBuffer::BUFFER_OK)
        qDebug() << "Index was successfuly set back to" << buffer->GetPopIndex();

    qDebug() << "\nPushing \'Hi world\' string.";
    const char str[] = "Hi world";

    for(int j=0; j<(sizeof(str)/sizeof(char)); j++)
    {
        buffer->Push((unsigned char)str[j]);
    }

    qDebug() << "Now reading string from buffer.";
    while((ch = buffer->Pop())!=NULL)
    {
        qDebug() << ":" << (char)ch;
    }

    qDebug() << "Current writing index position:" << buffer->GetPushIndex();

    const char str2[] = "Hi buffer";
    qDebug() <<"\nAppending next string: \'Hi buffer\'";

    for(int j=0; j<(sizeof(str2)/sizeof(char)); j++)
    {
        buffer->Push((unsigned char)str2[j]);
    }

    qDebug() << "Now reading string from buffer.";
    while((ch = buffer->Pop())!=NULL)
    {
        qDebug() << ":" << (char)ch;
    }

    qDebug() << "Current writing index position:" << buffer->GetPushIndex();
    qDebug() << "Current reading index position:" << buffer->GetPopIndex();

    qDebug() << "Moving top border to index 10.";
    buffer->SetTopIndex(10);
    qDebug() << "Now buffer size is:" << buffer->GetBufferSize() << "while total memory size is:" << buffer->GetTotalBufferSize();
    qDebug() << "Reallocating memory size to 32.";
    if(buffer->ReallocBuffer(32)==CyclicBuffer::BUFFER_OK)
    {
        qDebug() << "Successfuly reallocated. Memory size is:" << buffer->GetTotalBufferSize() << "while buffer size is:" << buffer->GetBufferSize();
    }
    else
        qDebug() << "Rallocation failed.";

    qDebug() << "\nResetting buffer...";
    buffer->ResetBuffer();
    qDebug() << "Read pointer:" << buffer->GetPopIndex() << "Write pointer:" << buffer->GetPushIndex() << "Bottom border:" << buffer->GetBottomIndex() << "Top border:" << buffer->GetTopIndex();

    qDebug() << "/nSetting bottom border to index 5.";
    buffer->SetBottomIndex(5);
    qDebug() << "Write and read pointers are now:" << buffer->GetPushIndex() << "and" << buffer->GetPopIndex();
    qDebug() << "Buffer size is:" << buffer->GetBufferSize() << "while total memory size is:" << buffer->GetTotalBufferSize();


    delete buffer;

    return a.exec();
}
