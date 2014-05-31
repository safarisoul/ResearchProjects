prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# updatable influence zone
################################################################################

fun=u

for nf in 2000 4000 8000 16000 32000 64000
do
  $prefix ./RoadNetworkRkNN nfac $nf func $fun
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN k $k func $fun
done

fun=us

for nf in 2000 4000 8000 16000 32000 64000
do
  $prefix ./RoadNetworkRkNN nfac $nf func $fun
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN k $k func $fun
done

################################################################################
# influence zone
################################################################################

fun=m

#for nf in 2000 4000 8000 16000 32000 64000
#do
#  $prefix ./RoadNetworkRkNN nfac $nf func $fun
#done

#for k in 1 2 4 8 16 32 64
#do
#  $prefix ./RoadNetworkRkNN k $k func $fun
#done

fun=mu
nupd=100

for nf in 2000 4000 8000 16000 32000 64000
do
  $prefix ./RoadNetworkRkNN nfac $nf func $fun nupd $nupd
done

for k in 1 2 4 8 16 32 64
do
  $prefix ./RoadNetworkRkNN k $k func $fun nupd $nupd
done