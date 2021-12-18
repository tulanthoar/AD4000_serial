#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>

int main()
{
    HANDLE hSerial;
    hSerial = CreateFile(
        "COM4",
        GENERIC_READ,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            printf("serial port does not exist. Inform user.\n");
        }
        printf("some other error occurred. Inform user.\n");
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        printf("error getting state\n");
    }
    dcbSerialParams.BaudRate = 12000000;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = EVENPARITY;
    dcbSerialParams.fParity = TRUE;
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        printf("error setting serial port state\n");
    }
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    FILE *f = fopen("C:/Users/natha/outf", "wb");
    if (f == NULL)
    {
        printf("unable to open outf\n");
        exit(1);
    }
    fflush(f);
    unsigned short rbuf[2048] = {0};
    unsigned short w = 0;
    unsigned long numBytesRead;
    unsigned long numBytesWritten;
    while (1)
    {
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        numBytesWritten = fwrite(rbuf, 1, sizeof(rbuf), f);

        if (numBytesRead != sizeof(rbuf))
        {
            printf("number bytes read = %d", numBytesRead);
            exit(1);
        }
        for (int i = 0; i < sizeof(rbuf) / 2; ++i)
        {
            //  printf("%hu ", rbuf[i]);
            if (rbuf[i] != w)
            {
                printf("w = %hu, rbuf = %hu", w, rbuf[i]);
                exit(1);
            }
            ++w;
            if (w == 16384)
                w = 0;
        }
        printf("%hu", w);

        fflush(f);
        if (numBytesWritten != sizeof(rbuf))
        {
            printf("number bytes written = %d", numBytesWritten);
            exit(1);
        }
    }
}