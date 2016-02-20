#ifndef INCLUDE_IRC_HPP
#define INCLUDE_IRC_HPP
#include <string>
#include <map>
#include <atomic>

#include <libircclient/libircclient.h>

namespace irc
{
class Exception : std::exception
{
    public:
        Exception(int error_code)
        {
            error_code_ = error_code;
        }

        virtual const char* what() const noexcept override
        {
            return irc_strerror(error_code_);
        }
        int error_code()
        {
            return error_code_;
        }
    private:
        int error_code_;
};

class Connection
{
    public:
        Connection(const std::string& address, uint16_t port,
                   const std::string& user_name,
                   const std::string& nick_name,
                   const std::string& real_name,
                   const std::string& password) : Connection(address, port,
                                                             user_name,
                                                             nick_name,
                                                             real_name,
                                                             password.data())
        {}

        Connection(const std::string& address, uint16_t port,
                   const std::string& user_name,
                   const std::string& nick_name,
                   const std::string& real_name) : Connection(address, port,
                                                              user_name,
                                                              nick_name,
                                                              real_name,
                                                              nullptr)
        {}

        Connection(const std::string& address, uint16_t port,
                   const std::string& user_name,
                   const std::string& nick_name,
                   const std::string& real_name,
                   const char* password);

        ~Connection();

        void run();
        void terminate();
        void join(const std::string& channel)
        {
            join(channel, nullptr);
        }
        void join(const std::string& channel, const std::string& key)
        {
            join(channel, key.c_str());
        }
    private:
        void join(const std::string& channel, const char* key);
        
        void connect_handler(std::string server,
                             std::string message_of_the_day);
        void join_handler(std::string origin, std::string channel);
        void part_handler(std::string channel);

        struct Message
        {
            std::string channel;
            std::string sender;
            std::string content;
        };
        void channel_handler(Message message);

        static void connect_handler(irc_session_s* session,
                                    const char* command,
                                    const char* server,
                                    const char** data,
                                    unsigned count);
        static void nick_handler(irc_session_t* session,
                                 const char* command,
                                 const char* server,
                                 const char** data,
                                 unsigned count);
        static void quit_handler(irc_session_t* session,
                                 const char* command,
                                 const char* server,
                                 const char** data,
                                 unsigned);
        static void join_handler(irc_session_s* session,
                                 const char* command,
                                 const char* server,
                                 const char** data,
                                 unsigned count);
        static void part_handler(irc_session_t* session,
                                 const char* command,
                                 const char* server,
                                 const char** data,
                                 unsigned count);
        static void mode_handler(irc_session_t* session,
                                 const char* command,
                                 const char* server,
                                 const char** data,
                                 unsigned count);
        static void umode_handler(irc_session_t* session,
                                  const char* command,
                                  const char* server,
                                  const char** data,
                                  unsigned count);
        static void topic_handler(irc_session_t* session,
                                  const char* command,
                                  const char* server,
                                  const char** data,
                                  unsigned count);
        static void kick_handler(irc_session_t* session,
                                 const char* command,
                                 const char* server,
                                 const char** data,
                                 unsigned count);
        static void channel_handler(irc_session_s* session,
                                    const char* command,
                                    const char* server,
                                    const char** data,
                                    unsigned count);
        static void private_message_handler(irc_session_t* session,
                                    const char* command,
                                    const char* server,
                                    const char** data,
                                    unsigned count);
        static void notice_handler(irc_session_t* session,
                                   const char* command,
                                   const char* server,
                                   const char** data,
                                   unsigned count);
        static void channel_notice_handler(irc_session_t* sesson,
                                           const char* command,
                                           const char* origin,
                                           const char** data,
                                           unsigned count);
        static void invite_handler(irc_session_t* session,
                                   const char* command,
                                   const char* server,
                                   const char** data,
                                   unsigned count);
        static void ctcp_request_handler(irc_session_t* session,
                                         const char* command,
                                         const char* server,
                                         const char** data,
                                         unsigned count);
        static void ctcp_rep_handler(irc_session_t* session,
                                     const char* command,
                                     const char* server,
                                     const char** data,
                                     unsigned count);
        static void ctcp_action_handler(irc_session_t* session,
                                        const char* command,
                                        const char* server,
                                        const char** data,
                                        unsigned count);
        static void unknown_handler(irc_session_t* session,
                                    const char* command,
                                    const char* server,
                                    const char** data,
                                    unsigned count);
        static void numeric_handler(irc_session_t* session,
                                    unsigned code,
                                    const char* prefix,
                                    const char** data,
                                    unsigned int count);
        static irc_callbacks_t handler_;
        static std::map<irc_session_s*, Connection*> lookup_table_;

        irc_session_t* session_;
        std::atomic<bool> terminate_{false};
};
}
#endif
