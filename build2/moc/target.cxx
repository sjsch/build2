#include <build2/moc/target.hxx>

#include <build2/context.hxx>

using namespace std;
using namespace butl;

namespace build2
{
  namespace moc
  {
    extern const char moc_cxx_ext_def[] = "moc.cxx";

    const target_type moc_cxx::static_type
    {
      "moc.cxx",
      &cxx::static_type,
      &target_factory<moc_cxx>,
      nullptr,
      &target_extension_var<var_extension, moc_cxx_ext_def>,
      &target_pattern_var<var_extension, moc_cxx_ext_def>,
      nullptr,
      &file_search,
      false
    };
  }
}
