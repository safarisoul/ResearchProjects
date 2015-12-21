d=3

np=10000
nw=10000
pf="point_anticorrelated_${np}_${d}"
pfile="../DataPrepare/data/${pf}.txt"
wf="preference_clustered_${nw}_${d}"
wfile="../DataPrepare/data/${wf}.txt"

for pro in "rtk" "rtk_mh" "tk" "tk_mh" "mul" "mul_mh"
do
	commend="./RepTopk pro ${pro} pfile ${pfile} wfile ${wfile}"
	ofile="result/${pf}_${wf}_${pro}.txt"
	echo $commend
	$commend > $ofile
done