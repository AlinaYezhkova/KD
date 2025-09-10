#pragma once

class IPeer;
class Message;

class MessageHandler {
   public:
    MessageHandler(){};
    void handle(IPeer& peer, const Message& msg);

   private:
    void handleBootstrapQuery(IPeer& peer, const Message& msg);
    void handleBootstrapReply(IPeer& peer, const Message& msg);

    void handleFindNodeQuery(IPeer& peer, const Message& msg);
    void handleFindNodeReply(IPeer& peer, const Message& msg);
};