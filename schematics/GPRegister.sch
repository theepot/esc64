v 20121123 2
C 3400 4300 1 0 0 74377-1.sym
{
T 3700 7750 5 10 0 0 0 0 1
device=74377
T 5100 7600 5 10 1 1 0 6 1
refdes=U1
T 3700 7950 5 10 0 0 0 0 1
footprint=DIP20
}
C 3400 600 1 0 0 74377-1.sym
{
T 3700 4050 5 10 0 0 0 0 1
device=74377
T 5100 3900 5 10 1 1 0 6 1
refdes=U2
T 3700 4250 5 10 0 0 0 0 1
footprint=DIP20
}
C 5800 4500 1 0 0 74541-1.sym
{
T 6100 7650 5 10 0 0 0 0 1
device=74541
T 7500 7500 5 10 1 1 0 6 1
refdes=U3
T 6100 7850 5 10 0 0 0 0 1
footprint=DIP20
}
C 5800 800 1 0 0 74541-1.sym
{
T 6100 3950 5 10 0 0 0 0 1
device=74541
T 7500 3800 5 10 1 1 0 6 1
refdes=U4
T 6100 4150 5 10 0 0 0 0 1
footprint=DIP20
}
C 7100 4300 1 0 0 gnd-1.sym
C 7100 600 1 0 0 gnd-1.sym
N 5800 7200 5400 7200 4
{
T 6200 7200 5 10 0 1 0 0 1
netname=ledBus:1
}
N 5800 6900 5400 6900 4
{
T 6200 6900 5 10 0 1 0 0 1
netname=ledBus:2
}
N 5800 6600 5400 6600 4
{
T 6200 6600 5 10 0 1 0 0 1
netname=ledBus:3
}
N 5800 6300 5400 6300 4
{
T 6200 6300 5 10 0 1 0 0 1
netname=ledBus:4
}
N 5800 6000 5400 6000 4
{
T 6200 6000 5 10 0 1 0 0 1
netname=ledBus:5
}
N 5800 5700 5400 5700 4
{
T 6200 5700 5 10 0 1 0 0 1
netname=ledBus:6
}
N 5800 5400 5400 5400 4
{
T 6200 5400 5 10 0 1 0 0 1
netname=ledBus:7
}
N 5800 5100 5400 5100 4
{
T 6200 5100 5 10 0 1 0 0 1
netname=ledBus:8
}
N 5800 3500 5400 3500 4
{
T 6200 3500 5 10 0 1 0 0 1
netname=ledBus:9
}
N 5800 3200 5400 3200 4
{
T 6200 3200 5 10 0 1 0 0 1
netname=ledBus:10
}
N 5800 2900 5400 2900 4
{
T 6200 2900 5 10 0 1 0 0 1
netname=ledBus:11
}
N 5800 2600 5400 2600 4
{
T 6200 2600 5 10 0 1 0 0 1
netname=ledBus:12
}
N 5800 2300 5400 2300 4
{
T 6200 2300 5 10 0 1 0 0 1
netname=ledBus:13
}
N 5800 2000 5400 2000 4
{
T 6200 2000 5 10 0 1 0 0 1
netname=ledBus:14
}
N 5800 1700 5400 1700 4
{
T 6200 1700 5 10 0 1 0 0 1
netname=ledBus:15
}
N 5800 1400 5400 1400 4
{
T 6200 1400 5 10 0 1 0 0 1
netname=ledBus:16
}
N 6400 4500 5700 4500 4
N 5700 500 5700 4500 4
N 5700 800 6400 800 4
N 2000 800 3400 800 4
N 2100 800 2100 4500 4
N 2100 4500 3400 4500 4
N 2000 500 5700 500 4
N 2000 1100 3400 1100 4
N 3400 4800 2300 4800 4
N 2300 4800 2300 1100 4
C 1400 1000 1 0 0 in-1.sym
{
T 1400 1300 5 10 0 0 0 0 1
device=INPUT
T 1000 1100 5 10 1 1 0 0 1
refdes=load
}
C 1400 700 1 0 0 in-1.sym
{
T 1400 1000 5 10 0 0 0 0 1
device=INPUT
T 900 800 5 10 1 1 0 0 1
refdes=clock
}
C 1400 400 1 0 0 in-1.sym
{
T 1400 700 5 10 0 0 0 0 1
device=INPUT
T 1000 500 5 10 1 1 0 0 1
refdes=OE
}
N 7200 4600 7200 4500 4
N 7200 900 7200 800 4
T 37400 48000 8 10 0 0 0 0 1
netname=Q?
T 37400 47700 8 10 0 0 0 0 1
netname=Q?
T 37400 47400 8 10 0 0 0 0 1
netname=Q?
T 37400 47100 8 10 0 0 0 0 1
netname=Q?
T 37400 46800 8 10 0 0 0 0 1
netname=Q?
T 37400 46500 8 10 0 0 0 0 1
netname=Q?
T 37400 46200 8 10 0 0 0 0 1
netname=Q?
T 37400 45900 8 10 0 0 0 0 1
netname=Q?
N 7800 3500 7900 3500 4
C 7900 3400 1 0 0 out-1.sym
{
T 8600 3500 5 10 1 1 0 0 1
refdes=Q9
T 7900 3700 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 3200 7900 3200 4
C 7900 3100 1 0 0 out-1.sym
{
T 8600 3200 5 10 1 1 0 0 1
refdes=Q10
T 7900 3400 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 2900 7900 2900 4
C 7900 2800 1 0 0 out-1.sym
{
T 8600 2900 5 10 1 1 0 0 1
refdes=Q11
T 7900 3100 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 2600 7900 2600 4
C 7900 2500 1 0 0 out-1.sym
{
T 8600 2600 5 10 1 1 0 0 1
refdes=Q12
T 7900 2800 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 2300 7900 2300 4
C 7900 2200 1 0 0 out-1.sym
{
T 8600 2300 5 10 1 1 0 0 1
refdes=Q13
T 7900 2500 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 2000 7900 2000 4
C 7900 1900 1 0 0 out-1.sym
{
T 8600 2000 5 10 1 1 0 0 1
refdes=Q14
T 7900 2200 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 1700 7900 1700 4
C 7900 1600 1 0 0 out-1.sym
{
T 8600 1700 5 10 1 1 0 0 1
refdes=Q15
T 7900 1900 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 1400 7900 1400 4
C 7900 1300 1 0 0 out-1.sym
{
T 8600 1400 5 10 1 1 0 0 1
refdes=Q16
T 7900 1600 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 7200 7900 7200 4
C 7900 7100 1 0 0 out-1.sym
{
T 8600 7200 5 10 1 1 0 0 1
refdes=Q1
T 7900 7400 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 6900 7900 6900 4
C 7900 6800 1 0 0 out-1.sym
{
T 8600 6900 5 10 1 1 0 0 1
refdes=Q2
T 7900 7100 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 6600 7900 6600 4
C 7900 6500 1 0 0 out-1.sym
{
T 8600 6600 5 10 1 1 0 0 1
refdes=Q3
T 7900 6800 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 6300 7900 6300 4
C 7900 6200 1 0 0 out-1.sym
{
T 8600 6300 5 10 1 1 0 0 1
refdes=Q4
T 7900 6500 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 6000 7900 6000 4
C 7900 5900 1 0 0 out-1.sym
{
T 8600 6000 5 10 1 1 0 0 1
refdes=Q5
T 7900 6200 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 5700 7900 5700 4
C 7900 5600 1 0 0 out-1.sym
{
T 8600 5700 5 10 1 1 0 0 1
refdes=Q6
T 7900 5900 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 5400 7900 5400 4
C 7900 5300 1 0 0 out-1.sym
{
T 8600 5400 5 10 1 1 0 0 1
refdes=Q7
T 7900 5600 5 10 0 0 0 0 1
device=OUTPUT
}
N 7800 5100 7900 5100 4
C 7900 5000 1 0 0 out-1.sym
{
T 8600 5100 5 10 1 1 0 0 1
refdes=Q8
T 7900 5300 5 10 0 0 0 0 1
device=OUTPUT
}
C 2800 7100 1 0 0 in-1.sym
{
T 2800 7400 5 10 0 0 0 0 1
device=INPUT
T 2400 7200 5 10 1 1 0 0 1
refdes=D1
}
C 2800 6800 1 0 0 in-1.sym
{
T 2800 7100 5 10 0 0 0 0 1
device=INPUT
T 2400 6900 5 10 1 1 0 0 1
refdes=D2
}
C 2800 6500 1 0 0 in-1.sym
{
T 2800 6800 5 10 0 0 0 0 1
device=INPUT
T 2400 6600 5 10 1 1 0 0 1
refdes=D3
}
C 2800 6200 1 0 0 in-1.sym
{
T 2800 6500 5 10 0 0 0 0 1
device=INPUT
T 2400 6300 5 10 1 1 0 0 1
refdes=D4
}
C 2800 5900 1 0 0 in-1.sym
{
T 2800 6200 5 10 0 0 0 0 1
device=INPUT
T 2400 6000 5 10 1 1 0 0 1
refdes=D5
}
C 2800 5600 1 0 0 in-1.sym
{
T 2800 5900 5 10 0 0 0 0 1
device=INPUT
T 2400 5700 5 10 1 1 0 0 1
refdes=D6
}
C 2800 5300 1 0 0 in-1.sym
{
T 2800 5600 5 10 0 0 0 0 1
device=INPUT
T 2400 5400 5 10 1 1 0 0 1
refdes=D7
}
C 2800 5000 1 0 0 in-1.sym
{
T 2800 5300 5 10 0 0 0 0 1
device=INPUT
T 2400 5100 5 10 1 1 0 0 1
refdes=D8
}
C 2800 3400 1 0 0 in-1.sym
{
T 2800 3700 5 10 0 0 0 0 1
device=INPUT
T 2400 3500 5 10 1 1 0 0 1
refdes=D9
}
C 2800 3100 1 0 0 in-1.sym
{
T 2800 3400 5 10 0 0 0 0 1
device=INPUT
T 2400 3200 5 10 1 1 0 0 1
refdes=D10
}
C 2800 2800 1 0 0 in-1.sym
{
T 2800 3100 5 10 0 0 0 0 1
device=INPUT
T 2400 2900 5 10 1 1 0 0 1
refdes=D11
}
C 2800 2500 1 0 0 in-1.sym
{
T 2800 2800 5 10 0 0 0 0 1
device=INPUT
T 2400 2600 5 10 1 1 0 0 1
refdes=D12
}
C 2800 2200 1 0 0 in-1.sym
{
T 2800 2500 5 10 0 0 0 0 1
device=INPUT
T 2400 2300 5 10 1 1 0 0 1
refdes=D13
}
C 2800 1900 1 0 0 in-1.sym
{
T 2800 2200 5 10 0 0 0 0 1
device=INPUT
T 2400 2000 5 10 1 1 0 0 1
refdes=D14
}
C 2800 1600 1 0 0 in-1.sym
{
T 2800 1900 5 10 0 0 0 0 1
device=INPUT
T 2400 1700 5 10 1 1 0 0 1
refdes=D15
}
C 2800 1300 1 0 0 in-1.sym
{
T 2800 1600 5 10 0 0 0 0 1
device=INPUT
T 2400 1400 5 10 1 1 0 0 1
refdes=D16
}
L 1000 1300 1400 1300 3 0 0 0 -1 -1
L 1000 700 1300 700 3 0 0 0 -1 -1
T 3900 8200 5 10 0 1 0 0 1
netname=ledBus:?
