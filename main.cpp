#include <Flask/flask.hpp>
#include <iostream>

int main ()
{
	Flask::flask app;
	app.set_static ("/static")
	   .route ("/", Flask::respond_manager ().set ([]() {
			std::cout << "hello world\n\r" << std::endl;		
			return Flask::response ();
	   }))
	   .route ("/users/{user:[a-zA-Z0-9]*}", Flask::respond_manager ().set ([](Flask::path_vars v) {
			for (auto& x : v)
				std::cout << x.first << " " << x.second << std::endl; 
			return Flask::response ();
	   }))
	   //TODO: .route ("/contest/{id:int}", Flask::respond_manager ().set ([](Flask::path_vars v) {
	   //TODO:  	for (auto& x : v)
	   //TODO:  		std::cout << x.first << " " << x.second << std::endl; 
	   //TODO:  	return Flask::response ();
	   //TODO: }))
	   .route ("/users/{user:[a-zA-Z0-9]*}/{tab:.*}", Flask::respond_manager ().set ([](Flask::path_vars v) {
			for (auto& x : v)
				std::cout << x.first << " " << x.second << std::endl; 
			return Flask::response ();
	   }))

	   .route ("/", []() {
			std::cout << "hello world";		
			return Flask::response ();
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

	app.respond ("/");
	std::cout << std::endl;
	app.respond ("/users/alexts");
	app.respond ("/users/alexts/dashboard");
}
