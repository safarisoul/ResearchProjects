prefix="" #"valgrind --leak-check=full" #"" #"valgrind --tool=memcheck"

################################################################################
# real
################################################################################

#./DCCR data real gen y

for n in 50000 150000 200000
do
  $prefix ./DCCR data real pick y chfc y dccr y nf $n nc $n
done

for n in 1 5 10 20 30 50
do
  $prefix ./DCCR data real pick y chfc y dccr y k $n
done

################################################################################
# uni
################################################################################

#./DCCR data uni gen y

for n in 200000 300000 500000 700000 1000000
do
  $prefix ./DCCR data uni pick y chfc y dccr y nf $n nc $n
done

for n in 1 5 10 20 30 50
do
  $prefix ./DCCR data uni pick y chfc y dccr y k $n
done

################################################################################
# uni-rst
################################################################################

for n in 200000 300000 500000 700000 1000000
do
  $prefix ./DCCR data uni pick y rstq y chfc y dccr y nf $n nc $n
done

for n in 1 5 10 20 30 50
do
  $prefix ./DCCR data uni pick y rstq y chfc y dccr y k $n
done

################################################################################
# uni-tpru pruning power & verifing power
################################################################################

#for n in 200000 300000 500000 700000 1000000
#do
#  $prefix ./DCCR data uni rstq y tpru y nf $n nc $n
#done

#for n in 1 5 10 20 30 50
#do
#  $prefix ./DCCR data uni rstq y tpru y k $n
#done

################################################################################
# unic Theoretical analysis - area outside of DC_k
################################################################################

#./DCCR data unic gen y

#for n in 200000 300000 500000 700000 1000000
#do
#  $prefix ./DCCR data unic pick y vdca y nf $n nc $n nq 1
#done

#for n in 1 5 10 20 30 50
#do
#  $prefix ./DCCR data unic pick y vdca y k $n nq 1
#done

#for n in 1000 2000 3000 6000 10000 20000 30000 40000 50000 60000 70000 80000 90000 100000
#do
#  $prefix ./DCCR data unic pick y vdca y nf $n nc $n nq 1
#done

#for n in 1 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000
#do
#  $prefix ./DCCR data unic pick y vdca y k $n nq 1
#done

################################################################################
# unic-rst Theoretical analysis - area in candidate range
################################################################################

#for n in 200000 300000 500000 700000 1000000
#do
#  $prefix ./DCCR data unic pick y rstq y vara y nf $n nc $n
#done

#for n in 1 5 10 20 30 50
#do
#  $prefix ./DCCR data unic pick y rstq y vara y k $n
#done

#for n in 1000 2000 3000 6000 10000 20000 30000 40000 50000 60000 70000 80000 90000 100000
#do
#  $prefix ./DCCR data unic pick y rstq y vara y nf $n nc $n
#done

#for n in 1 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000
#do
#  $prefix ./DCCR data unic pick y rstq y vara y k $n
#done


