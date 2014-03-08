#include "server.h"

#include <zeppelin/logger.h>

#include <jsoncpp/json/writer.h>

// =====================================================================================================================
std::shared_ptr<Server> Server::create(const std::shared_ptr<zeppelin::player::Controller>& ctrl)
{
    std::shared_ptr<Server> server(new Server(ctrl));
    server->m_selfRef = server;
    return server;
}

// =====================================================================================================================
Server::Server(const std::shared_ptr<zeppelin::player::Controller>& ctrl)
    : m_ctrl(ctrl)
{
    m_websocketServer.init_asio();
    m_websocketServer.set_reuse_addr(true);
    m_websocketServer.set_open_handler(websocketpp::lib::bind(&Server::onOpen, this, websocketpp::lib::placeholders::_1));
    m_websocketServer.set_close_handler(websocketpp::lib::bind(&Server::onClose, this, websocketpp::lib::placeholders::_1));
}

// =====================================================================================================================
void Server::start(const Json::Value& config, zeppelin::plugin::PluginManager& pm)
{
    // make sure we have a port configured
    if (!config.isMember("port") || !config["port"].isInt())
    {
	LOG("websocket-notifier: port not configured properly!");
	return;
    }

    // set websocket port
    m_websocketServer.listen(boost::asio::ip::tcp::v4(), config["port"].asInt());

    // register ourself as an event listener
    m_ctrl->addListener(m_selfRef.lock());

    // spawn a new thread for processing messages
    m_worker.reset(new websocketpp::lib::thread(websocketpp::lib::bind(&Server::run, this)));
}

// =====================================================================================================================
void Server::stop()
{
}

// =====================================================================================================================
void Server::started()
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "started";

    broadcast(resp);
}

// =====================================================================================================================
void Server::paused()
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "paused";

    broadcast(resp);
}

// =====================================================================================================================
void Server::stopped()
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "stopped";

    broadcast(resp);
}

// =====================================================================================================================
void Server::positionChanged(unsigned pos)
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "position-changed";
    resp["position"] = pos;

    broadcast(resp);
}

// =====================================================================================================================
void Server::songChanged(const std::vector<int>& idx)
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "song-changed";
    resp["index"] = Json::Value(Json::arrayValue);
    for (int i : idx)
	resp["index"].append(i);

    broadcast(resp);
}

// =====================================================================================================================
void Server::queueChanged()
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "queue-changed";

    broadcast(resp);
}

// =====================================================================================================================
void Server::volumeChanged(int level)
{
    Json::Value resp(Json::objectValue);
    resp["event"] = "volume-changed";
    resp["level"] = level;

    broadcast(resp);
}

// =====================================================================================================================
void Server::run()
{
    m_websocketServer.start_accept();

    try
    {
	m_websocketServer.run();
    }
    catch (const websocketpp::lib::error_code& e)
    {
	LOG("websocket-notifier: error: " << e);
    }
    catch (...)
    {
	LOG("websocket-notifier: unknown error");
    }
}

// =====================================================================================================================
void Server::onOpen(websocketpp::connection_hdl hdl)
{
    LOG("websocket-notifier: onOpen");

    websocketpp::lib::unique_lock<websocketpp::lib::mutex> lock(m_mutex);
    m_connections.insert(hdl);
}

// =====================================================================================================================
void Server::onClose(websocketpp::connection_hdl hdl)
{
    LOG("websocket-notifier: onClose");

    websocketpp::lib::unique_lock<websocketpp::lib::mutex> lock(m_mutex);
    m_connections.erase(hdl);
}

// =====================================================================================================================
void Server::broadcast(const Json::Value& data)
{
    std::string s = Json::FastWriter().write(data);

    websocketpp::lib::unique_lock<websocketpp::lib::mutex> lock(m_mutex);

    for (const auto& hdl : m_connections)
	m_websocketServer.send(hdl, s, websocketpp::frame::opcode::text);
}
