#ifndef WEBSOCKET_NOTIFIER_SERVER_H_INCLUDED
#define WEBSOCKET_NOTIFIER_SERVER_H_INCLUDED

#include <zeppelin/plugin/plugin.h>
#include <zeppelin/player/controller.h>
#include <zeppelin/player/eventlistener.h>

#include <jsoncpp/json/value.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/server.hpp>

class Server : public zeppelin::plugin::Plugin,
	       public zeppelin::player::EventListener
{
    public:
	static std::shared_ptr<Server> create(const std::shared_ptr<zeppelin::player::Controller>& ctrl);

	std::string getName() const override
	{ return "websocket-notifier"; }

	void start(const Json::Value& config, zeppelin::plugin::PluginManager& pm) override;
	void stop() override;

	void started() override;
	void paused() override;
	void stopped() override;
	void positionChanged() override;
	void songChanged() override;
	void queueChanged() override;
	void volumeChanged() override;

    private:
	Server(const std::shared_ptr<zeppelin::player::Controller>& ctrl);

	void run();

	void onOpen(websocketpp::connection_hdl hdl);
	void onClose(websocketpp::connection_hdl hdl);

	void broadcast(const std::string& data);

    private:
	std::shared_ptr<zeppelin::player::Controller> m_ctrl;

	websocketpp::lib::mutex m_mutex;
	std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> m_connections;

	typedef websocketpp::server<websocketpp::config::asio> WebsocketServer;
	WebsocketServer m_websocketServer;

	std::unique_ptr<websocketpp::lib::thread> m_worker;

	std::weak_ptr<Server> m_selfRef;
};

#endif
