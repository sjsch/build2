// file      : build2/cc/compile.hxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#ifndef BUILD2_CC_COMPILE_HXX
#define BUILD2_CC_COMPILE_HXX

#include <libbutl/path-map.hxx>

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/rule.hxx>
#include <build2/filesystem.hxx> // auto_rmfile

#include <build2/cc/types.hxx>
#include <build2/cc/common.hxx>

namespace build2
{
  class depdb;

  namespace cc
  {
    // The order is arranged so that their integral values indicate whether
    // one is a "stronger" than another.
    //
    enum class preprocessed: uint8_t {none, includes, modules, all};

    // Positions of the re-exported bmi{}s. See search_modules() for
    // details.
    //
    struct module_positions
    {
      size_t start;    // First imported    bmi*{}, 0 if none.
      size_t exported; // First re-exported bmi*{}, 0 if none.
      size_t copied;   // First copied-over bmi*{}, 0 if none.
    };

    class compile: public rule, virtual common
    {
    public:
      compile (data&&);

      virtual match_result
      match (action, target&, const string&) const override;

      virtual recipe
      apply (action, target&) const override;

      target_state
      perform_update (action, const target&) const;

      target_state
      perform_clean (action, const target&) const;

    private:
      struct match_data;
      using environment = small_vector<const char*, 2>;

      void
      append_lib_options (const scope&,
                          cstrings&,
                          const target&,
                          action, lorder) const;

      void
      hash_lib_options (const scope&,
                        sha256&,
                        const target&,
                        action, lorder) const;

      // Mapping of include prefixes (e.g., foo in <foo/bar>) for auto-
      // generated headers to directories where they will be generated.
      //
      // We are using a prefix map of directories (dir_path_map) instead of
      // just a map in order to also cover sub-paths (e.g., <foo/more/bar> if
      // we continue with the example). Specifically, we need to make sure we
      // don't treat foobar as a sub-directory of foo.
      //
      // @@ The keys should be normalized.
      //
      using prefix_map = butl::dir_path_map<dir_path>;

      void
      append_prefixes (prefix_map&, const target&, const variable&) const;

      void
      append_lib_prefixes (const scope&,
                           prefix_map&,
                           target&,
                           action, lorder) const;

      prefix_map
      build_prefix_map (const scope&, target&, action, lorder) const;

      // Reverse-lookup target type from extension.
      //
      const target_type*
      map_extension (const scope&, const string&, const string&) const;

      pair<auto_rmfile, bool>
      extract_headers (action, file&, lorder,
                       const file&, const match_data&,
                       depdb&, bool&) const;

      translation_unit
      parse_unit (action, file&, lorder,
                  const file&, auto_rmfile&, const match_data&) const;

      void
      extract_modules (action, file&, lorder, const compile_target_types&,
                       const file&, match_data&,
                       module_info&&, depdb&, bool&) const;

      module_positions
      search_modules (action, file&, lorder, const target_type&,
                      const file&, module_imports&&, sha256&) const;

      void
      append_modules (environment&, cstrings&, strings&,
                      const file&, const match_data&) const;

      // Language selection option (for VC) or the value for the -x option.
      //
      const char*
      langopt (const match_data&) const;

      void
      append_symexport_options (cstrings&, const target&) const;

    private:
      const string rule_id;
    };
  }
}

#endif // BUILD2_CC_COMPILE_HXX
