Bootstrap: docker
From: ctpelok77/forbiditerative:latest

%setup
    # Just for diagnosis purposes
    hostname -f > $SINGULARITY_ROOTFS/etc/build_host
%runscript
    # This will be called whenever the Singularity container is invoked
    python3 /workspace/forbiditerative/run.py $@

%post

## 
%labels
Name        Forbid-Iterative
Description Forbid-Iterative (FI) Planner is an Automated PDDL based planner that includes planners for top-k, top-quality, and diverse computational tasks.
Authors     Michael Katz <michael.katz1@ibm.com>