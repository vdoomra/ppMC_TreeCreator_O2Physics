# Installing O2/O2Physics on NERSC

## Prerequisites: Obtain Your CERN Grid Certificate

### What is a CERN Grid Certificate?

The ALICE Grid is a distributed network of computers at CERN and partner institutes worldwide, used by physicists to run large-scale data analysis jobs. To authenticate with this system, you need a digital certificate—essentially a password-protected file that serves as your credential passport.

### Getting Your Certificate

1. Download your certificate (.p12 file) from the CERN portal:
   https://alice-doc.github.io/alice-analysis-tutorial/start/cert.html
   
   **Default PEM Phrase:** `ENTER PASSWORD HERE`

2. Convert your certificate into two usable files:

   The command-line tools used for job submission cannot read the .p12 format directly, so you'll need to split it into:
   
   - **usercert.pem** — Your public certificate (your digital ID card)
   - **userkey.pem** — Your private key (your secret signature; protected with `chmod 0400`)
   
   These two files work together: the certificate identifies you, and the key proves you own that certificate.
   Please follow the detailed instructions on how to do so from: https://alice-doc.github.io/alice-analysis-tutorial/start/cert.html

---

## Installation Steps

### 1. Connect to Perlmutter

SSH into the NERSC Perlmutter system. Make sure the `.globus` folder from the previous step is located in your `$HOME` directory.

### 2. Prepare Your Workspace

Create a working directory for the build:

```bash
mkdir -p ~/work/O2Physics
cd ~/work/O2Physics
```

### 3. Use a Terminal Multiplexer

Start a `tmux` session to run the installation in the background. This allows you to detach and reattach as needed during the lengthy build process:

```bash
tmux new-session -s o2build
```

### 4. Enter the Shifter Container

A pre-configured Shifter container contains all necessary build dependencies:

```bash
shifter --image=tch285/o2alma:latest /bin/bash
```

**Important:** If you have local installations (e.g., ROOT) configured in your `.bashrc`, they may interfere with the build. Configure your shell profile to load these environment variables only when running outside Shifter.

### 5. Initialize the Build Environment

Run the following commands inside the container:

```bash
export ALIBUILD_WORK_DIR="/global/cfs/cdirs/alice/vdoomra/O2Physics/sw"
eval "$(alienv shell-helper)"
export GIT_SSL_CAINFO=/etc/ssl/certs/ca-bundle.crt
aliBuild init O2Physics@master
```

### 6. Clone the O2Physics Repository

We recommend forking the official repository first, then cloning your fork:

1. Fork: https://github.com/AliceO2Group/O2Physics
2. Clone your fork:
```bash
   git clone https://github.com/<your-username>/O2Physics.git
```

### 7. Build O2Physics

Run the build with the following command:

```bash
time aliBuild build O2Physics -d -j4
```

**Note:** Using 4 cores works well and provides a good balance. Increasing the core count may exhaust available memory on the node.

### 8. Verify Successful Installation

Once the build completes, you should see confirmation output indicating a successful installation.

---

## Troubleshooting

- **Memory issues during build:** Reduce the `-j` flag (number of parallel jobs) if you run out of memory
- **Environment variable conflicts:** Verify local installations are not loaded inside Shifter
- **Certificate issues:** Double-check that `.globus` folder is in your home directory
