#pragma once 

#include <Flask/base.hpp>
#include <map>
#include <set>
#include <string>
#include <functional>
#include <filesystem>

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
				std::cout << format << std::endl;
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

			response respond (std::string path)
			{
				std::cout << "Requested: " << path << std::endl;
				std::smatch pieces_match;

				for (const auto &s : routes)
				{
					std::regex pieces_regex(s.first.second);
					if (std::regex_match (path, pieces_match, pieces_regex)) {
						path_vars params;
						//std::cout << path << '\n';
						for (size_t i = 0; i < pieces_match.size(); ++i) {
							std::ssub_match sub_match = pieces_match[i];
							std::string piece = sub_match.str();
							//std::cout << "  submatch " << i << ": " << piece << '\n';
							if (i > 0)
							{
								params [s.first.first [i - 1]] = piece;
							}
						}
						return s.second.call (params); 
					}
				}
				return response (status_line ("1.1", "404"));
			}

		protected:
	};
}
