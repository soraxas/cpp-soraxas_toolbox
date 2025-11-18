/*
 * MIT License
 *
 * Copyright (c) 2019-2025 Tin Yiu Lai (@soraxas)
 *
 * This file is part of the project soraxas_toolbox, a collections of utilities
 * for developing c++ applications.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <iostream>
#include <string>

// Windows
#if defined(_WIN32)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef SSIZE_T ssize_t;

// Linux
#else

#include <arpa/inet.h>  // This contains inet_addr
#include <sys/socket.h>

#include <unistd.h>  // This contains close

#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
typedef int SOCKET;
#endif

#include "simple_logger.h"

#include <atomic>
#include <functional>
#include <thread>

namespace simple_socket
{
using namespace simple_logger;

#ifdef WIN32
#define CLOSE_SOCKET(socket) closesocket(socket)
#define SHUTDOWN_SOCKET(socket) shutdown(socket, SD_BOTH)
#define LOG_WIN_SOCKET_ERROR() log(LOG_DEBUG) << WSAGetLastError() << std::endl
#else
#define CLOSE_SOCKET(socket) close(socket)
#define SHUTDOWN_SOCKET(socket) shutdown(socket, SHUT_RDWR)
#define LOG_WIN_SOCKET_ERROR()                                                                     \
    while (false)                                                                                  \
    {                                                                                              \
    }
#endif

// These could also be enums
enum SocketStatus
{
    socket_bind_err = 3,
    socket_accept_err = 4,
    connection_err = 5,
    message_send_err = 6,
    receive_err = 7
};

////////////////////////////////////////

class Socket
{
public:
    enum class SocketType
    {
        TYPE_STREAM = SOCK_STREAM,
        TYPE_DGRAM = SOCK_DGRAM
    };

protected:
    explicit Socket(const SocketType socket_type) : m_socket(), m_addr()
    {
#ifdef WIN32
        // Initialize the WSDATA if no socket instance exists
        if (!s_count)
        {
            WSADATA wsa;
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
            {
                throw std::runtime_error("Error initializing Winsock " + WSAGetLastError());
            }
        }
#endif

        // Create the socket handle
        m_socket = socket(AF_INET, static_cast<int>(socket_type), 0);
        if (m_socket == INVALID_SOCKET)
        {
            throw std::runtime_error("Could not create socket");
        }

        m_addr.sin_family = AF_INET;

#ifdef WIN32
        ++s_count;
#endif
    }

    SOCKET m_socket;
    sockaddr_in m_addr;

    void set_port(u_short port)
    {
        m_addr.sin_port = htons(port);
    }

    int set_address(const std::string &ip_address)
    {
        return inet_pton(AF_INET, ip_address.c_str(), &m_addr.sin_addr);
    }

#ifdef WIN32
    ~Socket()
    {
        if (!--s_count)
        {
            WSACleanup();
        }
    }
#else

    ~Socket() = default;

#endif

private:
#ifdef WIN32
    // Number of sockets is tracked to call WSACleanup on Windows
    static int s_count;
#endif
};

#ifdef WIN32
// Initialize s_count on windows
int Socket::s_count{0};
#endif

////////////////////////////////////////

class UDPClient : public Socket
{
public:
    UDPClient(u_short port = 8000, const std::string &ip_address = "127.0.0.1");

    ssize_t send_message(const std::string &message);
};

////////////////////////////////////////

class UDPServer : public Socket
{
public:
    UDPServer(u_short port = 8000, const std::string &ip_address = "0.0.0.0");

    int socket_bind();

    void listen();
};

////////////////////////////////////////

class TCPClient : public Socket
{
public:
    TCPClient(u_short port = 8000, const std::string &ip_address = "127.0.0.1");

    int make_connection();

    int send_message(const std::string &message);
};

////////////////////////////////////////

class TCPServer : public Socket
{
public:
    TCPServer(u_short port, const std::string &ip_address = "0.0.0.0")
      : Socket(SocketType::TYPE_STREAM)
    {
        set_port(port);
        set_address(ip_address);
        log(LOG_DEBUG) << "TCP Server created." << std::endl;
    };

    int socket_bind()
    {
        int status = accept_new_client();
        if (status != 0)
        {
            CLOSE_SOCKET(m_socket);
            return status;
        }

        auto msg = receive_msg();

        log(LOG_DEBUG) << "Incoming message is:\n" << msg << std::endl;
        log(LOG_DEBUG) << "Message length was: " << msg.size() << std::endl;
        //            std::string message = "Your message has been received client\n";
        //            size_t message_length = message.length();
        //            send(new_socket, message.c_str(), message_length, 0);

        CLOSE_SOCKET(m_socket);
        return 0;
    }

protected:
    int accept_new_client()
    {
        log(LOG_DEBUG) << "TCP Socket waiting for incoming connections..." << std::endl;

        socklen_t client_size = sizeof(sockaddr_in);
        sockaddr_in client;

        connected_socket = accept(m_socket, reinterpret_cast<sockaddr *>(&client), &client_size);
        if (connected_socket == INVALID_SOCKET)
        {
            log(LOG_DEBUG) << "TCP Socket accept error" << std::endl;
            LOG_WIN_SOCKET_ERROR();
            //                throw std::runtime_error("socket accept error");
            return socket_accept_err;
        }

        log(LOG_DEBUG) << "Connection accepted from IP address " << inet_ntoa(client.sin_addr)
                       << " on port " << ntohs(client.sin_port) << std::endl;
        return 0;
    }

    std::string receive_msg() const
    {
        /**
         * This should run after accepting a client
         */
        char message_buffer[512];
        ssize_t recv_len = recv(connected_socket, message_buffer, sizeof(message_buffer), 0);
        // cut off non-used buffer
        std::string input_str(message_buffer, message_buffer + recv_len);
        return input_str;
    };

    SOCKET connected_socket;
};

////////////////////////////////////////

class TCPServerCallback : public TCPServer
{
public:
    TCPServerCallback(u_short port, const std::string &ip_address = "0.0.0.0")
      : TCPServer(port, ip_address), is_shutdowning(false)
    {
    }

    void bind_callback(std::function<void(const std::string &)> callback)
    {
        m_callback = callback;

        log(LOG_DEBUG) << "TCP Server binding to socket " << m_socket << std::endl;
        if (bind(m_socket, reinterpret_cast<sockaddr *>(&m_addr), sizeof(m_addr)) == SOCKET_ERROR)
        {
            std::cerr << "TCP Socket Bind error." << std::endl;
            LOG_WIN_SOCKET_ERROR();
            //                return socket_bind_err;
            throw std::runtime_error("socket bind error");
        }

        log(LOG_DEBUG) << "TCP Socket Bound." << std::endl;
        listen(m_socket, 3);

        m_thread = std::make_unique<std::thread>(&TCPServerCallback::loop, this);
        log(LOG_DEBUG) << "TCP Server created." << std::endl;
    };

    void close_socket()
    {
        if (!is_shutdowning)
        {
            log(LOG_DEBUG) << "closing socket" << std::endl;

            is_shutdowning.store(true);
            if (SHUTDOWN_SOCKET(m_socket))
            {
                std::cerr << "Error in shutdowning socket" << std::endl;
            }

            CLOSE_SOCKET(connected_socket);
            if (CLOSE_SOCKET(m_socket))
            {
                std::cerr << "Error in closing socket" << std::endl;
            }
            m_thread->join();
        }
    }

    ~TCPServerCallback()
    {
        log(LOG_DEBUG) << "destructor" << std::endl;
        close_socket();
    }

private:
    void socket_bind(){};

    void loop()
    {
        while (true)
        {
            //////////////
            if (accept_new_client())
            {
                if (is_shutdowning)
                    return;
                throw std::runtime_error("socket accept error");
            }
            while (true)
            {
                if (is_shutdowning)
                    return;
                m_callback(receive_msg());
            }
        }
    }

private:
    std::unique_ptr<std::thread> m_thread;
    std::function<void(const std::string &)> m_callback;
    std::atomic<bool> is_shutdowning;
};

////////////////////////////////////////

UDPClient::UDPClient(u_short port, const std::string &ip_address) : Socket(SocketType::TYPE_DGRAM)
{
    set_address(ip_address);
    set_port(port);
    log(LOG_DEBUG) << "UDP Client created." << std::endl;
};

ssize_t UDPClient::send_message(const std::string &message)
{
    size_t message_length = message.length();
    return sendto(
        m_socket, message.c_str(), message_length, 0, reinterpret_cast<sockaddr *>(&m_addr),
        sizeof(m_addr)
    );
};

UDPServer::UDPServer(u_short port, const std::string &ip_address) : Socket(SocketType::TYPE_DGRAM)
{
    set_port(port);
    set_address(ip_address);
    log(LOG_DEBUG) << "UDP Server created." << std::endl;
}

int UDPServer::socket_bind()
{
    if (bind(m_socket, reinterpret_cast<sockaddr *>(&m_addr), sizeof(m_addr)) == SOCKET_ERROR)
    {
        log(LOG_DEBUG) << "UDP Bind error." << std::endl;
        LOG_WIN_SOCKET_ERROR();
        return socket_bind_err;
    }
    log(LOG_DEBUG) << "UDP Socket Bound." << std::endl;
    return 0;
}

void UDPServer::listen()
{
    sockaddr_in client;
    char client_ip[INET_ADDRSTRLEN];
    socklen_t slen = sizeof(client);
    char message_buffer[512];
    log(LOG_DEBUG) << "Waiting for data..." << std::endl;

    while (true)
    {
        // This is a blocking call
        ssize_t recv_len = recvfrom(
            m_socket, message_buffer, sizeof(message_buffer), 0,
            reinterpret_cast<sockaddr *>(&client), &slen
        );
        if (recv_len == SOCKET_ERROR)
        {
            log(LOG_DEBUG) << "Receive Data error." << std::endl;
            LOG_WIN_SOCKET_ERROR();
        }
        log(LOG_DEBUG) << "Received packet from "
                       << inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN) << ':'
                       << ntohs(client.sin_port) << std::endl;
        log(LOG_DEBUG) << message_buffer << std::endl;
    }
}

TCPClient::TCPClient(u_short port, const std::string &ip_address) : Socket(SocketType::TYPE_STREAM)
{
    set_address(ip_address);
    set_port(port);
    log(LOG_DEBUG) << "TCP client created." << std::endl;
}

int TCPClient::make_connection()
{
    log(LOG_DEBUG) << "Connecting" << std::endl;
    if (connect(m_socket, reinterpret_cast<sockaddr *>(&m_addr), sizeof(m_addr)) < 0)
    {
        log(LOG_DEBUG) << "Connection error" << std::endl;
        return connection_err;
    }
    log(LOG_DEBUG) << "connected" << std::endl;

    return 0;
}

int TCPClient::send_message(const std::string &message)
{
    char server_reply[2000];
    size_t length = message.length();

    if (send(m_socket, message.c_str(), length, 0) < 0)
    {
        log(LOG_DEBUG) << "Send failed" << std::endl;
        return message_send_err;
    }
    else
    {
        log(LOG_DEBUG) << "Data sent" << std::endl;
    }

    if (recv(m_socket, server_reply, 2000, 0) == SOCKET_ERROR)
    {
        log(LOG_DEBUG) << "Receive Failed" << std::endl;
        return receive_err;
    }
    else
    {
        log(LOG_DEBUG) << server_reply << std::endl;
    }

    return 0;
}

}  // namespace simple_socket