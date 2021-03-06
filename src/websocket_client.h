// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_WEBSOCKET_CLIENT_H_
#define SRC_WEBSOCKET_CLIENT_H_

#include <string>
#include <vector>
#include <ctime>

#include "Poco/Activity.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Logger.h"

#include "./types.h"
#include "./proxy.h"

namespace toggl {

typedef void (*WebSocketMessageCallback)(
    void *callback,
    std::string json);

class WebSocketClient {
 public:
    explicit WebSocketClient(
        const std::string websocket_url) :
    activity_(this, &WebSocketClient::runActivity),
    session_(0),
    req_(0),
    res_(0),
    ws_(0),
    on_websocket_message_(0),
    ctx_(0),
    websocket_url_(websocket_url),
    last_connection_at_(0),
    api_token_("") {}
    virtual ~WebSocketClient();

    virtual void Start(
        void *ctx,
        const std::string api_token,
        WebSocketMessageCallback on_websocket_message);
    virtual void Shutdown();

    void SetWebsocketURL(const std::string value) {
        websocket_url_ = value;
    }

 protected:
    void runActivity();

 private:
    error createSession();
    void authenticate();
    error poll();
    std::string parseWebSocketMessageType(const std::string json);
    error receiveWebSocketMessage(std::string *message);
    void deleteSession();

    Poco::Logger &logger() const {
        return Poco::Logger::get("websocket_client");
    }

    Poco::Activity<WebSocketClient> activity_;
    Poco::Net::HTTPSClientSession *session_;
    Poco::Net::HTTPRequest *req_;
    Poco::Net::HTTPResponse *res_;
    Poco::Net::WebSocket *ws_;
    WebSocketMessageCallback on_websocket_message_;
    void *ctx_;

    std::string websocket_url_;

    std::time_t last_connection_at_;

    std::string api_token_;

    Poco::Mutex mutex_;
};
}  // namespace toggl

#endif  // SRC_WEBSOCKET_CLIENT_H_
