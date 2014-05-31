prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# generate query
################################################################################

fun=genq

for nq in 1 10 100 1000
do
  $prefix ./RoadNetworkRkNN nque $nq func $fun
done

for nf in 2000 4000 8000 16000 32000 64000
do
  $prefix ./RoadNetworkRkNN nfac $nf func $fun
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN k $k func $fun
done