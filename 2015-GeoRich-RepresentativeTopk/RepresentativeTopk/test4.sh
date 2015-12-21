d=4

# ===== default =====
	np=10000
	nw=10000
	t=10
	k=10
	a=0.5
	for pd in "anticorrelated" "uni"
	do
		pf="point_${pd}_${np}"
		pfile="../DataPrepare/data/${pf}_${d}.txt"
		wf="preference_clustered_${nw}"
		wfile="../DataPrepare/data/${wf}_${d}.txt"
		for pro in "rtk_mh" #"tk_mh" "mul" "mul_mh"
		do
			commend="./RepTopk${d} pro ${pro} pfile ${pfile} wfile ${wfile} t ${t} k ${k} dweight ${a}"
			of="${d}_${pf}_${wf}_${t}_${k}_${a}_${pro}"
			ofile="result/${of}.txt"
			echo $commend
			$commend > $ofile
		done
	done