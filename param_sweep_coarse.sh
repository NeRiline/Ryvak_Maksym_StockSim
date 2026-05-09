#!/bin/bash
#SBATCH --job-name=stocksim-phase4
#SBATCH --partition=short-40core-shared
#SBATCH --cpus-per-task=1
#SBATCH --mem=2G
#SBATCH --time=00:10:00
#SBATCH --array=0-20
#SBATCH --output=logs/sweep_phase4_%A_%a.out
#SBATCH --error=logs/sweep_phase4_%A_%a.err

set -euo pipefail

if [[ ! -x ./stocksim ]]; then
	echo "Error: ./stocksim is missing or not executable. Build it on a compute node first."
	exit 1
fi

# Map array task ID (0-20) to dip percentage (26.0-28.0 in 0.1 steps)
dip=$(awk -v id="$SLURM_ARRAY_TASK_ID" 'BEGIN { printf "%.1f", 26.0 + (id * 0.1) }')

echo "Task $SLURM_ARRAY_TASK_ID: Testing dip=$dip% with rally=0.0 and mult=2.0-3.0"

# Run menu option 16 (grid search)
# Each task tests one dip value; rally is fixed and mult sweeps the next Phase 4 range
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
1
0
0.0
0.1
2.0
3.0
0.05
5
0
EOF

echo "Task $SLURM_ARRAY_TASK_ID complete"