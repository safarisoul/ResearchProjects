d=6

# ===== default =====
	nw=200000
	t=10
	k=10
	a=0.5
	pf="house_0_10000_147043";
	pfile="../DataPrepare/data/${pf}.txt"
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

# ===== nw =====
for nw in 100000 500000
do
	t=10
	k=10
	a=0.5
	pf="house_0_10000_147043";
	pfile="../DataPrepare/data/${pf}.txt"
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

# ===== t =====
for t in 5 15 20 30
do
	nw=200000
	k=10
	a=0.5
	pf="house_0_10000_147043";
	pfile="../DataPrepare/data/${pf}.txt"
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

# ===== k =====
for k in 5 20 30 50
do
	nw=200000
	t=10
	a=0.5
	pf="house_0_10000_147043";
	pfile="../DataPrepare/data/${pf}.txt"
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

# ===== dweight =====
for a in 0.2 0.8
do
	nw=200000
	t=10
	k=10
	pf="house_0_10000_147043";
	pfile="../DataPrepare/data/${pf}.txt"
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
