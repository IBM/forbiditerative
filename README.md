# Forbid-Iterative (FI) Planner is an Automated PDDL based planner that includes planners for top-k, top-quality, and diverse computational tasks.

## The codebase consists of five planners, for five computational problems:

1. Top-k planning
2. Top-quality planning
3. Unordered top-quality planning
4. Satisficing/Agile diverse planning
5. Bounded diversity diverse planning

## The planners are based on the idea of obtaining multiple solutions by iteratively reformulating planning tasks to restrict the set of valid plans, forbidding previously found ones. Thus, the planners can be referred to as FI-top-k, FI-top-quality, FI-unordered-top-quality, FI-diverse-{agl,sat,bD}.

The example invocation code can be found (for the corresponding computational problem) in
1. plan_topk.sh or plan_topk_via_unordered_topq.sh
2. plan_topq_via_topk.sh or plan_topq_via_unordered_topq.sh
3. plan_unordered_topq.sh
4. plan_diverse_{agl,sat}.sh
5. plan_diverse_bounded.sh

# Building
For building the code please use
```
./build.py
```

# Running
## FI-top-k
```
# ./plan_topk.sh <domain> <problem> <number-of-plans>
./plan_topk.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 1000
```
## FI-top-quality
```
# ./plan_topq_via_topk.sh <domain> <problem> <quality-multiplier>
./plan_topq_via_topk.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 1.1
```
## FI-unordered-top-quality
```
# ./plan_unordered_topq.sh <domain> <problem> <quality-multiplier>
./plan_unordered_topq.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 1.1
```
## FI-diverse-agl
```
# ./plan_diverse_agl.sh <domain> <problem> <number-of-plans>
./plan_diverse_agl.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10
```
## FI-diverse-sat
```
## See the dependencies below (1 and 2)
# ./plan_diverse_sat.sh <domain> <problem> <number-of-plans> <diversity-metric> <larger-number-of-plans>
./plan_diverse_sat.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 stability 20
```
## FI-diverse-bD
```
## See the dependencies below (1, 2, and 3)
# ./plan_diverse_bounded.sh <domain> <problem> <number-of-plans> <diversity-metric> <bound> <larger-number-of-plans>
./plan_diverse_bounded.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 stability 0.25 20
```


# Dependencies
For some of the diverse planners, the dependencies are as follows:
1. A Fast Downward (recent version) based planner for computing a single plan should be installed and a path to that planner should be specified in an environment variable **DIVERSE_FAST_DOWNWARD_PLANNER_PATH**. We suggest using the [*Cerberus* planner, post-IPC2018 version](https://github.com/ctpelok77/fd-red-black-postipc2018)
2. Computation of a subset of plans is performed in a post-processing, path to the code should be specified in an environment variable **DIVERSE_SCORE_COMPUTATION_PATH**. The code can be found [here](https://github.com/IBM/diversescore).
3. Note that for the diversity-bounded diverse planning the computation in a post-processing requires enabling CPLEX support in Fast Downward (see http://www.fast-downward.org/) and building the post-processing code with LP support.

## Licensing

Forbid-Iterative (FI) Planner is an Automated PDDL based planner that
includes planners for top-k, top-quality, and diverse computational
tasks. Copyright (C) 2019  Michael Katz, IBM Research, USA.
The code extends the Fast Downward planning system. The license for the
extension is specified in the LICENSE file.

## Fast Downward
Fast Downward is a domain-independent planning system.

For documentation and contact information see http://www.fast-downward.org/.

The following directories are not part of Fast Downward as covered by this
license:

* ./src/search/ext

For the rest, the following license applies:

```
Fast Downward is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

Fast Downward is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
```
