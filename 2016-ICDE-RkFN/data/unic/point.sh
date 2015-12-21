dfile="point.txt"
echo $dfile
epsfile="point.eps"
cmd="
reset
#set term postscript eps enhanced color
set term postscript eps enhanced monochrome 28
set output \"${epsfile}\"

set xrange[0:120000]
set yrange[0:120000]

plot '$dfile'


"

echo "$cmd" | gnuplot

