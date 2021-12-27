F_spi = 25e6 / 3 / 4 * 360 / 8;
Ts = 16 / F_spi;
fc = 50e3;
n = 1;
[zb,pb,kb] = butter(n,2*pi*fc,'s');
[bb,ab] = zp2tf(zb,pb,kb);
[hb,wb] = freqs(bb,ab,4096);

figure
plot(wb/(2*pi),mag2db(abs(hb)))
hold on
axis([0 300e3 -40 5])
grid
xlabel('Frequency (Hz)')
ylabel('Attenuation (dB)')
legend('butter')

Gp = tf(bb, ab);
figure;
bode(Gp);

Gcz = c2d(Gp, Ts, 'tustin');
Gcz.variable='z^-1';
cn = Gcz.Numerator{1};
cd = Gcz.Denominator{1};