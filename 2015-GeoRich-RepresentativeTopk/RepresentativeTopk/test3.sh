d=3

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

# ===== np =====
for np in 20000 30000 40000 50000
do
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
done

# ===== nw =====
for nw in 20000 30000 40000 50000
do
	np=10000
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
done

# ===== t =====
for t in 5 15 20 30
do
	np=10000
	nw=10000
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
done

# ===== k =====
for k in 5 20 30 50
do
	np=10000
	nw=10000
	t=10
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
done

# ===== dweight =====
for a in 0.2 0.8
do
	np=10000
	nw=10000
	t=10
	k=10
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
done

# ===== default =====
	np=100000
	nw=100000
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

# ===== np =====
for np in 200000 500000 1000000
do
	nw=100000
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
done

# ===== nw =====
for nw in 200000 500000 1000000
do
	np=100000
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
done
