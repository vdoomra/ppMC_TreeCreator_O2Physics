# PPMc to Berkeley Tree Format Conversion

## Overview

This repository provides tools to convert ppMC (proton-proton Monte Carlo) datasets from the ALICE Grid into Berkeley Tree format. The conversion pipeline involves downloading ppMC files, processing them through the O2Physics EECTreeCreator task, and converting the output to ROOT trees.

## Prerequisites

### 1. O2Physics Installation

You must have O2Physics installed before proceeding. If you haven't already done so, follow the instructions in `InstallingO2.md`.

### 2. ppMC File List

You'll need a list of ppMC files with their locations on the ALICE Grid. The repository includes an example file list for LHC26b6 data:

- **File:** `filelist_LHC26b6.txt` (located in the `macros/` folder)
- **Data Type:** ppMC anchored to OO

---

## Workflow Overview
---

## Step-by-Step Instructions

### Step 1: Generate ALICE Grid Token

To access files on the ALICE Grid, you must generate an authentication token. Run this command once:

```bash
shifter --image=tch285/o2alma:latest /bin/bash -c 'alienv setenv O2Physics/latest-master-o2 -c /bin/bash -c "alien-token-init"'
```

**Token Validity:** The token remains valid for one month.

### Step 2: Move Token to Shared Storage

⚠️ **Critical:** By default, the token is stored in `/tmp/` on the login node, but compute nodes cannot access this directory. You must move the token to a shared location accessible from all nodes.

```bash
# Create a shared token directory
mkdir -p ~/.alien
chmod 700 ~/.alien

# Move token files
cp /tmp/tokencert_*.pem ~/.alien/
cp /tmp/tokenkey_*.pem ~/.alien/

# Secure permissions
chmod 600 ~/.alien/*
```

### Step 3: Run the Job Script

Submit the conversion job using the main script:

```bash
./job_script.sh
```

This script will:
1. Download ppMC files using the token
2. Process each file with `EECTreeCreator.cxx` (via `script_bash.sh`)
3. Apply the JSON configuration files:
   - `OutputDirector.json`
   - `dal-config-base.json`
4. Pass output to `Convert2ROOTTree.C` for final conversion

### Step 4: Retrieve Output

Once complete, your final ROOT trees will be ready for analysis.

---

## Configuration Files

The conversion process uses the following JSON configuration files (located in `macros/`):

- **OutputDirector.json** — Configures output handling
- **dal-config-base.json** — Base DAL (Data Abstraction Layer) configuration

Ensure these are properly configured before running `job_script.sh`.

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Token not found by compute nodes | Verify you've moved tokens to `~/.alien/` and check permissions with `chmod 600` |
| Job fails to find ppMC files | Confirm the filelist exists and all paths are correct |
| Conversion errors | Check that O2Physics is properly installed and sourced in your environment |

---
