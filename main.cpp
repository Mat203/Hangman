#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>

enum Difficulty {
    Easy,
    Medium,
    Hard
};

class Hangman {
private:
    int hangmanParts;
public:
    void drawNext() {
    }
};

class WordManager {
private:
    std::map<Difficulty, std::vector<std::string>> wordDictionary;
public:
    std::string pickWord(Difficulty difficulty) {
    }
    std::string updateGuessedWord(std::string secretWord, std::string guessedWord, char letter) {
    }
    void readWordDictionary(std::string filename) {
    }
};

class HangmanGame {
private:
    std::string secretWord;
    std::string guessedWord;
    std::vector<char> usedLetters;
    Difficulty difficulty;
    Hangman hangman;
    WordManager wordManager;
public:
    HangmanGame(Difficulty difficulty) {
    }
    void reset() {
    }
    bool isGameOver() {
    }
    std::string getGuessedWord() {
    }
    std::vector<char> getUsedLetters() {
    }
    void guessLetter(char letter) {
    }
};

class Renderer {
private:
    const int windowSize;
    const int cellSize;
    sf::RenderWindow window;
public:
    Renderer() : windowSize(600), cellSize(30) {
    }
    void run(HangmanGame game) {
    }
    void initWindow() {
    }
    void processEvents(HangmanGame game) {
    }
    void handleKeyPress(HangmanGame game, char key) {
    }
    void render(HangmanGame game) {
    }
    void drawGameEndMessage(HangmanGame game) {
    }
};
