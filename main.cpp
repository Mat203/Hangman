#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

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
        int randomIndex = rand() % wordDictionary[difficulty].size();
        return wordDictionary[difficulty][randomIndex];
    }

    std::string updateGuessedWord(std::string secretWord, std::string guessedWord, char letter) {
        for (int i = 0; i < secretWord.size(); i++) {
            if (secretWord[i] == letter) {
                guessedWord[i] = letter;
            }
        }
        return guessedWord;
    }

    void readWordDictionary(std::string filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string word;
            std::getline(iss, word, ',');
            Difficulty difficulty;
            if (word == "Easy") difficulty = Easy;
            else if (word == "Medium") difficulty = Medium;
            else if (word == "Hard") difficulty = Hard;
            while (std::getline(iss, word, ',')) {
                word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
                wordDictionary[difficulty].push_back(word);
            }
        }
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

int main() {
    WordManager wm;
    wm.readWordDictionary("text.txt"); 

    std::string easyWord = wm.pickWord(Easy);
    std::cout << "Easy word: " << easyWord << std::endl;

    std::string mediumWord = wm.pickWord(Medium);
    std::cout << "Medium word: " << mediumWord << std::endl;

    std::string hardWord = wm.pickWord(Hard);
    std::cout << "Hard word: " << hardWord << std::endl;

    std::string secretWord = "table";
    std::string guessedWord = "_____";
    char letter = 'a';
    std::string updatedWord = wm.updateGuessedWord(secretWord, guessedWord, letter);
    std::cout << "Updated word: " << updatedWord << std::endl;

    return 0;
}