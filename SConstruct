# -*- python -*-

vars = Variables()
vars.Add(PathVariable('PREFIX', 'prefix used to install files', '/'))
vars.Add(PathVariable('WEBSOCKETPP', 'path of the websocket++ checkout', None))

env = Environment(variables = vars)

env["CPPFLAGS"] = ["-O2", "-Wall", "-Werror", "-Wshadow", "-std=c++11", "-pthread", "-g"]
env["CPPPATH"] = [Dir("src")]
env["CPPDEFINES"] = ["_WEBSOCKETPP_CPP11_STL_"]

if "PREFIX" in env :
    env["CPPPATH"] += ["%s/%s" % (env["PREFIX"], "/include")]

if "WEBSOCKETPP" in env :
    env["CPPPATH"] += [env["WEBSOCKETPP"]]

plugin = env.SharedLibrary(
    target = "websocket-notifier",
    source = ["src/server.cpp", "src/plugin.cpp"]
)

env.Alias("install", env.Install("$PREFIX/lib/zeppelin/plugins", plugin))
