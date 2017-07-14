#include "eager_search.h"
#include "search_common.h"

#include "../option_parser.h"
#include "../plugin.h"

using namespace std;

namespace plugin_astar {
static shared_ptr<SearchEngine> _parse(OptionParser &parser) {
    parser.document_synopsis(
        "A* search (eager)",
        "A* is a special case of eager best first search that uses g+h "
        "as f-function. "
        "We break ties using the evaluator. Closed nodes are re-opened.");
    parser.document_note(
        "mpd option",
        "This option is currently only present for the A* algorithm and not "
        "for the more general eager search, "
        "because the current implementation of multi-path depedence "
        "does not support general open lists.");
    parser.document_note(
        "Equivalent statements using general eager search",
        "\n```\n--search astar(evaluator)\n```\n"
        "is equivalent to\n"
        "```\n--heuristic h=evaluator\n"
        "--search eager(tiebreaking([sum([g(), h]), h], unsafe_pruning=false),\n"
        "               reopen_closed=true, f_eval=sum([g(), h]))\n"
        "```\n", true);
    parser.add_option<Evaluator *>("eval", "evaluator for h-value");
    parser.add_option<bool>("mpd",
                            "use multi-path dependence (LM-A*)", "false");

    SearchEngine::add_pruning_option(parser);
    SearchEngine::add_options_to_parser(parser);
    parser.add_option<shared_ptr<Group>>(
        "symmetries",
        "symmetries object to compute structural symmetries for pruning",
        OptionParser::NONE);
    Options opts = parser.parse();

    shared_ptr<eager_search::EagerSearch> engine;
    if (!parser.dry_run()) {
        if (opts.contains("symmetries")) {
            shared_ptr<Group> group = opts.get<shared_ptr<Group>>("symmetries");
            if (group->get_search_symmetries() == SearchSymmetries::NONE) {
                cerr << "Symmetries option passed to eager search, but no "
                     << "search symmetries should be used." << endl;
                utils::exit_with(utils::ExitCode::INPUT_ERROR);
            }
        }
        auto temp = search_common::create_astar_open_list_factory_and_f_eval(opts);
        opts.set("open", temp.first);
        opts.set("f_eval", temp.second);
        opts.set("reopen_closed", true);
        vector<Heuristic *> preferred_list;
        opts.set("preferred", preferred_list);
        engine = make_shared<eager_search::EagerSearch>(opts);
    }

    return engine;
}

static PluginShared<SearchEngine> _plugin("astar", _parse);
}
