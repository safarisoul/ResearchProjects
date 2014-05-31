prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# generate query
################################################################################

network=northAmerica
fun=genq

for nq in 1 10 100 1000
do
  $prefix ./RoadNetworkRkNN netw $network nque $nq func $fun
done

for nf in 20000 40000 80000 160000 320000 640000
do
  $prefix ./RoadNetworkRkNN netw $network nfac $nf func $fun
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN netw $network k $k func $fun
done