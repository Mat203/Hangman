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

enum GameState {
    SelectingDifficulty,
    InProgress,
    GameOver
};

class Hangman {
private:
    int hangmanParts;
public:
    void drawNext() {
    }

    int getHangmanParts() {
        return hangmanParts;
    }

    void resetHangmanParts() {
        hangmanParts = 0;
    }

    void addHangmanParts() {
        hangmanParts++;
    }
};

class WordManager {
private:
    std::map<Difficulty, std::vector<std::string>> wordDictionary;
public:
    std::string pickWord(Difficulty difficulty) {
        int randomIndex = rand() % wordDictionary[difficulty].size();
        std::cout << "Right word: " << wordDictionary[difficulty][randomIndex] << std::endl;
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
    GameState gameState;

public:
    HangmanGame(Difficulty difficulty) : difficulty(difficulty) {
        wordManager.readWordDictionary("text.txt");
        gameState = SelectingDifficulty;
        reset();
    }

    GameState getGameState() const {
        return gameState;
    }

    void setGameState(GameState gameState) {
        this->gameState = gameState;
    }

    void reset() {
        secretWord = wordManager.pickWord(difficulty);
        guessedWord = std::string(secretWord.size(), '_');
        usedLetters.clear();
        hangman.resetHangmanParts();
    }

    bool isGameOver() {
        return hangman.getHangmanParts() >= 6 || guessedWord == secretWord;
    }

    std::string getGuessedWord() const {
        return guessedWord;
    }

    std::string getSecretWord() const {
        return secretWord;
    }

    std::vector<char> getUsedLetters() const {
        return usedLetters;
    }

    int getRemainingAttempts() {
        return std::max(6 - hangman.getHangmanParts(), 0);
    }

    void restart(Difficulty newDifficulty) {
        difficulty = newDifficulty;
        reset();
    }

    void guessLetter(char letter) {
        usedLetters.push_back(letter);
        if (secretWord.find(letter) == std::string::npos) {
            hangman.drawNext();
            hangman.addHangmanParts();
        }
        else {
            guessedWord = wordManager.updateGuessedWord(secretWord, guessedWord, letter);
        }
    }
};

class Renderer {
private:
    const int windowSize;
    const int cellSize;
    sf::RenderWindow window;
public:
    Renderer() : windowSize(600), cellSize(30), window(sf::VideoMode(windowSize, windowSize), "Hangman Game") {
    }

    void run(HangmanGame& game) {
        while (window.isOpen()) {
            processEvents(game);
            render(game);
        }
    }

    void processEvents(HangmanGame& game) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    if (game.getGameState() == SelectingDifficulty) {
                        handleDifficultyInput(game, static_cast<char>(event.text.unicode));
                    }
                    else if (game.getGameState() == InProgress) {
                        game.guessLetter(static_cast<char>(event.text.unicode));
                    }
                }
            }
        }
    }

    void handleDifficultyInput(HangmanGame& game, char input) {
        if (input >= '1' && input <= '3') {
            Difficulty selectedDifficulty = static_cast<Difficulty>(input - '1');
            game.restart(selectedDifficulty);
            game.setGameState(InProgress);
        }
    }

    void render(HangmanGame& game) {
        window.clear();
        sf::RectangleShape rectangle(sf::Vector2f(cellSize - 50, cellSize - 50));
        static sf::Font font;
        bool bFontLoaded = false;
        if (!bFontLoaded)
        {
            font.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\Arial.ttf"); 
            bFontLoaded = true;
        }

        if (game.getGameState() == SelectingDifficulty) {
            sf::Text selectDifficultyText;
            selectDifficultyText.setFont(font);
            selectDifficultyText.setString("Choose a difficulty: 1 - Easy, 2 - Medium, 3 - Hard");
            selectDifficultyText.setCharacterSize(24);
            selectDifficultyText.setPosition(10, windowSize / 2);
            window.draw(selectDifficultyText);
        }

        else if (game.getGameState() == InProgress) {
            for (int i = 0; i < game.getGuessedWord().size(); i++) {
                sf::Text text;
                text.setFont(font);
                text.setString(std::string(1, game.getGuessedWord()[i]));
                text.setCharacterSize(24);
                text.setPosition(i * cellSize + cellSize / 4, windowSize / 2 + cellSize / 4);
                window.draw(text);
            }

            //draw guessed word DELETE THEN
            sf::Text guessedWordText;
            guessedWordText.setFont(font);
            guessedWordText.setString("Guessed Word: " + game.getSecretWord());
            guessedWordText.setCharacterSize(24);
            guessedWordText.setPosition(10, 10);
            window.draw(guessedWordText);


            //draw attempts DELETE THEN
            sf::Text attemptsText;
            attemptsText.setFont(font);
            attemptsText.setString("Attempts left: " + std::to_string(game.getRemainingAttempts()));
            attemptsText.setCharacterSize(24);
            attemptsText.setPosition(windowSize - 200, 10);
            window.draw(attemptsText);
            if (game.isGameOver()) {
                game.setGameState(GameOver);
            }
        }
        //win message
        else if (game.getGameState() == GameOver) {
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(20);
            gameOverText.setPosition(0, windowSize / 2 - 50);

            if (game.getGuessedWord() == game.getSecretWord()) {
                gameOverText.setString("Congratulations! You guessed the word!");
            }
            else {
                gameOverText.setString("Game over. The word was: " + game.getSecretWord());
            }

            window.draw(gameOverText);
        }

        window.display();
    }
};



int main() {
    HangmanGame game(Medium);
    Renderer renderer;
    renderer.run(game);
    return 0;
}
