#include "server.h"

namespace zeppelin
{
namespace library
{
class MusicLibrary;
}
}

// =====================================================================================================================
extern "C"
std::shared_ptr<zeppelin::plugin::Plugin> plugin_create(const std::shared_ptr<zeppelin::library::MusicLibrary>& library,
							const std::shared_ptr<zeppelin::player::Controller>& ctrl)
{
    return Server::create(ctrl);
}
