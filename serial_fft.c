#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>
#include <fftw3.h>
#include <math.h>
#include <complex.h>

int main(int argc, char *argv[])
{
    // handle for the serial interface
    HANDLE hSerial;
    hSerial = CreateFile(
        argv[1], // command line argument is COM port
        GENERIC_READ, // read only
        0,
        0,
        OPEN_EXISTING, // open existing file
        FILE_ATTRIBUTE_NORMAL,
        0);
    // check if the file opening was successful
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            printf("serial port does not exist. Inform user.\n");
        }
        printf("some other error occurred. Inform user.\n");
    }

    // struct to define the serial parameters
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    // get the current state
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        printf("error getting state\n");
    }
    // set the baud rate
    dcbSerialParams.BaudRate = 11978688;
    // 8 bits to a byte
    dcbSerialParams.ByteSize = 8;
    // one stop bit
    dcbSerialParams.StopBits = ONESTOPBIT;
    // no parity bit
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fParity = TRUE;
    // set the communication parameters
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        printf("error setting serial port state\n");
    }
    // clear the current content of the serial device
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // open the output file
    FILE *f = fopen("C:/Users/natha/fftf", "wb");
    if (f == NULL)
    {
        printf("unable to open outf\n");
        exit(1);
    }
    // flush any data to the file
    fflush(f);
    // complex fourier coefficients
    fftw_complex *out;
    // the inputs are real values
    double *in;
    // fft plan to calculate the transform
    fftw_plan p;
    // number of points
    unsigned int N = 4096;
    // initialize the input to the fourier transform algorithm
    in = (double*) fftw_malloc(sizeof(double) * N);
    // initialize the output of the fourier transform
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    // the magnitude of the fourier coefficients to be plotted
    double abs_out[2049];
    // create the fftw plan, discrete forward transform real to complex 1D
    p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
    // buffer for the recieved data
    unsigned short rbuf[4096] = {0};
    // number of bytes read from serial device
    unsigned long numBytesRead;
    // number of bytes written to the output file 
    unsigned long numBytesWritten;
    while (1)
    {
        // read data from serial device
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        // convert the read values from 16 bit integers to doubles
        for(int i = 0; i < N; ++i) in[i] = rbuf[i] * 1.0;
        // execute the fourier transform
        fftw_execute(p);
        // convert the complex fourier coefficients to magnitudes, subtracting 133 for 0 dB to correspond to full scale
        for(int i = 0; i < N/2 + 1; ++i){
            abs_out[i] = 20 * log10(sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1])) - 133.0;
        }
        // seek to the start of the file to overwrite our prior data
        fseek(f, 0, SEEK_SET);
        // write the values to the file
        numBytesWritten = fwrite(abs_out, 1, sizeof(abs_out), f);
        // flush the data to the output file
        fflush(f);
    }
    // dealocate the memory we reserved
    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
}