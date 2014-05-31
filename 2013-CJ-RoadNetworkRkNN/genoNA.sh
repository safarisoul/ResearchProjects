prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# generate objects
################################################################################

network=northAmerica
fun=geno

for no in 250000 375000 500000 625000 750000
do
  $prefix ./RoadNetworkRkNN netw $network nobj $no func $fun
done

#for mp in 20 40 60 80 100
#do
#  $prefix ./RoadNetworkRkNN netw $network movp $mp func $fun
#done

#for sp in 60 70 80 90 100 110 120
#do
#  $prefix ./RoadNetworkRkNN netw $network sped $sp func $fun
#done
