#!/usr/bin/env bash
# Sync this project to the HPC cluster, keeping only selected benchmarks:
# s27, s298, and s1423.
#
# Run this script from your local x86 machine, not from inside the cluster:
#   bash sync_to_hpc_s27_s298.sh
#
# Local repo path expected by this script:
#   /home/tsiantosd/Desktop/TsiantosD/HERMES/workspace/ECE484-Radhard-Circuit-Design
# Remote destination created by this script:
#   /home/compmech/dtsiantos/ECE484-Radhard-Circuit-Design

set -euo pipefail

LOCAL_REPO="/home/tsiantosd/Desktop/TsiantosD/HERMES/workspace/ECE484-Radhard-Circuit-Design"
REMOTE_USER="compmech"
REMOTE_HOST="147.102.81.129"
REMOTE_BASE="/home/compmech/dtsiantos"
REMOTE_REPO="${REMOTE_BASE}/ECE484-Radhard-Circuit-Design"

# Password auth helper.
# WARNING: this stores the HPC password in plaintext in this script.
# Prefer SSH keys when possible. Keep this file private: chmod 700 sync_to_hpc_s27_s298.sh
HPC_PASSWORD='hpc2026!'

if command -v sshpass >/dev/null 2>&1; then
  export SSHPASS="${HPC_PASSWORD}"
  SSH_CMD=(sshpass -e ssh)
  RSYNC_RSH="sshpass -e ssh"
else
  echo "WARNING: sshpass not found. You will be asked for the HPC password interactively." >&2
  echo "Install it locally if you want non-interactive password auth, e.g.: sudo apt install sshpass" >&2
  SSH_CMD=(ssh)
  RSYNC_RSH="ssh"
fi

if [[ ! -d "${LOCAL_REPO}" ]]; then
  echo "ERROR: local repo not found: ${LOCAL_REPO}" >&2
  exit 1
fi

if ! command -v rsync >/dev/null 2>&1; then
  echo "ERROR: rsync is required on the local machine." >&2
  exit 1
fi

echo "Creating remote directory: ${REMOTE_REPO}"
"${SSH_CMD[@]}" "${REMOTE_USER}@${REMOTE_HOST}" "mkdir -p '${REMOTE_REPO}'"

echo "Syncing repo to ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_REPO}"
# NOTE: --delete-excluded makes the remote copy contain only the selected subset.
# Use REMOTE_REPO as a dedicated project directory; do not point it at a directory
# containing unrelated files.
rsync -avz --delete --delete-excluded \
  -e "${RSYNC_RSH}" \
  --exclude='/.git/' \
  --exclude='/timing_logs/' \
  --exclude='/src/main' \
  --exclude='/src/*.o' \
  --exclude='/src/*.d' \
  --exclude='/src/*.so' \
  --exclude='/src/*.a' \
  --include='/tests/' \
  --include='/tests/cells.v' \
  --include='/tests/s27/***' \
  --include='/tests/s298/***' \
  --include='/tests/s1423/***' \
  --exclude='/tests/*' \
  --exclude='/outputs/' \
  "${LOCAL_REPO}/" \
  "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_REPO}/"

cat <<EOF

Done.

On the cluster you can continue with:
  ssh ${REMOTE_USER}@${REMOTE_HOST}
  cd ${REMOTE_REPO}

Example runs, depending on what is available on the cluster:
  ./run.sh --backend legacy s27
  ./run.sh --backend flat s27
  ./run.sh --backend legacy s298
  ./run.sh --backend flat s298
  ./run.sh --backend legacy s1423
  ./run.sh --backend flat s1423

If CUDA/nvcc is available and the Makefile CUDA path is configured:
  ./run.sh --backend cuda s27
  ./run.sh --backend cuda s298
  ./run.sh --backend cuda s1423
EOF
