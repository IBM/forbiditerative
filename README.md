# Forbid-Iterative (FI) Planner is an Automated PDDL based planner that includes planners for top-k, top-quality, and diverse computational tasks.

## The codebase consists of multiple planners, for multiple computational problems, roughly divided into three categories:

1. Top-k planning
2. Top-quality planning  
    2.1. Top-quality planning  
    2.2. Unordered top-quality planning  
    2.3. Sub(multi)set top-quality planning  
3. Diverse planning  
    3.1. Satisficing/Agile diverse planning  
    3.2. Bounded diversity diverse planning  
    3.3. Bounded quality diverse planning  
    3.4. Bounded quality and diversity diverse planning  
    3.5. Bounded quality optimal diversity diverse planning  

## The planners are based on the idea of obtaining multiple solutions by iteratively reformulating planning tasks to restrict the set of valid plans, forbidding previously found ones. Thus, the planners can be referred to as FI-top-k, FI-top-quality, FI-unordered-top-quality, FI-diverse-{agl,sat,bD,bQ,bQbD, bQoptD}.

The example invocation code can be found (for the corresponding computational problem) in  
1. plan_topk.sh or plan_topk_via_unordered_topq.sh  
2.   
    2.1. plan_topq_via_topk.sh or plan_topq_via_unordered_topq.sh  
    2.2. plan_unordered_topq.sh  
    2.3. plan_{subset,submultiset}_topq.sh  
3.   
    3.1. plan_diverse_{agl,sat}.sh  
3.2. plan_diverse_bounded.sh  
3.3. plan_quality_bounded_diverse_sat.sh  
3.4. plan_quality_bounded_diversity_bounded_diverse.sh  
3.5. plan_quality_bounded_diversity_optimal_diverse.sh  

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
## See the dependencies below (1)
# ./plan_diverse_sat.sh <domain> <problem> <number-of-plans> <diversity-metric> <larger-number-of-plans>
./plan_diverse_sat.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 stability 20
```
## FI-diverse-bD
``` 
## See the dependencies below (1 and 2)
# ./plan_diverse_bounded.sh <domain> <problem> <number-of-plans> <diversity-metric> <bound> <larger-number-of-plans>
./plan_diverse_bounded.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 stability 0.25 20
```
## FI-diverse-bQ
``` 
## See the dependencies below (1)
# ./plan_quality_bounded_diverse_sat.sh <domain> <problem> <number-of-plans> <quality-bound>  <diversity-metric> 
./plan_quality_bounded_diverse_sat.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 1.1 stability 
```
## FI-diverse-bQbD
``` 
## See the dependencies below (1 and 2)
# ./plan_quality_bounded_diversity_bounded_diverse.sh <domain> <problem> <number-of-plans> <quality-bound> <diversity-bound> <diversity-metric> 
./plan_quality_bounded_diversity_bounded_diverse.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 1.1 0.1 stability 
```
## FI-diverse-bQoptD
``` 
## See the dependencies below (1 and 2)
# ./plan_quality_bounded_diversity_optimal_diverse.sh <domain> <problem> <number-of-plans> <quality-bound> <diversity-metric>  
./plan_quality_bounded_diversity_optimal_diverse.sh examples/logistics00/domain.pddl examples/logistics00/probLOGISTICS-4-0.pddl 10 1.1 stability 
```


# Dependencies
For some of the diverse planners, the dependencies are as follows:
1. Computation of a subset of plans is performed in a post-processing, path to the code should be specified in an environment variable **DIVERSE_SCORE_COMPUTATION_PATH**. The code can be found [here](https://github.com/IBM/diversescore).
2. Note that for the diversity-bounded diverse planning and for diversity-optimal one the computation in a post-processing requires enabling CPLEX support in Fast Downward (see https://www.fast-downward.org/) and building the post-processing code with LP support.

# Building the package:
```bash
# Testing locally
pip install tox pytest -e .
tox
# Output a wheel
python -c "import setuptools; setuptools.setup()" bdist_wheel
```

# Using as a package:
```bash
pip install git+https://github.com/IBM/forbiditerative.git
```
Due to the CLI-oriented design, the code must be run using subprocess.
```python
import sys
import subprocess
import logging
from subprocess import SubprocessError

try:
    output = subprocess.check_output([sys.executable, "-m" "forbiditerative.plan", "..your args"])
except SubprocessError as err:
    logging.error(err.output.decode())
```

## Citing

### Top-k planning
```
@InProceedings{katz-et-al-icaps2018,
  title =        "A Novel Iterative Approach to Top-k Planning",
  author =       "Michael Katz and Shirin Sohrabi and Octavian Udrea and Dominik Winterer",
  booktitle =    "Proceedings of the Twenty-Eighth International Conference on
                  Automated Planning and Scheduling (ICAPS 2018)",
  publisher =    "{AAAI} Press",
  pages =        "132--140",
  year =         "2018"
}
```

### Top-quality planning
```
@InProceedings{katz-et-al-aaai2020,
  author =       "Michael Katz and Shirin Sohrabi and Octavian Udrea",
  title =        "Top-Quality Planning: Finding Practically Useful Sets of Best Plans",
  booktitle =    "Proceedings of the Thirty-Fourth {AAAI} Conference on
                  Artificial Intelligence ({AAAI} 2020)",
  publisher =    "{AAAI} Press",
  pages =        "9900--9907",
  year =         "2020"
}

@InProceedings{katz-sohrabi-icaps2022,
  author =       "Michael Katz and Shirin Sohrabi",
  title =        "Who Needs These Operators Anyway: Top Quality Planning with Operator Subset Criteria",
  booktitle =    "Proceedings of the Thirty-Second International Conference on
                  Automated Planning and Scheduling (ICAPS 2022)",
  publisher =    "{AAAI} Press",
  year =         "2022"
}
```

### Diverse planning
```
@InProceedings{katz-sohrabi-aaai2020,
  title =        "Reshaping diverse planning",
  author =       "Michael Katz and Shirin Sohrabi",
  booktitle =    "Proceedings of the Thirty-Fourth {AAAI} Conference on
                  Artificial Intelligence ({AAAI} 2020)",
  publisher =    "{AAAI} Press",
  pages =        "9892--9899",
  year =         "2020"
}

@InProceedings{katz-et-al-aaai2022,
  title =        "Bounding Quality in Diverse Planning",
  author =       "Michael Katz and Shirin Sohrabi and Octavian Udrea",
  booktitle =    "Proceedings of the Thirty-Sixth {AAAI} Conference on
                  Artificial Intelligence ({AAAI} 2022)",
  publisher =    "{AAAI} Press",
  year =         "2022"
}
```

## Licensing

Forbid-Iterative (FI) Planner is an Automated PDDL based planner that
includes planners for top-k, top-quality, and diverse computational
tasks. Copyright (C) 2019  Michael Katz, IBM Research, USA.
The code extends the Fast Downward planning system. The license for the
extension is specified in the LICENSE file.

## Fast Downward
<img src="misc/images/fast-downward.svg" width="800" alt="Fast Downward">

Fast Downward is a domain-independent classical planning system.

Copyright 2003-2022 Fast Downward contributors (see below).

For further information:
- Fast Downward website: <https://www.fast-downward.org>
- Report a bug or file an issue: <https://issues.fast-downward.org>
- Fast Downward mailing list: <https://groups.google.com/forum/#!forum/fast-downward>
- Fast Downward main repository: <https://github.com/aibasel/downward>


## Tested software versions

This version of Fast Downward has been tested with the following software versions:

| OS           | Python | C++ compiler                                                     | CMake |
| ------------ | ------ | ---------------------------------------------------------------- | ----- |
| Ubuntu 20.04 | 3.8    | GCC 9, GCC 10, Clang 10, Clang 11                                | 3.16  |
| Ubuntu 18.04 | 3.6    | GCC 7, Clang 6                                                   | 3.10  |
| macOS 10.15  | 3.6    | AppleClang 12                                                    | 3.19  |
| Windows 10   | 3.6    | Visual Studio Enterprise 2017 (MSVC 19.16) and 2019 (MSVC 19.28) | 3.19  |

We test LP support with CPLEX 12.9, SoPlex 3.1.1 and Osi 0.107.9.
On Ubuntu, we test both CPLEX and SoPlex. On Windows, we currently
only test CPLEX, and on macOS, we do not test LP solvers (yet).


## Contributors

The following list includes all people that actively contributed to
Fast Downward, i.e. all people that appear in some commits in Fast
Downward's history (see below for a history on how Fast Downward
emerged) or people that influenced the development of such commits.
Currently, this list is sorted by the last year the person has been
active, and in case of ties, by the earliest year the person started
contributing, and finally by last name.

- 2003-2022 Malte Helmert
- 2008-2016, 2018-2022 Gabriele Roeger
- 2010-2022 Jendrik Seipp
- 2010-2011, 2013-2022 Silvan Sievers
- 2012-2022 Florian Pommerening
- 2013, 2015-2022 Salomé Eriksson
- 2018-2022 Patrick Ferber
- 2021-2022 Clemens Büchner
- 2021-2022 Dominik Drexler
- 2022 Remo Christen
- 2015, 2021 Thomas Keller
- 2016-2020 Cedric Geissmann
- 2017-2020 Guillem Francès
- 2018-2020 Augusto B. Corrêa
- 2020 Rik de Graaff
- 2015-2019 Manuel Heusner
- 2017 Daniel Killenberger
- 2016 Yusra Alkhazraji
- 2016 Martin Wehrle
- 2014-2015 Patrick von Reth
- 2009-2014 Erez Karpas
- 2014 Robert P. Goldman
- 2010-2012 Andrew Coles
- 2010, 2012 Patrik Haslum
- 2003-2011 Silvia Richter
- 2009-2011 Emil Keyder
- 2010-2011 Moritz Gronbach
- 2010-2011 Manuela Ortlieb
- 2011 Vidal Alcázar Saiz
- 2011 Michael Katz
- 2011 Raz Nissim
- 2010 Moritz Goebelbecker
- 2007-2009 Matthias Westphal
- 2009 Christian Muise


## History

The current version of Fast Downward is the merger of three different
projects:

- the original version of Fast Downward developed by Malte Helmert
  and Silvia Richter
- LAMA, developed by Silvia Richter and Matthias Westphal based on
  the original Fast Downward
- FD-Tech, a modified version of Fast Downward developed by Erez
  Karpas and Michael Katz based on the original code

In addition to these three main sources, the codebase incorporates
code and features from numerous branches of the Fast Downward codebase
developed for various research papers. The main contributors to these
branches are Malte Helmert, Gabi Röger and Silvia Richter.


## License

The following directory is not part of Fast Downward as covered by
this license:

- ./src/search/ext

For the rest, the following license applies:

```
Fast Downward is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

Fast Downward is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
```
