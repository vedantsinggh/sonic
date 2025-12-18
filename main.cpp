#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <random>
#include <limits>

int main(void){
	std::string folder = "./music/";
	std::filesystem::path path{folder};
	std::random_device rd;
	std::mt19937 gen(rd());

	int total_album = 0;
	if(std::filesystem::exists(path) && std::filesystem::is_directory(path)){
		std::cout << "All available albums:\n";

		for(const auto& file : std::filesystem::directory_iterator(path)){
			const std::string f = file.path();
			std::cout << total_album <<  ". " <<f.substr(folder.size()) << std::endl;
			total_album++;
		}
	}

	std::cout << "Select ablum number from 0 to " << total_album - 1 << std::endl;

	int selected_choice = 0;
	std::cin >> selected_choice;
	std::string music_folder;
	std::string music_file;

	int temp = 0;
	for(const auto& file : std::filesystem::directory_iterator(path)){
		if (temp == selected_choice){
			const std::filesystem::path f = file.path();
			std::cout << "Playing: " << f.filename().string() << std::endl;
			music_folder = f;
			break;
		}
		temp++;
	}

	int total_music_count = 0;
	for(const auto& file : std::filesystem::directory_iterator(music_folder)){
		const std::filesystem::path f = file.path();
		std::cout << total_music_count <<". " << f.stem().string() << std::endl;
		total_music_count++;
	}

	std::uniform_int_distribution<int> distrib(0, total_music_count);
	int rand_music = distrib(gen);

	temp = 0;
	for(const auto& file : std::filesystem::directory_iterator(music_folder)){
		if (temp == rand_music){
			const std::filesystem::path f = file.path();
			std::cout << "Playing Music: " << f.stem().string() << std::endl;
			music_file = f;
			break;
		}
		temp++;
	}

	std::string cmd = "ffplay -nodisp -loglevel quiet -autoexit \"" + music_file + "\"";

	int result = std::system(cmd.c_str());

	if (result){
		std::cerr << "Failed!" << std::endl;
	}else{
		std::cout << "Finished!" << std::endl;
	}

	return 0;
}
