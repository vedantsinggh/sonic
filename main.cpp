#include <iostream>
#include <cstdlib>
#include <string>

int main(void){
	std::string file = "music.wav";
	std::string cmd = "ffplay -nodisp -loglevel quiet -autoexit \"" + file + "\"";

	std::cout << "Playing" << std::endl;

	int result = std::system(cmd.c_str());

	if (result){
		std::cerr << "Failed!" << std::endl;
	}else{
		std::cout << "Finished!" << std::endl;
	}

	return 0;
}
