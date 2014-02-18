# -*- python -*-

vars = Variables()
vars.Add(PathVariable('PREFIX', 'prefix used to install files', '/usr'))
vars.Add(PathVariable('WEBSOCKETPP', 'path of the websocket++ checkout', None))
vars.Add(PathVariable('JSONCPP', 'path of jsoncpp library', None))

env = Environment(variables = vars)

env["CPPFLAGS"] = ["-O2", "-Wall", "-Werror", "-Wshadow", "-std=c++11", "-pthread"]
env["CPPPATH"] = [Dir("src")]
env["CPPDEFINES"] = ["_WEBSOCKETPP_CPP11_STL_"]

env["SHCXXCOMSTR"] = "Compiling $SOURCE"
env["SHLINKCOMSTR"] = "Linking $TARGET"

if "PREFIX" in env :
    env["CPPPATH"] += ["%s/include" % env["PREFIX"]]

if "WEBSOCKETPP" in env :
    env["CPPPATH"] += [env["WEBSOCKETPP"]]

if "JSONCPP" in env :
    env["CPPPATH"] += ["%s/include" % env["JSONCPP"]]
    env["LIBPATH"] += ["%s/lib" % env["JSONCPP"]]

plugin = env.SharedLibrary(
    target = "websocket-notifier",
    source = ["src/server.cpp", "src/plugin.cpp"]
)

env.Alias("install", env.Install("$PREFIX/lib/zeppelin/plugins", plugin))
