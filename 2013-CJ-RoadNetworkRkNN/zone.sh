prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# influence zone
################################################################################

fun=m

for nf in 2000 4000 8000 16000 32000 64000
do
  $prefix ./RoadNetworkRkNN nfac $nf func $fun
done

for no in 50000 75000 100000 125000 150000
do
  $prefix ./RoadNetworkRkNN nobj $no func $fun
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN k $k func $fun
done

for sp in 60 70 80 90 100 110 120
do
  $prefix ./RoadNetworkRkNN sped $sp$ func $fun
done

for mp in 20 40 60 80 100
do
  $prefix ./RoadNetworkRkNN movp $mp func $fun
done

for nq in 1 10 100 1000
do
  $prefix ./RoadNetworkRkNN nque $nq func $fun
done