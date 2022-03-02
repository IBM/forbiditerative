Bootstrap: docker
From: ctpelok77/cerberus:latest

%setup
    # Just for diagnosis purposes
    hostname -f > $SINGULARITY_ROOTFS/etc/build_host
%runscript
    # This will be called whenever the Singularity container is invoked
    python3 /workspace/cerberus/fast-downward.py $@

%post

## Cerberus planner
%labels
Name        Cerberus
Description Red-black planning heuristic with native support for conditional effects, h^2 mutexes, novelty heuristic for search guidance. Post-IPC 2018 version.
Authors     Michael Katz <michael.katz1@ibm.com>