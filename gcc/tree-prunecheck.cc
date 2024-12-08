/* Prototype pass to mark function clones for pruning

   Chris Tyler, Seneca Polytechnic College, 2024-11
   John Pierre Mendoza, Seneca Polytechnic College, 2024-12

   Modelled on tree-nrv.cc

   This pass attempts to compare function clones to check whether or not their
   bodies are essentially the same. Function clones that are essentially the
   same should be marked for pruning.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#define INCLUDE_MEMORY
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "tree.h"
#include "gimple.h"
#include "tree-pass.h"
#include "ssa.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "internal-fn.h"
#include "gimple-pretty-print.h"

// Added headers:
#include "gimple-ssa.h"
#include "cgraph.h"
#include "attribs.h"
#include "pretty-print.h"
#include "tree-inline.h"
#include "intl.h"

// Included for dump_printf:
//#include "tree-pretty-print.h"
//#include "diagnostic.h"
//#include "dumpfile.h"
//#include "builtins.h"
//#include <stdlib.h>

// =================================================== vvv
// Test pass



namespace {

const pass_data pass_data_prune_check =
{
  GIMPLE_PASS, /* type */
  "prunecheck", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  PROP_cfg, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0, /* todo_flags_finish */
};

class pass_prunecheck: public gimple_opt_pass
{
public:
  pass_prunecheck(gcc::context *ctxt)
    : gimple_opt_pass(pass_data_prune_check, ctxt)
  { }

  /* opt_pass methods: */
  bool gate(function *) final override {
    return 1; // always execute pass
  }

  unsigned int execute(function *) final override;
}; // class pass_ctyler

unsigned int
// pass_prunecheck::execute(function *fun)
pass_prunecheck::execute(function *)
{
  struct cgraph_node *node;
  int func_cnt = 0;

  FOR_EACH_FUNCTION (node)
  {
    if (dump_file)
    {

      basic_block bb;
      int bb_cnt = 0, stmt_cnt = 0;

      struct function *fdecl = node->get_fun();
      cgraph_function_version_info *vi = node->function_version();


      if (fdecl != NULL && vi != NULL) {
        fprintf (dump_file, "=== Function %d Name '%s' ===\n", ++func_cnt, node->name() );
        fprintf (dump_file, "=== Is a function version: true ===\n");

        char *codes = (char *)xmalloc((stmt_cnt + 1) * sizeof(char));


        FOR_EACH_BB_FN(bb, fdecl) {

          bb_cnt++;
          // if (dump_file)
          // {
          //   fprintf (dump_file, "===== Basic block count: %d =====\n", bb_cnt);
          // }

          for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
            gimple *g = gsi_stmt(gsi);
            stmt_cnt++;
            int code = gimple_code(g);

            codes = (char *)xrealloc(codes, (stmt_cnt + 1) * sizeof(char));
            codes[stmt_cnt - 1] = code + 48;

            // if (dump_file)
            // {
            //   fprintf (dump_file, "----- Statement count: %d -----\n", stmt_cnt);
            //   fprintf (dump_file, "----- Gimple code: %d -----\n", code);
            //   print_gimple_stmt (dump_file, g, 0, TDF_VOPS|TDF_MEMSYMS);
            // }
          }
        }

        codes[stmt_cnt] = 0;

        if (dump_file) {
          fprintf(dump_file, "----- Function codes summary: -----\n");
          fprintf(dump_file, "%s\n\n", codes);
        }

        free(codes);

      }
      // else {
      //   fprintf (dump_file, "=== Is a function version: false ===\n\n");
      // }
    }
  }

  if (dump_file) {
    fprintf(dump_file, "\n\n##### End function prune check, start regular dump of current gimple #####\n\n\n");
  }

  return 0;
}

} // anon namespace

gimple_opt_pass *
make_pass_prunecheck(gcc::context *ctxt) {
  return new pass_prunecheck(ctxt);
}

// ============================================================= ^^^

