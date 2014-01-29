#include "server.h"

namespace zeppelin
{
namespace library
{
class MusicLibrary;
}
}

// TODO: someone should delete this :)
static std::shared_ptr<Server> s_instance;

// =====================================================================================================================
extern "C"
zeppelin::plugin::Plugin* plugin_create(const std::shared_ptr<zeppelin::library::MusicLibrary>& library,
					const std::shared_ptr<zeppelin::player::Controller>& ctrl)
{
    s_instance = Server::create(ctrl);
    return s_instance.get();
}
