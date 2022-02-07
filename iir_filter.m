HSE = 25e6 / 3; % HSE from stlink
pll2m = 4;
pll2n = 280;
pll2p = 8;
F_spi = HSE / pll2m * pll2n / pll2p / 2;
Ts = 16 / F_spi;
fc = 75e3;
% fc = 1.1e6;
n = 3;
[zb,pb,kb] = butter(n,2*pi*fc,'s');
[bb,ab] = zp2tf(zb,pb,kb);
Fs = 1 / Ts;
Ws = Fs * 2 * pi;
nyquist = (Ws / 2);
wb = logspace(2, log10(nyquist), 4000);
hb = freqs(bb,ab,wb);

figure
semilogx(wb/(2*pi),mag2db(abs(hb)))
hold on
axis([10 1.1e6 -80 5])
grid
xlabel('Frequency (Hz)')
ylabel('Attenuation (dB)')
title('Butterworth Filter (n=3)')

Gp = tf(bb, ab);
figure;
bode(Gp);

Gcz = c2d(Gp, Ts, 'tustin');
Gcz.variable='z^-1';
cn = Gcz.Numerator{1};
cd = Gcz.Denominator{1};
format long
if n == 5
fprintf(strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
"+ (%.10e) * aRxBuffer[j-4] + (%.10e) * aRxBuffer[j-5] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
"- (%.10e) * yi[j-3] - (%.10e) * yi[j-4] \\\n", ...
"- (%.10e) * yi[j-5];\n\n"), cn(1), cn(2), cn(3), cn(4), cn(5), cn(6), ...
    cd(2), cd(3), cd(4), cd(5), cd(6));
end

if n == 4
fprintf(strcat("#define CALC_YI yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
"+ (%.10e) * aRxBuffer[j-4] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
"- (%.10e) * yi[j-3] - (%.10e) * yi[j-4];\n\n"), cn(1), cn(2), cn(3), cn(4), cn(5), ...
    cd(2), cd(3), cd(4), cd(5));
end

if n == 3
fprintf(strcat("#define CALC_YI yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
"- (%.10e) * yi[j-3];\n\n"), cn(1), cn(2), cn(3), cn(4), ...
    cd(2), cd(3), cd(4));
end

if n == 2
fprintf(strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2];\n\n"), cn(1), cn(2), cn(3), ...
    cd(2), cd(3));
end

if n == 1
fprintf(strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"- (%.10e) * yi[j-1];\n\n"), cn(1), cn(2), ...
    cd(2));
end