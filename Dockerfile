# The recipe below implements a Docker multi-stage build:
# <https://docs.docker.com/develop/develop-images/multistage-build/>

###############################################################################
## First stage: an image to build the planner.
## 
## We'll install here all packages we need to build the planner
###############################################################################
FROM ubuntu:18.04 AS builder

RUN apt-get update && apt-get install --no-install-recommends -y \
    cmake           \
    ca-certificates \
    curl            \
    g++             \
    make            \
    python3
    

WORKDIR /workspace/forbiditerative/

# Set up some environment variables.
ENV CXX g++
ENV BUILD_COMMIT_ID 1751d5e
# Fetch the code at the right commit ID from the Github repo
RUN curl -L https://github.com/IBM/forbiditerative/archive/${BUILD_COMMIT_ID}.tar.gz | tar xz --strip=1

# Invoke the build script with appropriate options
RUN python3 ./build.py -j4 release64

# Strip the main binary to reduce size
RUN strip --strip-all builds/release64/bin/downward

#################################
# Download and Install ForbidIterative
#################################
WORKDIR /workspace/diversescore/

ENV BUILD_COMMIT_ID 8208ae3

# Fetch the code at the right commit ID from the Github repo
RUN curl -L https://github.com/IBM/diversescore/archive/${BUILD_COMMIT_ID}.tar.gz | tar xz --strip=1

# Invoke the build script with appropriate options
RUN python3 ./build.py -j4 

# Strip the main binary to reduce size
RUN strip --strip-all builds/release/bin/downward

###############################################################################
## Second stage: the image to run the planner
## 
## This is the image that will be distributed, we will simply copy here
## the files that we fetched and compiled in the previous image and that 
## are strictly necessary to run the planner
###############################################################################
FROM ctpelok77/cerberus:latest

# Install any package needed to *run* the planner
RUN apt-get update && apt-get install --no-install-recommends -y \
    python3  \
    && rm -rf /var/lib/apt/lists/*

# Copy the relevant files from the previous docker build into this build.

## Copying ForbidIterative planner essential files
WORKDIR /workspace/diversescore/
COPY --from=builder /workspace/diversescore/fast-downward.py .
COPY --from=builder /workspace/diversescore/builds/release/bin/ ./builds/release/bin/
COPY --from=builder /workspace/diversescore/driver ./driver
WORKDIR /workspace/forbiditerative/
COPY --from=builder /workspace/forbiditerative/fast-downward.py .
COPY --from=builder /workspace/forbiditerative/builds/release64/bin/ ./builds/release64/bin/
COPY --from=builder /workspace/forbiditerative/driver ./driver
COPY --from=builder /workspace/forbiditerative/copy_plans.py .
COPY --from=builder /workspace/forbiditerative/plan.py .
COPY --from=builder /workspace/forbiditerative/planner_call.py .
COPY --from=builder /workspace/forbiditerative/timers.py .
COPY --from=builder /workspace/forbiditerative/iterative/ ./iterative/
COPY --from=builder /workspace/forbiditerative/plan_topk.sh .
COPY --from=builder /workspace/forbiditerative/plan_topq.sh .
COPY --from=builder /workspace/forbiditerative/plan_topk_q.sh .
COPY --from=builder /workspace/forbiditerative/plan_diverse_agl.sh .
COPY --from=builder /workspace/forbiditerative/plan_diverse_sat.sh .
COPY --from=builder /workspace/forbiditerative/run.py .
# COPY --from=builder /workspace/forbiditerative/plan_diverse_bounded.sh .

ENV DIVERSE_FAST_DOWNWARD_PLANNER_PATH=/workspace/cerberus
ENV DIVERSE_SCORE_COMPUTATION_PATH=/workspace/diversescore

RUN ln -s /usr/bin/python3 /usr/bin/python

WORKDIR /work
CMD /bin/bash