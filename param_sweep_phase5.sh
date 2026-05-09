#!/bin/bash
#SBATCH --job-name=stocksim-phase5
#SBATCH --partition=short-40core-shared
#SBATCH --cpus-per-task=1
#SBATCH --mem=2G
#SBATCH --time=00:15:00
# Split work across fewer array tasks to avoid submit limits
# Each task will handle a block of dip values
#SBATCH --array=0-19
#SBATCH --output=logs/phase5/sweep_phase5_%A_%a.out
#SBATCH --error=logs/phase5/sweep_phase5_%A_%a.err

set -euo pipefail

if [[ ! -x ./stocksim ]]; then
  echo "Error: ./stocksim is missing or not executable. Build it on a compute node first."
  exit 1
fi

# Total dip points and grid parameters
BASE=26.50       # starting dip (inclusive)
POINTS=101       # 26.50..27.50 inclusive in 0.01 steps -> 101 points
STEP=0.01

# Number of array tasks used (0..19)
NUM_TASKS=20

blockSize=$(awk -v p="$POINTS" -v n="$NUM_TASKS" 'BEGIN { printf "%d", int((p + n - 1) / n) }')
startIdx=$(( SLURM_ARRAY_TASK_ID * blockSize ))
endIdx=$(( startIdx + blockSize - 1 ))
if [ $endIdx -ge $((POINTS - 1)) ]; then endIdx=$((POINTS - 1)); fi

echo "Task $SLURM_ARRAY_TASK_ID: handling dip indices $startIdx..$endIdx (base $BASE step $STEP)"

for idx in $(seq $startIdx $endIdx); do
  dip=$(awk -v b="$BASE" -v i="$idx" -v s="$STEP" 'BEGIN { printf "%.2f", b + i * s }')
  echo "  Running dip=$dip"

  ./stocksim << EOF
Name
111
1
4
16
500
2000
2020
${dip}
${dip}
0.01
0
0.0
0.1
2.95
3.05
0.01
10
0
EOF

done

echo "Task $SLURM_ARRAY_TASK_ID complete"
