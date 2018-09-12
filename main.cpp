#include <Flask/flask.hpp>
#include <iostream>

int main ()
{
	Flask::flask app;
	app.set_static ("/static")
	   //EXTENDED_VERSION: .route ("/", Flask::respond_manager ().set ([]() {
	   //EXTENDED_VERSION:  	std::cout << "hello world\r\n" << std::endl;		
	   //EXTENDED_VERSION:  	return Flask::response (Flask::status_line ("200", "1.1"), {}, "<h1>I love you! I'm working</h1>");
	   //EXTENDED_VERSION: }))
	   //EXTENDED_VERSION: .route ("/users/{user:[a-zA-Z0-9]*}", Flask::respond_manager ().set ([](Flask::path_vars v) {
	   //EXTENDED_VERSION:  	for (auto& x : v)
	   //EXTENDED_VERSION:  		std::cout << x.first << " " << x.second << std::endl; 
	   //EXTENDED_VERSION:  	return Flask::response ();
	   //EXTENDED_VERSION: }))
	   //TODO: .route ("/contest/{id:int:[0-9]*}", Flask::respond_manager ().set ([](Flask::path_vars v) {
	   //TODO:  	for (auto& x : v)
	   //TODO:  		std::cout << x.first << " " << x.second << std::endl; 
	   //TODO:  	return Flask::response ();
	   //TODO: }))
	   //.route ("/users/{user:[a-zA-Z0-9]*}/{tab:.*}", Flask::respond_manager ().set ([](Flask::path_vars v) {
	   // 	for (auto& x : v)
	   // 		std::cout << x.first << " " << x.second << std::endl; 
	   // 	return Flask::response ();
	   //}))
	   .route ("/", []() {
			std::cout << "Home: returning" << std::endl;		
	     	return Flask::response (Flask::status_line ("200", "1.1"), {}, "<h1>I love you! I'm working</h1>");
	   })
	   .route ("/users/{user:[a-zA-Z0-9]*}", [](Flask::path_vars v) {
			for (auto& x : v)
				std::cout << x.first << " " << x.second << std::endl; 
			return Flask::response ();
	   })
	   //TODO: .route ("/contest/{id:int}", [](Flask::path_vars v) {
	   //TODO:  	for (auto& x : v)
	   //TODO:  		std::cout << x.first << " " << x.second << std::endl; 
	   //TODO:  	return Flask::response ();
	   //TODO: })
	   .route ("/users/{user:[a-zA-Z0-9]*}/{tab:.*}", [](Flask::path_vars v) {
			for (auto& x : v)
				std::cout << x.first << " " << x.second << std::endl; 
			return Flask::response ();
	   })
	;

	app.run (8888, std::thread::hardware_concurrency () - 1);

	app.respond ("/");
	std::cout << std::endl;
	app.respond ("/users/alexts");
	app.respond ("/users/alexts/dashboard");
}
