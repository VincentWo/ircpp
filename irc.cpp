#include <iostream>
#include <cstring>

#include "irc.hpp"

namespace irc
{
Connection::Connection(const std::string& address, uint16_t port,
                       const std::string& user_name,
                       const std::string& nick_name,
                       const std::string& real_name,
                       const char* password)
                                      : session_{irc_create_session(&handler_)},
                                        terminate_{false}
{
    if(!session_)
    {
        std::cerr << "Couldn't create Session.\n"
                  << "sterror: " << irc_strerror(irc_errno(session_))
                  << std::endl;
        std::terminate();
    }
    if(irc_connect(session_,
                   address.data(), port,
                   password,
                   user_name.data(),
                   nick_name.data(),
                   real_name.data()))
    {
        std::cerr << "Couldn't establish Connection.\n"
                  << "strerror: " << irc_strerror(irc_errno(session_))
                  << std::endl;
        std::terminate();
    }
    lookup_table_.insert(std::make_pair(session_, this));
}
Connection::~Connection()
{
    std::cerr << "ERROR ~Connection not implemented\n";
    //std::terminate();
}
void Connection::run()
{
    while(!terminate_ && irc_is_connected(session_))
    {
        fd_set in_set;
        FD_ZERO(&in_set);
        fd_set out_set;
        FD_ZERO(&out_set);
        int max_fd = 0;

        irc_add_select_descriptors(session_,
                                   &in_set, &out_set,
                                   &max_fd);

        struct timeval tv = {250'000, 0};
        if(select(max_fd + 1, &in_set, &out_set, nullptr, &tv) < 0)
        {
            auto error_code = errno;
            std::cerr << "ERROR while calling select\n"
                      << "Error message: " << std::strerror(error_code) << '\n';
            std::terminate();
        }
        if(irc_process_select_descriptors(session_, &in_set, &out_set))
        {
            std::cerr << "ERROR while calling irc_process_seelct_descriptors\n"
                      << "Error message: "
                      << irc_strerror(irc_errno(session_)) << '\n';
            std::terminate();
        }
    }
}

void Connection::terminate()
{
    terminate_ = true;
}

void Connection::join(const std::string& channel, const char* key)
{
    if(irc_cmd_join(session_, channel.c_str(), key))
    {
        throw Exception{irc_errno(session_)};
    }
}

void Connection::connect_handler(irc_session_t* session, const char* command,
                                 const char* server,
                                 const char** data, unsigned count)
{
    auto user_name = data[0];
    auto message_of_the_day = data[1];
    lookup_table_[session]->connect_handler(server, message_of_the_day);
}
void Connection::nick_handler(irc_session_t* session, const char* command,
                              const char* server,
                              const char** data, unsigned count)
{
    std::cerr << "ERROR nick_handler not implemented\n";
    std::terminate();
}
void Connection::join_handler(irc_session_t* session, const char* command,
                              const char* user_name,
                              const char** data, unsigned count)
{
    auto channel = data[0];
    lookup_table_[session]->join_handler(user_name, channel);
}
void Connection::channel_handler(irc_session_t* session, const char* command,
                                 const char* sender,
                                 const char** data, unsigned count)
{
    auto channel = data[0];
    auto message = data[1];
    lookup_table_[session]->channel_handler({channel, sender, message});
}
void Connection::private_message_handler(irc_session_t* session,
                                         const char* command,
                                         const char* sender,
                                         const char** data, unsigned count)
{
    std::cerr << "ERROR private_message_handler not implemented\n";
    std::terminate();
}
void Connection::notice_handler(irc_session_t* session, const char* command,
                                const char* sender,
                                const char** data, unsigned count)
{
    std::cerr << "ERROR notice_handler not implemented\n";
    std::terminate();
}
void Connection::channel_notice_handler(irc_session_t* session, const char* cmd,
                                        const char* sender,
                                        const char** data, unsigned count)
{
    std::cerr << "ERROR channel_notice_handler not implemented\n";
    std::terminate();
}
void Connection::invite_handler(irc_session_t* session, const char* command,
                                const char* sender,
                                const char** data, unsigned count)
{
    std::cerr << "ERROR invite_handler not implemented\n";
    std::terminate();
}
void Connection::unknown_handler(irc_session_t* session, const char* command,
                                 const char* sender,
                                 const char** data, unsigned count)
{
    std::cerr << "ERROR unknown_handler not implemented\n";
    std::terminate();
}
void Connection::quit_handler(irc_session_s* session, const char* command,
                              const char* origin,
                              const char** data, unsigned count)
{
    std::cerr << "ERROR quit_handler not implemented\n";
    std::terminate();
}
void Connection::part_handler(irc_session_s* session, const char* command,
                              const char* sender,
                              const char** data, unsigned count)
{
    std::cerr << "ERROR part_handler not implemented\n";
    std::terminate();
}
void Connection::mode_handler(irc_session_s* session, const char* command,
                              const char* sender,
                              const char** data, unsigned count)
{
    std::cerr << "ERROR mode_handler not implemented\n";
    std::terminate();
}
void Connection::umode_handler(irc_session_s* session, const char* command,
                               const char* sender,
                               const char** data, unsigned count)
{
    std::cerr << "ERROR umode_handler not implemented\n";
    std::terminate();
}
void Connection::topic_handler(irc_session_s* session, const char* command,
                               const char* sender,
                               const char** data, unsigned count)
{
    std::cerr << "ERROR topic_handler not implemented\n";
    std::terminate();
}
void Connection::kick_handler(irc_session_s* session, char const* command,
                              const char* kicked_by,
                              const char** data, unsigned count)
{
    std::cerr << "ERROR kick_handler not implemented\n";
    std::terminate();
}
void Connection::numeric_handler(irc_session_s* session,
                                 unsigned code,
                                 const char* sender,
                                 const char** data, unsigned count)
{
    std::cout << "Code: " << code << '\n'
              << "Origin: " << sender << '\n'
              << "Data:\n";
    for(unsigned i = 0; i < count; ++i)
    {
        std::cout << '\t' << data[i] << '\n';
    }
    flush(std::cout);
}

void Connection::channel_handler(Message message)
{
    std::cout << "Received message " << message.content << '\n'
              << "from " << message.sender << '\n'
              << "in " << message.channel << std::endl;
}
void Connection::join_handler(std::string origin, std::string channel)
{
    std::cerr << "ERROR join_handler not implemented\n";
    std::terminate();
}
void Connection::connect_handler(std::string server,
                                 std::string message_of_the_day)
{
    std::cout << "Connected to " << server << '\n'
              << "Message of the day: " << message_of_the_day << std::endl;
}
irc_callbacks_t Connection::handler_ = {connect_handler,
                                        nick_handler,
                                        quit_handler,
                                        join_handler,
                                        part_handler,
                                        mode_handler,
                                        umode_handler,
                                        topic_handler,
                                        kick_handler,
                                        channel_handler,
                                        private_message_handler,
                                        notice_handler,
                                        channel_notice_handler,
                                        invite_handler,
                                        nullptr,//ctcp_request_handler,
                                        nullptr,//ctcp_rep_handler,
                                        nullptr,//ctcp_action_handler,
                                        unknown_handler,
                                        numeric_handler,
                                        nullptr,
                                        nullptr
                                        };
std::map<irc_session_t*, Connection*> Connection::lookup_table_ = {};
}
