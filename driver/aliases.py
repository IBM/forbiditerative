import os

from .util import DRIVER_DIR


PORTFOLIO_DIR = os.path.join(DRIVER_DIR, "portfolios")

ALIASES = {}


ALIASES["seq-sat-fd-autotune-1"] = [
    "--evaluator", "hff=ff(transform=adapt_costs(one))",
    "--evaluator", "hcea=cea()",
    "--evaluator", "hcg=cg(transform=adapt_costs(plusone))",
    "--evaluator", "hgc=goalcount()",
    "--evaluator", "hAdd=add()",
    "--search", """iterated([
lazy(alt([single(sum([g(),weight(hff,10)])),
          single(sum([g(),weight(hff,10)]),pref_only=true)],
         boost=2000),
     preferred=[hff],reopen_closed=false,cost_type=one),
lazy(alt([single(sum([g(),weight(hAdd,7)])),
          single(sum([g(),weight(hAdd,7)]),pref_only=true),
          single(sum([g(),weight(hcg,7)])),
          single(sum([g(),weight(hcg,7)]),pref_only=true),
          single(sum([g(),weight(hcea,7)])),
          single(sum([g(),weight(hcea,7)]),pref_only=true),
          single(sum([g(),weight(hgc,7)])),
          single(sum([g(),weight(hgc,7)]),pref_only=true)],
         boost=1000),
     preferred=[hcea,hgc],reopen_closed=false,cost_type=one),
lazy(alt([tiebreaking([sum([g(),weight(hAdd,3)]),hAdd]),
          tiebreaking([sum([g(),weight(hAdd,3)]),hAdd],pref_only=true),
          tiebreaking([sum([g(),weight(hcg,3)]),hcg]),
          tiebreaking([sum([g(),weight(hcg,3)]),hcg],pref_only=true),
          tiebreaking([sum([g(),weight(hcea,3)]),hcea]),
          tiebreaking([sum([g(),weight(hcea,3)]),hcea],pref_only=true),
          tiebreaking([sum([g(),weight(hgc,3)]),hgc]),
          tiebreaking([sum([g(),weight(hgc,3)]),hgc],pref_only=true)],
         boost=5000),
     preferred=[hcea,hgc],reopen_closed=false,cost_type=normal),
eager(alt([tiebreaking([sum([g(),weight(hAdd,10)]),hAdd]),
           tiebreaking([sum([g(),weight(hAdd,10)]),hAdd],pref_only=true),
           tiebreaking([sum([g(),weight(hcg,10)]),hcg]),
           tiebreaking([sum([g(),weight(hcg,10)]),hcg],pref_only=true),
           tiebreaking([sum([g(),weight(hcea,10)]),hcea]),
           tiebreaking([sum([g(),weight(hcea,10)]),hcea],pref_only=true),
           tiebreaking([sum([g(),weight(hgc,10)]),hgc]),
           tiebreaking([sum([g(),weight(hgc,10)]),hgc],pref_only=true)],
          boost=500),
      preferred=[hcea,hgc],reopen_closed=true,cost_type=normal)
],repeat_last=true,continue_on_fail=true)"""]

ALIASES["seq-sat-fd-autotune-2"] = [
    "--evaluator", "hcea=cea(transform=adapt_costs(plusone))",
    "--evaluator", "hcg=cg(transform=adapt_costs(one))",
    "--evaluator", "hgc=goalcount(transform=adapt_costs(plusone))",
    "--evaluator", "hff=ff()",
    "--search", """iterated([
ehc(hcea,preferred=[hcea],preferred_usage=0,cost_type=normal),
lazy(alt([single(sum([weight(g(),2),weight(hff,3)])),
          single(sum([weight(g(),2),weight(hff,3)]),pref_only=true),
          single(sum([weight(g(),2),weight(hcg,3)])),
          single(sum([weight(g(),2),weight(hcg,3)]),pref_only=true),
          single(sum([weight(g(),2),weight(hcea,3)])),
          single(sum([weight(g(),2),weight(hcea,3)]),pref_only=true),
          single(sum([weight(g(),2),weight(hgc,3)])),
          single(sum([weight(g(),2),weight(hgc,3)]),pref_only=true)],
         boost=200),
     preferred=[hcea,hgc],reopen_closed=false,cost_type=one),
lazy(alt([single(sum([g(),weight(hff,5)])),
          single(sum([g(),weight(hff,5)]),pref_only=true),
          single(sum([g(),weight(hcg,5)])),
          single(sum([g(),weight(hcg,5)]),pref_only=true),
          single(sum([g(),weight(hcea,5)])),
          single(sum([g(),weight(hcea,5)]),pref_only=true),
          single(sum([g(),weight(hgc,5)])),
          single(sum([g(),weight(hgc,5)]),pref_only=true)],
         boost=5000),
     preferred=[hcea,hgc],reopen_closed=true,cost_type=normal),
lazy(alt([single(sum([g(),weight(hff,2)])),
          single(sum([g(),weight(hff,2)]),pref_only=true),
          single(sum([g(),weight(hcg,2)])),
          single(sum([g(),weight(hcg,2)]),pref_only=true),
          single(sum([g(),weight(hcea,2)])),
          single(sum([g(),weight(hcea,2)]),pref_only=true),
          single(sum([g(),weight(hgc,2)])),
          single(sum([g(),weight(hgc,2)]),pref_only=true)],
         boost=1000),
     preferred=[hcea,hgc],reopen_closed=true,cost_type=one)
],repeat_last=true,continue_on_fail=true)"""]

def _get_lama(**kwargs):
    return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),pref={pref})".format(**kwargs),
        "--evaluator", "hff=ff()",
        "--search", """iterated([
                         lazy_greedy([hff,hlm],preferred=[hff,hlm]),
                         lazy_wastar([hff,hlm],preferred=[hff,hlm],w=5),
                         lazy_wastar([hff,hlm],preferred=[hff,hlm],w=3),
                         lazy_wastar([hff,hlm],preferred=[hff,hlm],w=2),
                         lazy_wastar([hff,hlm],preferred=[hff,hlm],w=1)
                         ],repeat_last=true,continue_on_fail=true)""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm1=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", "hff1=ff(transform=adapt_costs(one))",
        "--evaluator",
        "hlm2=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(plusone),pref={pref})".format(**kwargs),
        "--evaluator", "hff2=ff(transform=adapt_costs(plusone))",
        "--search", """iterated([
                         lazy_greedy([hff1,hlm1],preferred=[hff1,hlm1],
                                     cost_type=one,reopen_closed=false),
                         lazy_greedy([hff2,hlm2],preferred=[hff2,hlm2],
                                     reopen_closed=false),
                         lazy_wastar([hff2,hlm2],preferred=[hff2,hlm2],w=5),
                         lazy_wastar([hff2,hlm2],preferred=[hff2,hlm2],w=3),
                         lazy_wastar([hff2,hlm2],preferred=[hff2,hlm2],w=2),
                         lazy_wastar([hff2,hlm2],preferred=[hff2,hlm2],w=1)
                         ],repeat_last=true,continue_on_fail=true)""",
        # Append --always to be on the safe side if we want to append
        # additional options later.
        "--always"]

ALIASES["seq-sat-lama-2011"] = _get_lama(pref="true")
ALIASES["lama"] = _get_lama(pref="false")

ALIASES["lama-first"] = [
    "--evaluator",
    "hlm=lmcount(lm_factory=lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref=false)",
    "--evaluator", "hff=ff(transform=adapt_costs(one))",
    "--search", """lazy_greedy([hff,hlm],preferred=[hff,hlm],
                               cost_type=one,reopen_closed=false)"""]

ALIASES["seq-opt-bjolp"] = [
    "--evaluator",
    "lmc=lmcount(lm_merged([lm_rhw(),lm_hm(m=1)]),admissible=true)",
    "--search",
    "astar(lmc,lazy_evaluator=lmc)"]

ALIASES["seq-opt-lmcut"] = [
    "--search", "astar(lmcut())"]


def _get_cerberus(**kwargs):
    return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true)".format(**kwargs),
        "--evaluator", 
        "hn=novelty_simplified(eval=hrb)",        
        "--search", """iterated([
                         lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000),preferred=[hrb,hlm]),
                         lazy_wastar([hrb,hlm],preferred=[hrb,hlm],w=5),
                         lazy_wastar([hrb,hlm],preferred=[hrb,hlm],w=3),
                         lazy_wastar([hrb,hlm],preferred=[hrb,hlm],w=2),
                         lazy_wastar([hrb,hlm],preferred=[hrb,hlm],w=1)
                         ],repeat_last=true,continue_on_fail=true)""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm1=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb1=RB(dag={dag}, extract_plan=true, transform=adapt_costs(one))".format(**kwargs),
        "--evaluator",
        "hn=novelty_simplified(eval=hrb1)",
        "--evaluator",
        "hlm2=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(plusone),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb2=RB(dag={dag}, extract_plan=true, transform=adapt_costs(plusone))".format(**kwargs),
        "--search", """iterated([
                         lazy(open=alt([tiebreaking([hn, hrb1]), single(hrb1,pref_only=true), single(hlm1), single(hlm1,pref_only=true)], boost=1000), preferred=[hrb1,hlm1],
                                     cost_type=one,reopen_closed=false),
                         lazy_greedy([hrb2,hlm2],preferred=[hrb2,hlm2],
                                     reopen_closed=false),
                         lazy_wastar([hrb2,hlm2],preferred=[hrb2,hlm2],w=5),
                         lazy_wastar([hrb2,hlm2],preferred=[hrb2,hlm2],w=3),
                         lazy_wastar([hrb2,hlm2],preferred=[hrb2,hlm2],w=2),
                         lazy_wastar([hrb2,hlm2],preferred=[hrb2,hlm2],w=1)
                         ],repeat_last=true,continue_on_fail=true)""",
        # Append --always to be on the safe side if we want to append
        # additional options later.
        "--always"]


def _get_cerberus_first(**kwargs):
    return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true)".format(**kwargs),
        "--evaluator", 
        "hn=novelty_simplified(eval=hrb)",        
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000),preferred=[hrb,hlm])""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true, transform=adapt_costs(one))".format(**kwargs),
        "--evaluator", 
        "hn=novelty_simplified(eval=hrb)",        
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000), preferred=[hrb,hlm],
                                     cost_type=one,reopen_closed=false)""",
        # Append --always to be on the safe side if we want to append
        # additional options later.
        "--always"]


def _get_cerberus_novelty_ops_first(**kwargs):
    cutoff = "cutoff_type={cutoff_type}".format(**kwargs)
    if "cutoff_bound" in kwargs:
        cutoff = cutoff + ",cutoff_bound={cutoff_bound}".format(**kwargs)
    return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true)".format(**kwargs),
        "--evaluator", 
        "hn=novelty(evals=[hrb], type=separate_both, pref=true, %s)" % cutoff,
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hn,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000),preferred=[hrb,hlm])""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true, transform=adapt_costs(one))".format(**kwargs),
        "--evaluator", 
        "hn=novelty(evals=[hrb], type=separate_both, pref=true, %s)" % cutoff,
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hn,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000), preferred=[hrb,hlm],
                                     cost_type=one,reopen_closed=false)""",
        "--always"]
        # Append --always to be on the safe side if we want to append
        # additional options later.


def _get_new_cerberus_first(**kwargs):
    return [
        "--if-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true)".format(**kwargs),
        "--evaluator", 
        "hn=novelty(evals=[hrb], type=separate_both)",
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000),preferred=[hrb,hlm])""",
        "--if-non-unit-cost",
        "--evaluator",
        "hlm=lmcount(lm_reasonable_orders_hps(lm_rhw()),transform=adapt_costs(one),pref={pref})".format(**kwargs),
        "--evaluator", 
        "hrb=RB(dag={dag}, extract_plan=true, transform=adapt_costs(one))".format(**kwargs),
        "--evaluator", 
        "hn=novelty(evals=[hrb], type=separate_both)",
        "--search", """lazy(open=alt([tiebreaking([hn, hrb]), single(hrb,pref_only=true), single(hlm), single(hlm,pref_only=true)], boost=1000), preferred=[hrb,hlm],
                                     cost_type=one,reopen_closed=false)""",
        "--always"]
        # Append --always to be on the safe side if we want to append
        # additional options later.

ALIASES["seq-agl-cerberus2018"] = _get_cerberus_first(pref="true", dag="from_coloring")
ALIASES["seq-sat-cerberus2018"] = _get_cerberus(pref="true", dag="from_coloring")
ALIASES["seq-agl-cerberus-gl-2018"] = _get_cerberus_first(pref="true", dag="greedy_level")
ALIASES["seq-sat-cerberus-gl-2018"] = _get_cerberus(pref="true", dag="greedy_level")

ALIASES["seq-agl-cerberus-novelops-argmax"] = _get_cerberus_novelty_ops_first(pref="true", dag="from_coloring", cutoff_type="argmax")
ALIASES["seq-agl-cerberus-novelops-co1"] = _get_cerberus_novelty_ops_first(pref="true", dag="from_coloring", cutoff_type="all_ordered", cutoff_bound="1")

ALIASES["seq-agl-cerberus-new"] = _get_new_cerberus_first(pref="true", dag="from_coloring")

ALIASES["seq-opt-oss-por-lmcut"] = ["--if-conditional-effects", "--evaluator", "h=celmcut()",
                "--if-no-conditional-effects", "--evaluator", "h=lmcut()",
                "--always", "--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=oss, \
                    stabilize_initial_state=false, keep_operator_symmetries=false)",
                "--search",
                "astar(h, symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))"]


ALIASES["seq-sopt-oss-por-lmcut"] = ["--if-conditional-effects", "--evaluator", "h=celmcut()",
        "--if-no-conditional-effects", "--evaluator", "h=lmcut()",
        "--always", "--symmetries", "sym=structural_symmetries(time_bound=0,search_symmetries=oss, stabilize_initial_state=false, keep_operator_symmetries=false)",
        "--if-unit-cost",
        "--search", "astar(h, symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))",
        "--if-non-unit-cost",
        "--search", "shortest_astar(h, symmetries=sym,pruning=stubborn_sets_simple(min_required_pruning_ratio=0.01,expansions_before_checking_pruning_ratio=1000))",
        "--always"]


ALIASES["get_landmarks_exhaust"] = ["--evaluator", "h=lmcount(lm_factory=lm_exhaust(),dump_action_landmarks=true)",
                "--always", "--search", "lazy_greedy([h,const(infinity())])"]

ALIASES["get_landmarks_h1"] = ["--evaluator", "h=lmcount(lm_factory=lm_hm(m=1),dump_action_landmarks=true)",
                "--always", "--search", "lazy_greedy([h,const(infinity())])"]

ALIASES["get_landmarks_h2"] = ["--evaluator", "h=lmcount(lm_factory=lm_hm(m=2),dump_action_landmarks=true)",
                "--always", "--search", "lazy_greedy([h,const(infinity())])"]

ALIASES["get_landmarks_rhw"] = ["--evaluator", "h=lmcount(lm_factory=lm_rhw(),dump_action_landmarks=true)",
                "--always", "--search", "lazy_greedy([h,const(infinity())])"]

ALIASES["get_landmarks_zg"] = ["--evaluator", "h=lmcount(lm_factory=lm_zg(),dump_action_landmarks=true)",
                "--always", "--search", "lazy_greedy([h,const(infinity())])"]

PORTFOLIOS = {}
for portfolio in os.listdir(PORTFOLIO_DIR):
    if portfolio == "__pycache__":
        continue
    name, ext = os.path.splitext(portfolio)
    assert ext == ".py", portfolio
    PORTFOLIOS[name.replace("_", "-")] = os.path.join(PORTFOLIO_DIR, portfolio)


def show_aliases():
    for alias in sorted(list(ALIASES) + list(PORTFOLIOS)):
        print(alias)


def set_options_for_alias(alias_name, args):
    """
    If alias_name is an alias for a configuration, set args.search_options
    to the corresponding command-line arguments. If it is an alias for a
    portfolio, set args.portfolio to the path to the portfolio file.
    Otherwise raise KeyError.
    """
    assert not args.search_options
    assert not args.portfolio

    if alias_name in ALIASES:
        args.search_options = [x.replace(" ", "").replace("\n", "")
                               for x in ALIASES[alias_name]]
    elif alias_name in PORTFOLIOS:
        args.portfolio = PORTFOLIOS[alias_name]
    else:
        raise KeyError(alias_name)
