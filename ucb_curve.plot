#!/usr/local/bin/gnuplot --persist 

set multiplot layout 2,2
set grid

N = 1000000;
C = log(N);

_  = " "
a1 = "0.4"
a2 = "0.5" 
b1 = "0.5"
b2 = "0.6" 
c1 = "0.4"
c2 = "0.6" 
d1 = "0.6"
d2 = "0.5" 
e1 = "0.5"
e2 = "0.5" 

a(x) = a1 * (C/x) ** a2
b(x) = b1 * (C/x) ** b2
c(x) = c1 * (C/x) ** c2
d(x) = d1 * (C/x) ** d2
e(x) = e1 * (C/x) ** e2

set xrange [0:100]
set yrange [0.00:0.50]
plot a(x) t a1._.a2, \
     b(x) t b1._.b2, \
     c(x) t c1._.c2, \
     d(x) t d1._.d2, \
     e(x) t e1._.e2

set xrange [100:1000]
set yrange [0.00:0.10]
plot a(x) t a1._.a2, \
     b(x) t b1._.b2, \
     c(x) t c1._.c2, \
     d(x) t d1._.d2, \
     e(x) t e1._.e2

set xrange [1000:10000]
set yrange [0.000:0.03]
plot a(x) t a1._.a2, \
     b(x) t b1._.b2, \
     c(x) t c1._.c2, \
     d(x) t d1._.d2, \
     e(x) t e1._.e2

set xrange [10000:100000]
set yrange [0.000:0.01]
plot a(x) t a1._.a2, \
     b(x) t b1._.b2, \
     c(x) t c1._.c2, \
     d(x) t d1._.d2, \
     e(x) t e1._.e2

unset multiplot
