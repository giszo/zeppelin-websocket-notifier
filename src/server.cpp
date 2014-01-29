#include "server.h"

#include <zeppelin/logger.h>

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
    broadcast("started");
}

// =====================================================================================================================
void Server::paused()
{
    broadcast("paused");
}

// =====================================================================================================================
void Server::stopped()
{
    broadcast("stopped");
}

// =====================================================================================================================
void Server::positionChanged()
{
    broadcast("position-changed");
}

// =====================================================================================================================
void Server::songChanged()
{
    broadcast("song-changed");
}

// =====================================================================================================================
void Server::queueChanged()
{
    broadcast("queue-changed");
}

// =====================================================================================================================
void Server::volumeChanged()
{
    broadcast("volume-changed");
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
void Server::broadcast(const std::string& data)
{
    websocketpp::lib::unique_lock<websocketpp::lib::mutex> lock(m_mutex);

    for (const auto& hdl : m_connections)
	m_websocketServer.send(hdl, data, websocketpp::frame::opcode::text);
}
