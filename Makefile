build:
	gcc -O3 serial_fft.c -o serial_fft -lfftw3 -lfftw3f -lm
	gcc -O3 serial_plot.c -o serial_plot -lfftw3 -lm
	gcc -O3 serial_test.c -o serial_test -lfftw3 -lm