prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# generate objects
################################################################################

fun=geno

for no in 50000 75000 100000 125000 150000
do
  $prefix ./RoadNetworkRkNN nobj $no func $fun
done

for mp in 20 40 60 80 100
do
  $prefix ./RoadNetworkRkNN movp $mp func $fun
done

for sp in 60 70 80 90 100 110 120
do
  $prefix ./RoadNetworkRkNN sped $sp func $fun
done
