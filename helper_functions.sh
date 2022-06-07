

function check_enough_plans {
    PLANSDIR=$1
    NUM_PLANS=$2

    num_found_plans=`ls -1q $PLANSDIR/sas_plan.* | wc -l`

    if [ "$NUM_PLANS" -gt $num_found_plans ]; then
        echo "The number of bounded unordered plans found is smaller than K."
    else 
        echo "The number of bounded unordered plans found is sufficient."
    fi
}

function extend_plans_complete {
    PLANSDIR=$1
    NUM_PLANS=$2
    domain=$3
    problem=$4

    SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
    num_found_plans=`ls -1q $PLANSDIR/sas_plan.* | wc -l`

    if [ "$NUM_PLANS" -gt $num_found_plans ]; then
        echo "The number of bounded unordered plans found is smaller than the total number of plans required for the first phase."
        echo "Finding additional plans by generating all possible reorderings of found plans, until the total number of plans is reached."
        found_so_far=$num_found_plans

        for ((i=1; i<=$num_found_plans; i++)); do
            plan_file=$PLANSDIR/sas_plan.$i
            remaining=$( expr $NUM_PLANS - $found_so_far )

            SEARCH_PARAMS="forbid_iterative(reformulate=NONE_FIND_ADDITIONAL_PLANS,dump=false,number_of_plans=$remaining,external_plan_file=$plan_file)"
            (cd $PLANSDIR && 
                $SOURCE/fast-downward.py $domain $problem --internal-previous-portfolio-plans $found_so_far --search $SEARCH_PARAMS)
            # Check how many plans we now have
            found_so_far=`ls -1q $PLANSDIR/sas_plan.* | wc -l`
            if [ "$found_so_far" -ge $NUM_PLANS ]; then
                echo "Extended the set of plans to $found_so_far plans"
                break
            fi
        done
    fi
}


function extend_plans_uniformly {
    PLANSDIR=$1
    NUM_PLANS=$2
    domain=$3
    problem=$4

    SOURCE="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
    num_found_plans=`ls -1q $PLANSDIR/sas_plan.* | wc -l`

    if [ "$NUM_PLANS" -gt $num_found_plans ]; then
        echo "The number of bounded unordered plans found is smaller than the total number of plans required for the first phase."
        echo "Finding additional plans by generating reorderings of found plans, same number from each plan, until the total number of plans for the first phase is reached."
        found_so_far=$num_found_plans

        let n=($NUM_PLANS+$num_found_plans-1)/$num_found_plans
        for ((i=1; i<=$num_found_plans; i++)); do
            plan_file=$PLANSDIR/sas_plan.$i

            echo "Generating additional $n plans for each found plan"
            SEARCH_PARAMS="forbid_iterative(reformulate=NONE_FIND_ADDITIONAL_PLANS,dump=false,number_of_plans=$n,external_plan_file=$plan_file)"
            (cd $PLANSDIR && 
                $SOURCE/fast-downward.py $domain $problem --internal-previous-portfolio-plans $found_so_far --search $SEARCH_PARAMS)
            # Check how many plans we now have
            found_so_far=`ls -1q $PLANSDIR/sas_plan.* | wc -l`
        done
    fi
}



function get_metric_param() {
    ret=""
    if [[ $1 == *"stability"* ]]; then
        ret=$ret"compute_stability_metric=true,"
    fi    
    if [[ $1 == *"state"* ]]; then
        ret=$ret"compute_states_metric=true,"
    fi    
    if [[ $1 == *"uniqueness"* ]]; then
        ret=$ret"compute_uniqueness_metric=true,"
    fi    
    echo $ret 
}

