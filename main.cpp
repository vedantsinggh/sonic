#include <iostream>
#include <filesystem>
#include <vector>
#include <random>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

class TerminalRawMode {
    termios old{};
public:
    TerminalRawMode() {
        tcgetattr(STDIN_FILENO, &old);
        termios raw = old;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
    ~TerminalRawMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
    }
};

int read_arrow() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;

    if (c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return 0;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return 0;

        if (seq[0] == '[') {
            if (seq[1] == 'A') return -1;
            if (seq[1] == 'B') return +1;
        }
    }

    if (c == 'q') return 99;
    return 0;
}

pid_t play_music(const std::filesystem::path& file) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("ffplay", "ffplay", "-nodisp", "-loglevel", "quiet", "-autoexit", file.c_str(), nullptr);
        std::_Exit(1);
    }
    return pid;
}

int main() {
    namespace fs = std::filesystem;

    fs::path root = "./music";
    if (!fs::exists(root)) {
        std::cerr << "music folder not found\n";
        return 1;
    }

    std::vector<fs::path> albums;
    for (auto& p : fs::directory_iterator(root))
        if (p.is_directory())
            albums.push_back(p.path());

    for (size_t i = 0; i < albums.size(); ++i)
        std::cout << i << ". " << albums[i].filename() << '\n';

    std::cout << "Select album: ";
    size_t album_index;
    std::cin >> album_index;

    if (album_index >= albums.size()) return 1;

    std::vector<fs::path> tracks;
    for (auto& p : fs::directory_iterator(albums[album_index]))
        if (p.is_regular_file())
            tracks.push_back(p.path());

    if (tracks.empty()) return 0;

    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist(0, tracks.size() - 1);
    size_t selected = dist(rng);

    TerminalRawMode raw;
    pid_t player = -1;

    while (true) {
        system("clear");

        for (size_t i = 0; i < tracks.size(); ++i) {
            if (i == selected)
                std::cout << "> ";
            else
                std::cout << "  ";
            std::cout << tracks[i].stem() << '\n';
        }

        if (player > 0) kill(player, SIGKILL);
        player = play_music(tracks[selected]);

        int key;
        while ((key = read_arrow()) == 0) {}

        if (key == 99) break;
        if (key == -1 && selected > 0) selected--;
        if (key == +1 && selected + 1 < tracks.size()) selected++;
    }

    if (player > 0) kill(player, SIGKILL);
    return 0;
}

