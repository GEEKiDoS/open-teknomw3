#include <std_include.hpp>
#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/cryptography.hpp>

#include "game/demonware/services/bdLSGHello.hpp"       // 7
#include "game/demonware/services/bdStorage.hpp"        // 10
#include "game/demonware/services/bdDediAuth.hpp"       // 12
#include "game/demonware/services/bdTitleUtilities.hpp" // 12
#include "game/demonware/services/bdDML.hpp"            // 27
#include "game/demonware/services/bdDediRSAAuth.hpp"    // 26
#include "game/demonware/services/bdSteamAuth.hpp"      // 28

#include "game/game.hpp"
#include "dw.hpp"


namespace demonware
{
	namespace io
	{
		int __stdcall send_to(const SOCKET s, const char* buf, const int len, const int flags, const sockaddr* to,
			const int tolen)
		{
			if (tolen == sizeof(sockaddr_in))
			{
				const auto in_addr = reinterpret_cast<const sockaddr_in*>(to);
				const auto server = find_stun_server_by_address(in_addr->sin_addr.s_addr);
				if (server) return server->send(s, buf, len, to, tolen);
			}

			return sendto(s, buf, len, flags, to, tolen);
		}

		int __stdcall recv_from(const SOCKET s, char* buf, const int len, const int flags, sockaddr* from, int* fromlen)
		{
			auto res = recv_datagam_packet(s, buf, len, from, fromlen);
			if (res != 0) return res;

			res = recvfrom(s, buf, len, flags, from, fromlen);

			return res;
		}

		int __stdcall send(const SOCKET s, const char* buf, const int len, const int flags)
		{
			auto server = find_server_by_socket(s);
			if (server) return server->send(buf, len);

			return ::send(s, buf, len, flags);
		}

		int __stdcall recv(const SOCKET s, char* buf, const int len, const int flags)
		{
			auto server = find_server_by_socket(s);
			if (server)
			{
				const auto blocking = is_blocking_socket(s, TCP_BLOCKING);

				int result;
				do
				{
					result = server->recv(buf, len);
					if (blocking && result < 0) std::this_thread::sleep_for(1ms);
				} while (blocking && result < 0);

				if (!blocking && result < 0)
				{
					WSASetLastError(WSAEWOULDBLOCK);
				}

				return result;
			}

			return ::recv(s, buf, len, flags);
		}

		int __stdcall connect(const SOCKET s, const sockaddr* addr, const int len)
		{
			if (len == sizeof(sockaddr_in))
			{
				const auto* in_addr = reinterpret_cast<const sockaddr_in*>(addr);
				if (link_socket(s, in_addr->sin_addr.s_addr)) return 0;
			}

			return ::connect(s, addr, len);
		}

		int __stdcall close_socket(const SOCKET s)
		{
			remove_blocking_socket(s);
			unlink_socket(s);
			return closesocket(s);
		}

		int __stdcall ioctl_socket(const SOCKET s, const long cmd, u_long* argp)
		{
			if (static_cast<unsigned long>(cmd) == (FIONBIO))
			{
				set_blocking_socket(s, *argp == 0);
			}

			return ioctlsocket(s, cmd, argp);
		}

		hostent* __stdcall get_host_by_name(char* name)
		{
			static std::mutex mutex;
			std::lock_guard<std::mutex> _(mutex);

			unsigned long addr = 0;
			const auto server = find_server_by_name(name);
			if (server) addr = server->get_address();

			const auto stun_server = find_stun_server_by_name(name);
			if (stun_server) addr = stun_server->get_address();

			if (server || stun_server)
			{
				static in_addr address;
				address.s_addr = addr;

				static in_addr* addr_list[2];
				addr_list[0] = &address;
				addr_list[1] = nullptr;

				static hostent host;
				host.h_name = name;
				host.h_aliases = nullptr;
				host.h_addrtype = AF_INET;
				host.h_length = sizeof(in_addr);
				host.h_addr_list = reinterpret_cast<char**>(addr_list);

				return &host;
			}

#pragma warning(push)
#pragma warning(disable: 4996)

			// ReSharper disable once CppDeprecatedEntity
			return gethostbyname(name);

#pragma warning(pop)
		}
	}

	namespace
	{
		bool terminate_;
		std::thread message_thread_;
		std::recursive_mutex server_mutex_;

		uint8_t encryption_key_[24];
		uint8_t decryption_key_[24];

		std::map<SOCKET, bool> blocking_sockets_;
		std::map<SOCKET, std::shared_ptr<service_server>> socket_links_;
		std::map<unsigned long, std::shared_ptr<service_server>> servers_;
		std::map<unsigned long, std::shared_ptr<stun_server>> stun_servers_;
		std::map<SOCKET, std::queue<std::pair<std::string, std::string>>> datagram_packets_;

		void server_thread()
		{
			terminate_ = false;
			while (!terminate_)
			{
				std::unique_lock lock(server_mutex_);

				for (auto& server : servers_)
				{
					server.second->run_frame();
				}

				lock.unlock();

				std::this_thread::sleep_for(50ms);
			}
		}

		void bd_logger_stub(int /*type*/, const char* /*channelName*/, const char*, const char* /*file*/, const char* function, unsigned int /*line*/, const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			_vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			printf("%s: %s\n", function, buffer);

			va_end(ap);
		}
	}

	std::shared_ptr<service_server> find_server_by_name(const std::string& name)
	{
		std::lock_guard _(server_mutex_);
		return find_server_by_address(utils::cryptography::jenkins_one_at_a_time::compute(name));
	}

	std::shared_ptr<service_server> find_server_by_address(const unsigned long address)
	{
		std::lock_guard _(server_mutex_);

		const auto server = servers_.find(address);
		if (server != servers_.end())
		{
			return server->second;
		}

		return std::shared_ptr<service_server>();
	}

	std::shared_ptr<stun_server> find_stun_server_by_name(const std::string& name)
	{
		std::lock_guard _(server_mutex_);
		return find_stun_server_by_address(utils::cryptography::jenkins_one_at_a_time::compute(name));
	}

	std::shared_ptr<stun_server> find_stun_server_by_address(const unsigned long address)
	{
		std::lock_guard _(server_mutex_);

		const auto server = stun_servers_.find(address);
		if (server != stun_servers_.end())
		{
			return server->second;
		}

		return std::shared_ptr<stun_server>();
	}

	std::shared_ptr<service_server> find_server_by_socket(const SOCKET s)
	{
		std::lock_guard _(server_mutex_);

		const auto server = socket_links_.find(s);
		if (server != socket_links_.end())
		{
			return server->second;
		}

		return std::shared_ptr<service_server>();
	}

	bool link_socket(const SOCKET s, const unsigned long address)
	{
		std::lock_guard _(server_mutex_);

		const auto server = find_server_by_address(address);
		if (!server) return false;

		socket_links_[s] = server;
		return true;
	}

	void unlink_socket(const SOCKET sock)
	{
		std::lock_guard _(server_mutex_);

		const auto server = socket_links_.find(sock);
		if (server != socket_links_.end())
		{
			socket_links_.erase(server);
		}

		const auto dgram_packets = datagram_packets_.find(sock);
		if (dgram_packets != datagram_packets_.end())
		{
			datagram_packets_.erase(dgram_packets);
		}
	}

	int recv_datagam_packet(const SOCKET s, char* buf, const int len, sockaddr* from, int* fromlen)
	{
		std::unique_lock lock(server_mutex_);

		auto queue = datagram_packets_.find(s);
		if (queue != datagram_packets_.end())
		{
			const auto blocking = is_blocking_socket(s, UDP_BLOCKING);

			lock.unlock();
			while (blocking && queue->second.empty())
			{
				std::this_thread::sleep_for(1ms);
			}
			lock.lock();

			if (!queue->second.empty())
			{
				auto packet = queue->second.front();
				queue->second.pop();

				*fromlen = INT(packet.first.size());
				std::memcpy(from, packet.first.data(), *fromlen);

				const int size = std::min(len, INT(packet.second.size()));
				std::memcpy(buf, packet.second.data(), size);

				return size;
			}

			WSASetLastError(WSAEWOULDBLOCK);
			return -1;
		}

		return 0;
	}

	void send_datagram_packet(const SOCKET s, const std::string& data, const sockaddr* to, const int tolen)
	{
		std::lock_guard _(server_mutex_);
		datagram_packets_[s].push({ std::string(LPSTR(to), tolen), data });
	}

	bool is_blocking_socket(const SOCKET s, const bool def)
	{
		std::lock_guard _(server_mutex_);

		if (blocking_sockets_.find(s) != blocking_sockets_.end())
		{
			return blocking_sockets_[s];
		}

		return def;
	}

	void remove_blocking_socket(const SOCKET s)
	{
		std::lock_guard _(server_mutex_);

		const auto entry = blocking_sockets_.find(s);
		if (entry != blocking_sockets_.end())
		{
			blocking_sockets_.erase(entry);
		}
	}

	void set_blocking_socket(const SOCKET s, const bool blocking)
	{
		std::lock_guard _(server_mutex_);
		blocking_sockets_[s] = blocking;
	}

	uint8_t* get_key(const bool encrypt)
	{
		return encrypt ? encryption_key_ : decryption_key_;
	}

	void set_key(const bool encrypt, uint8_t* key)
	{
		static_assert(sizeof encryption_key_ == sizeof decryption_key_);
		std::memcpy(encrypt ? encryption_key_ : decryption_key_, key, sizeof encryption_key_);
	}

	std::shared_ptr<stun_server> register_stun_server(const std::string& name)
	{
		std::lock_guard _(server_mutex_);
		auto server = std::make_shared<stun_server>(name);
		stun_servers_[server->get_address()] = server;
		return server;
	}

	template <typename... Args>
	std::shared_ptr<service_server> register_server(Args ... args)
	{
		std::lock_guard _(server_mutex_);
		auto server = std::make_shared<service_server>(args...);
		servers_[server->get_address()] = server;
		return server;
	}

	class component final : public component_interface
	{
	public:
		component()
		{
			register_stun_server("mw3-stun.us.demonware.net");
			register_stun_server("mw3-stun.eu.demonware.net");
			register_stun_server("stun.jp.demonware.net");
			register_stun_server("stun.au.demonware.net");
			register_stun_server("stun.eu.demonware.net");
			register_stun_server("stun.us.demonware.net");

			auto lsg_server = register_server("mw3-pc-lobby.prod.demonware.net");
			auto auth_server = register_server("mw3-pc-auth.prod.demonware.net");

			auth_server->register_service<bdDediAuth>();
			auth_server->register_service<bdSteamAuth>();
			auth_server->register_service<bdDediRSAAuth>();

			lsg_server->register_service<bdLSGHello>();
			lsg_server->register_service<bdStorage>();
			lsg_server->register_service<bdTitleUtilities>();
			lsg_server->register_service<bdDML>();
		}

		void post_load() override
		{
			message_thread_ = std::thread(server_thread);

			utils::hook::jump(SELECT_VALUE(0x6EA960, 0x0), bd_logger_stub);
		}

		void* load_import(const std::string& library, const std::string& function) override
		{
			if (library == "WS2_32.dll")
			{
				if (function == "#3") return io::close_socket;
				if (function == "#4") return io::connect;
				if (function == "#10") return io::ioctl_socket;
				if (function == "#16") return io::recv;
				if (function == "#17") return io::recv_from;
				if (function == "#19") return io::send;
				if (function == "#20") return io::send_to;
				if (function == "#52") return io::get_host_by_name;
			}

			return nullptr;
		}

		void pre_destroy() override
		{
			std::lock_guard _(server_mutex_);

			terminate_ = true;
			if (message_thread_.joinable())
			{
				message_thread_.join();
			}

			servers_.clear();
			stun_servers_.clear();
			socket_links_.clear();
			blocking_sockets_.clear();
			datagram_packets_.clear();
		}
	};
}

REGISTER_COMPONENT(demonware::component)
