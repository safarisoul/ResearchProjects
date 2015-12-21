################################################################################
# color (normMin, normMax, total) 68040 raw tuples 9 dimension
# house (normMin, normMax, total) 147043 raw tuples 6 dimension
# uniform preference (total, dim)
# clustered preference (total, dim)
# correlated points (maxValue, total, dim)
# anti-correlated points (maxValue, total, dim)
# clustered points (maxValue, total, dim)
# uniform points (maxValue, total, dim)
################################################################################

./DataPrepare color 0 10000 100000
./DataPrepare house 0 10000 200000

d=3

for n in 10000 20000 30000 40000 50000 100000 200000 500000 1000000
do
	#./DataPrepare preferenceuni $n $d
	./DataPrepare preferenceclustered $n $d
	#./DataPrepare pointcorrelated 10000 $n $d
	./DataPrepare pointanticorrelated 10000 $n $d
	#./DataPrepare pointclustered 10000 $n $d
	./DataPrepare pointuni 10000 $n $d
done

for d in 4 5
do
	for n in 10000
	do
		#./DataPrepare preferenceuni $n $d
		./DataPrepare preferenceclustered $n $d
		#./DataPrepare pointcorrelated 10000 $n $d
		./DataPrepare pointanticorrelated 10000 $n $d
		#./DataPrepare pointclustered 10000 $n $d
		./DataPrepare pointuni 10000 $n $d
	done
done

for d in 6
do
	for n in 100000 200000 500000
	do
		#./DataPrepare preferenceuni $n $d
		./DataPrepare preferenceclustered $n $d
	done
done
