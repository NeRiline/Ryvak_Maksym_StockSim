#!/bin/bash
#SBATCH --job-name=stocksim-phase5
#SBATCH --partition=short-40core-shared
#SBATCH --cpus-per-task=1
#SBATCH --mem=2G
#SBATCH --time=00:15:00
# Split work across array tasks (one task per dip value)
# Each task will handle a block of dip values
#SBATCH --array=0-6
#SBATCH --output=logs/phase5/sweep_phase5_%A_%a.out
#SBATCH --error=logs/phase5/sweep_phase5_%A_%a.err

set -euo pipefail

# Allow local runs without SLURM
SLURM_ARRAY_TASK_ID=${SLURM_ARRAY_TASK_ID:-0}

if [[ ! -x ./runner ]]; then
  echo "Error: ./runner is missing or not executable. Build it first (exclude main.cpp when compiling)."
  exit 1
fi

# Sweep ranges (recommended)
# Dip: 10..40 step 5
DIP_START=10
DIP_END=40
DIP_STEP=5

# Rally: 1..8 step 1
RALLY_START=1
RALLY_END=8
RALLY_STEP=1

# Multiplier: 1.0..6.0 step 0.5
MULT_START=1.0
MULT_END=6.0
MULT_STEP=0.5

# Lookbacks to test
LOOKBACKS=(252 378)

# other fixed params
MONTHLY=500
STARTYEAR=2000
ENDYEAR=2020

# number of array tasks used (0..N-1)
NUM_TASKS=7


# build dip list and split across array tasks
dips=()
for d in $(seq $DIP_START $DIP_STEP $DIP_END); do
  dips+=("$d")
done

POINTS=${#dips[@]}
blockSize=$(( (POINTS + NUM_TASKS - 1) / NUM_TASKS ))
startIdx=$(( SLURM_ARRAY_TASK_ID * blockSize ))
endIdx=$(( startIdx + blockSize - 1 ))
if [ $endIdx -ge $((POINTS - 1)) ]; then endIdx=$((POINTS - 1)); fi

echo "Task $SLURM_ARRAY_TASK_ID: handling dip indices $startIdx..$endIdx (total dips=$POINTS)"

if [ $startIdx -gt $endIdx ]; then
  echo "Task $SLURM_ARRAY_TASK_ID: no work for this task id."
  exit 0
fi

mkdir -p results logs/phase5


OUTFILE=results/phase5_results_${SLURM_ARRAY_TASK_ID}.csv
echo "dip,rally,mult,lookback,finalValue,totalInvested,totalReturn,cagr,maxDrawdown,totalTrades" > "$OUTFILE"

for idx in $(seq $startIdx $endIdx); do
  dip=${dips[$idx]}
  echo "  Running dip=$dip"

  # loop rallies
  for rally in $(seq $RALLY_START $RALLY_STEP $RALLY_END); do
    # loop multipliers (floating)
    mult=$(awk -v s="$MULT_START" 'BEGIN{printf "%.2f", s}')
    while (( $(awk -v m="$mult" -v e="$MULT_END" 'BEGIN{print (m<=e)}') )); do
      for lookback in "${LOOKBACKS[@]}"; do
        echo "    rally=$rally mult=$mult lookback=$lookback"
        # runner prints: dip,rally,mult,finalValue,... -> insert lookback as 4th field
        out=$(./runner "$dip" "$rally" "$mult" "$lookback" "$MONTHLY" "$STARTYEAR" "$ENDYEAR")
        echo "$out" | awk -F',' -v lb="$lookback" 'BEGIN{OFS=","} {print $1,$2,$3,lb,$4,$5,$6,$7,$8,$9}' >> "$OUTFILE"
      done
      mult=$(awk -v m="$mult" -v s="$MULT_STEP" 'BEGIN{printf "%.2f", m + s}')
    done
  done
done

echo "Task $SLURM_ARRAY_TASK_ID complete. Results: $OUTFILE"
