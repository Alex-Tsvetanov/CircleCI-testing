#pragma once 

#include <Flask/base.hpp>
#include <map>
#include <set>
#include <string>
#include <functional>
#include <filesystem>

#include <utility>
#include <thread>
#include <chrono>

#include <ios>

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

namespace Flask
{
	class flask;

	struct respond_manager
	{
		public:
			respond_manager ()
			{
			}
		private:
			//Flask::Function::callable f;
			std::function <response (path_vars)> f;
		public:
			respond_manager (std::function <response (path_vars)> l)
			{
				set (l);
			}
			
			respond_manager (std::function <response ()> l)
			{
				set (l);
			}

			respond_manager& set (std::function <response (path_vars)> l)
			{
				this->f = l;
				return *this;
			}

			respond_manager& set (std::function <response ()> l)
			{
				this->f = [l](__attribute__((unused)) path_vars _) -> response { return l (); };
				return *this;
			}

			response call (const path_vars& vars) const
			{
				return f (vars);
			}
		private:
			friend class flask;
	};

	class flask
	{
		private:
			struct cmp { bool operator () (const auto& a, const auto& b) const { return a.first.size () < b.first.size (); }};
			std::map < 
				std::pair < std::vector < std::string >, // var name
							std::regex >,                // regex 
				respond_manager,
				cmp> routes;
			std::set < std::filesystem::path > static_folders;

			static std::pair < std::vector < std::string >, // var name
							std::regex >                // regex 
			                             convert_path_to_regex (std::string str)
			{
				std::vector < std::string > v;
				std::string format = "^";
				for (size_t i = 0 ; i < str.size () ; i ++)
				{
					if (str [i] == '{')
					{
						std::string var_name;
						for (i ++ ; i < str.size () and str [i] != '}' ; i ++)
						{
							if (str [i] == ':') break;
							var_name += str [i];
						}
						v.push_back (var_name);
						format += "(";
						for (i ++ ; i < str.size () and str [i] != '}' ; i ++)
						{
							if (str [i] == ':') break;
							format += str [i];
						}
						format += ")";
					}
					else
						format += str [i];
				}
				format += "$";
				//std::cout << format << std::endl;
				return {v, std::regex (format)};
			}

		public:

			flask& set_static (std::string path)
			{
				std::filesystem::path p = std::filesystem::relative (path);
				static_folders.insert (p);
				return *this;
			}

			flask& route (std::string path, respond_manager res)
			{
				auto x = convert_path_to_regex (path);
				if (routes.find (x) == routes.end ())
					routes [x] = res;
				else // rewriting path
					routes [x] = res;
				return *this;
			}

			flask& route (std::string path, std::function <response ()> res)
			{
				auto x = convert_path_to_regex (path);
				if (routes.find (x) == routes.end ())
					routes [x] = res;
				else // rewriting path
					routes [x] = res;
				return *this;
			}

			flask& route (std::string path, std::function <response (path_vars)> res)
			{
				auto x = convert_path_to_regex (path);
				if (routes.find (x) == routes.end ())
					routes [x] = res;
				else // rewriting path
					routes [x] = res;
				return *this;
			}
			
			response respond (const request& req) const
			{
				std::cout << "Requested: " << req.uri << std::endl;
				std::smatch pieces_match;

				for (const auto &s : routes)
				{
					std::regex pieces_regex(s.first.second);
					if (std::regex_match (req.uri, pieces_match, pieces_regex)) {
						path_vars params;
						for (size_t i = 0; i < pieces_match.size(); ++i) {
							std::ssub_match sub_match = pieces_match[i];
							std::string piece = sub_match.str();
							if (i > 0)
								params [s.first.first [i - 1]] = piece;
						}
						return s.second.call (params); 
					}
				}
				return response (status_line ("1.1", "404"));
			}

			response respond (const char* p) const
			{
				std::string path (p);
				std::cout << "Requested: " << path << std::endl;
				std::smatch pieces_match;

				for (const auto &s : routes)
				{
					std::regex pieces_regex(s.first.second);
					if (std::regex_match (path, pieces_match, pieces_regex)) {
						path_vars params;
						for (size_t i = 0; i < pieces_match.size(); ++i) {
							std::ssub_match sub_match = pieces_match[i];
							std::string piece = sub_match.str();
							if (i > 0)
								params [s.first.first [i - 1]] = piece;
						}
						return s.second.call (params); 
					}
				}
				return response (status_line ("404"));
			}

			void run (unsigned short PORT, const unsigned char threads)
			{
				int server_fd, valread; 
				struct sockaddr_in address; 
				int opt = 1; 
				int addrlen = sizeof(address); 
				   
				// Creating socket file descriptor 
				if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
				{ 
					perror("socket failed"); 
					throw std::ios_base::failure ("Socket failed"); 
				} 
				   
				// Forcefully attaching socket to the port 8080 
				if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
															  &opt, sizeof(opt))) 
				{ 
					perror("setsockopt"); 
					throw std::ios_base::failure ("Setsockopt"); 
				} 
				address.sin_family = AF_INET; 
				address.sin_addr.s_addr = INADDR_ANY; 
				address.sin_port = htons (PORT); 
				   
				// Forcefully attaching socket to the port 8080 
				if (bind(server_fd, (struct sockaddr *)&address,  
											 sizeof(address))<0) 
				{ 
					perror("bind failed"); 
					throw std::ios_base::failure ("Socket failed"); 
				} 
				if (listen(server_fd, SOMAXCONN) < 0) 
				{ 
					perror("listen"); 
					throw std::ios_base::failure ("Listen"); 
				} 

				auto accept_req = [&] (int new_socket, size_t id) {
					char buffer[8196] = {0}; 
					valread = read (new_socket, buffer, 8196); 
					printf("Accept:\n%s\n--------- BUFFER ------------\n"); 

					if (strlen (buffer) == 0)
					{
						std::cout << "================= FACK YOU ===============" << std::endl;
						this->busy [id] = false;
						return ;
					}

					response res;
					request r (buffer);
					std::cout << r.to_string () << std::endl;
					printf("--------- REQUEST ------------\n",buffer); 

					res = this->respond (r);
					res.set_http (r.http);
					std::cout << res.to_string () << std::endl;
					printf("--------- REQUEST ------------\n",buffer); 
					char* msg = new char [res.to_string ().size () + 1];
					std::string s = res.to_string ();
					for (size_t i = 0 ; i < s.size () ; i ++)
					{
						msg [i] = s [i];
					}
					msg [s.size ()] = '\0';
					std::cout << msg << std::endl;
					printf("--------- REQUEST ------------\n",buffer); 
					send (new_socket, s.c_str (), s.size (), 0); 
					printf("Message sent\n"); 
					shutdown (new_socket, 2);
					//close (new_socket);
					this->busy [id] = false;
				};
				threads_ptr = new std::thread* [threads];
				busy = new bool [threads];
				for (size_t i = 0 ; i < threads ; i ++)
				{
					threads_ptr [i] = nullptr;
					busy [i] = false;
				}
				while (true)
				{
					int new_socket; 
					if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
									   (socklen_t*)&addrlen))<0) 
					{ 
						continue;
						perror("accept"); 
						throw std::ios_base::failure ("Accept"); 
					} 
					std::cout << "Received request on " << new_socket << std::endl;
					for (size_t i = 0 ; i < threads ; i ++)
						if (threads_ptr [i] == nullptr)
						{
							std::cout << "Create a thread... " << std::endl;
							threads_ptr [i] = new std::thread (accept_req, new_socket, i);
							std::cout << "Mark as busy core... " << std::endl;
							busy [i] = true;
							std::cout << "Mark as busy core... " << std::endl;
							threads_ptr [i]->detach ();
						}
						else if (busy [i] == false)
						{
							delete threads_ptr [i];
							threads_ptr [i] = new std::thread (accept_req, new_socket, i);
							busy [i] = true;
							threads_ptr [i]->detach ();
						}
				}
			}

		private:
			std::thread** threads_ptr;
			bool* busy;
	};
}
