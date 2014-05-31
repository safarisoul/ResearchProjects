prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# sac
################################################################################

network=northAmerica
fun=sac

for nf in 20000 40000 80000 160000 320000 640000
do
  $prefix ./RoadNetworkRkNN netw $network nfac $nf func $fun
done

for no in 250000 375000 500000 625000 750000
do
  $prefix ./RoadNetworkRkNN netw $network nobj $no func $fun
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN netw $network k $k func $fun
done

#for sp in 60 70 80 90 100 110 120
#do
#  $prefix ./RoadNetworkRkNN netw $network sped $sp$ func $fun
#done

#for mp in 20 40 60 80 100
#do
#  $prefix ./RoadNetworkRkNN netw $network movp $mp func $fun
#done

for nq in 1 10 100 1000
do
  $prefix ./RoadNetworkRkNN netw $network nque $nq func $fun
done