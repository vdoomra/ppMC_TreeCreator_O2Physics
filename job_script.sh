#!/bin/bash
#SBATCH --qos=shared
#SBATCH --constraint=cpu
#SBATCH --account=alice
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=3:00:00
#SBATCH --array=0-1625
#SBATCH --mem=24G
#SBATCH --output=/dev/null
#SBATCH --error=/dev/null

BASE_DIR="/global/cfs/cdirs/alice/$USER/O2Physics"
FILELIST="${BASE_DIR}/filelist_LHC26b6.txt"
OUTPUT_DIR="${BASE_DIR}/output"

INDEX=${SLURM_ARRAY_TASK_ID}

LINE_NUM=$((INDEX + 1))

MC_FILE=$(sed -n "${LINE_NUM}p" "$FILELIST")

if [[ -z "$MC_FILE" ]]; then
    echo "ERROR: No file found at line ${LINE_NUM}"
    exit 1
fi

RUN_NUMBER=$(echo "$MC_FILE" | awk -F'/' '{print $7}')
OUT_FILE="AO2D_${RUN_NUMBER}_${INDEX}.root"
CONVERTED_OUT="BerkeleyTree_${RUN_NUMBER}_${INDEX}.root"

WORK_DIR="$(pwd)/o2_task_${INDEX}"

mkdir -p "$WORK_DIR"
chmod 700 "$WORK_DIR"

# Copy ALIEN token
cp ~/.alien/tokencert_*.pem "$WORK_DIR/tokencert.pem"
cp ~/.alien/tokenkey_*.pem  "$WORK_DIR/tokenkey.pem"

chmod 600 "$WORK_DIR"/token*.pem

# Create run script outside Shifter
cat > "$WORK_DIR/run_analysis.sh" <<EOF
#!/bin/bash

export XRD_STREAMS=4
export XRD_PARALLELTCP=4
export XRD_CONNECTIONWINDOW=10
export XRD_REQUESTTIMEOUT=600

export TMPDIR="$WORK_DIR"
export XDG_CACHE_HOME="$WORK_DIR/.cache"

mkdir -p "\$XDG_CACHE_HOME"

export JALIEN_TOKEN_CERT="$WORK_DIR/tokencert.pem"
export JALIEN_TOKEN_KEY="$WORK_DIR/tokenkey.pem"

cd "$WORK_DIR"

echo "======================================"
echo "Task: $INDEX"
echo "Input: $MC_FILE"
echo "Output: $OUT_FILE"
echo "======================================"

echo "Downloading input file"

alien_cp "alien://$MC_FILE" "file:$OUT_FILE"

if [[ $? -ne 0 ]]; then
    echo "ERROR: alien_cp failed"
    exit 1
fi

cp "$BASE_DIR/dpl-config-base.json" .
cp "$BASE_DIR/OutputDirector.json" .
cp "$BASE_DIR/script_base.sh" .
cp "$BASE_DIR/Convert2ROOTTree.C" .

chmod +x script_base.sh

echo "===== Starting analysis ====="

./script_base.sh "$OUT_FILE" dpl-config-base.json

echo "===== Analysis finished ====="

EEC_FILE=\$(ls EECTree_*.root 2>/dev/null | head -n 1)
root -l -b -q "Convert2ROOTTree.C(\"\$EEC_FILE\", \"$CONVERTED_OUT\")"

echo "Moving output"
mv "$CONVERTED_OUT" "$OUTPUT_DIR/"
cd ../
rm -rf "$WORK_DIR"
EOF

chmod +x "$WORK_DIR/run_analysis.sh"

echo "Starting Shifter"

shifter --image=tch285/o2alma:latest /bin/bash -c "
    echo 'Starting O2 environment'
    alienv --no-refresh setenv O2Physics/latest-master-o2 -c $WORK_DIR/run_analysis.sh
"
