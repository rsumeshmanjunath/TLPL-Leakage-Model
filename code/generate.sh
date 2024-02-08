#!/bin/bash

#check the number of parameters passed
if [ "$#" -ne 4 ]; then
	echo "Usage:"
	echo "./generate.sh <Working DIR> < Start Key Value> <End Key Value> <Maximum Trace Count> "
	exit
fi

MAIN_DIR=$PWD
#create a working directory 
WORK_DIR=$1

#check if WORK_DIR already exists
#if [ -d "$WORK_DIR" ]; then
#	echo "Directory already exists. Please try a different name or remove the existing directory and try again"
#	exit
#fi

#create a WORK_DIR and enter the same
mkdir $WORK_DIR
cd $WORK_DIR
WORKDIR=$PWD


#generate the list of jobs to execute

for (( i=$2; i<=$3; i++ ))
do
	for (( j=100; j<=$4; j+=10))
	do
		echo "echo \"$i,$j\" ; $MAIN_DIR/main $i $j" >> $WORKDIR/jobs.txt
	done
done 


#create a bash script for merging/concatenation and runscript
cat << EOF >> $WORKDIR/submit.sh

#!/bin/bash

module load gcc

#Submit the list of jobs as parallel job array
slurm_parallel_ja_submit.sh -N 200 -t 24:00:00 jobs.txt 

EOF



cat << EOF >> $WORKDIR/output.sh
#!/bin/bash

if [ "\$#" -ne 4 ]; then
        echo "Usage:"
        echo "./output.sh <path-to-the-output-directory>< Start Key Value> <End Key Value> <Maximum Trace Count> "
        exit
fi

CURRENT_DIR=$PWD

mkdir output
cd output

mkdir cpa_keys
mkdir random_input

cd $CURRENT_DIR

cat jobs.txt-*.out > jobs.out
cat jobs.txt-*.err > jobs.err

rm jobs.txt-*

for i in {${2}..${3}}; do
        for j in {100..${4}..10}; do
                cat $1/cpa_keys/cpa_keys_${i}_${j}.txt >> $CURRENT_DIR/output/cpa_keys_$i.txt
                cat $1/random_input/random_input_${i}_${j}.txt >> $CURRENT_DIR/output/random_input_$i.txt
        done
done

EOF

chmod 755 $WORKDIR/submit.sh $WORKDIR/output.sh
