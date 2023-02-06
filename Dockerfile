# The recipe below implements a Docker multi-stage build:
# <https://docs.docker.com/develop/develop-images/multistage-build/>

###############################################################################
## First stage: an image to build the planner.
## 
## We'll install here all packages we need to build the planner
###############################################################################
FROM ubuntu:18.04 AS builder

RUN apt-get update && \
    apt-get install -y locales jq vim wget curl gawk \
    cmake g++ g++-multilib make python python-dev python-pip

# Install basic dev tools
RUN pip install --upgrade pip
# RUN pip install h5py keras numpy pillow scipy tensorflow-cpu subprocess32

# Set up environment variables
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8 \ 
	CXX=g++ \
	HOME=/app \
	BASE_DIR=/app/planners \
    INT_BUILD_COMMIT_ID=1e53834 \
    DIV_SC_BUILD_COMMIT_ID=af16888

WORKDIR $BASE_DIR/integrated/

# Fetch the code at the right commit ID from the Github repo
RUN curl -L https://github.com/IBM/forbiditerative/archive/${INT_BUILD_COMMIT_ID}.tar.gz | tar xz --strip=1 \
# Invoke the build script with appropriate options
    && python ./build.py \
# Strip the main binary to reduce size
    && strip --strip-all builds/release/bin/downward

#################################
# Download and Install diverse score computation
#################################
WORKDIR $BASE_DIR/diversescore/
# Fetch the code at the right commit ID from the Github repo
RUN curl -L https://github.com/IBM/diversescore/archive/${DIV_SC_BUILD_COMMIT_ID}.tar.gz | tar xz --strip=1 \
# Invoke the build script with appropriate options
    && python ./build.py \
# Strip the main binary to reduce size
    && strip --strip-all builds/release/bin/downward

###############################################################################
## Second stage: the image to run the planner
## 
## This is the image that will be distributed, we will simply copy here
## the files that we fetched and compiled in the previous image and that 
## are strictly necessary to run the planner
###############################################################################
FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y locales curl gawk \
    # cmake g++ g++-multilib make \
    python python-dev python-pip \
    && rm -rf /var/lib/apt/lists/*

# Set up environment variables
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8 \ 
	CXX=g++ \
	HOME=/app \
	BASE_DIR=/app/planners \
    DIVERSE_SCORE_COMPUTATION_PATH=/app/planners/diversescore

# Create required directories
RUN mkdir -p $HOME && mkdir -p $BASE_DIR
WORKDIR $BASE_DIR

## Copying integrated planner essential files
WORKDIR ${BASE_DIR}/integrated/
COPY --from=builder ${BASE_DIR}/integrated/fast-downward.py ${BASE_DIR}/integrated/plan-cerberus-sat.py ${BASE_DIR}/integrated/plan-cerberus-agl.py ./
COPY --from=builder ${BASE_DIR}/integrated/builds/release/bin/ ./builds/release/bin/
COPY --from=builder ${BASE_DIR}/integrated/driver ./driver
COPY --from=builder ${BASE_DIR}/integrated/fast-downward.py ${BASE_DIR}/integrated/copy_plans.py ${BASE_DIR}/integrated/plan.py ${BASE_DIR}/integrated/planner_call.py ${BASE_DIR}/integrated/timers.py  ${BASE_DIR}/integrated/plan_*.sh  ./
COPY --from=builder ${BASE_DIR}/integrated/iterative ./iterative

## Copying diverse score computation essential files
WORKDIR ${BASE_DIR}/diversescore/
COPY --from=builder ${BASE_DIR}/diversescore/fast-downward.py .
COPY --from=builder ${BASE_DIR}/diversescore/builds/release/bin/ ./builds/release/bin/
COPY --from=builder ${BASE_DIR}/diversescore/driver ./driver

WORKDIR $BASE_DIR

COPY examples ./

CMD /bin/bash
