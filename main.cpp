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
    std::string hint;
    Difficulty difficulty;
    Hangman hangman;
    WordManager wordManager;
    GameState gameState;
    bool hintDisplayed;
    bool hintRequested;

public:
    HangmanGame(Difficulty difficulty) : difficulty(difficulty), hintDisplayed(false) {
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
        if (std::find(usedLetters.begin(), usedLetters.end(), letter) != usedLetters.end()) {
            return;
        }
        usedLetters.push_back(letter);
        if (secretWord.find(letter) == std::string::npos) {
            hangman.addHangmanParts();
            if (hangman.getHangmanParts() == 3 && !hintRequested) {
                hintRequested = true;
            }
        }
        else {
            guessedWord = wordManager.updateGuessedWord(secretWord, guessedWord, letter);
        }
    }

    bool isHintRequested() {
        return hintRequested;
    }

    std::string getHint() {
        std::string missingLetters = "";
        for (char c : secretWord) {
            if (guessedWord.find(c) == std::string::npos) {
                missingLetters += c;
            }
        }

        int randomIndex = rand() % secretWord.size();
        hint = std::string(1, missingLetters[3]);
        
        return hint;
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
                    if (event.text.unicode == '0' && game.getGameState() == GameOver) { 
                        game.setGameState(SelectingDifficulty);
                    }
                    else if (game.getGameState() == SelectingDifficulty) {
                        handleDifficultyInput(game, static_cast<char>(event.text.unicode));
                    }
                    else if (game.getGameState() == InProgress) {
                        game.guessLetter(static_cast<char>(event.text.unicode));
                    }
                    else if (static_cast<char>(event.text.unicode) == 'h' && game.isHintRequested()) {
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
        window.clear(sf::Color::White);

        static sf::Font font;
        bool bFontLoaded = false;
        if (!bFontLoaded)
        {
            font.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\Arial.ttf");
            bFontLoaded = true;
        }
        sf::Color grassGreen(124, 252, 0);

        if (game.getGameState() == SelectingDifficulty) {
            sf::Text selectDifficultyText;
            selectDifficultyText.setFont(font);
            selectDifficultyText.setString("Choose a difficulty: 1 - Easy, 2 - Medium, 3 - Hard");
            selectDifficultyText.setCharacterSize(24);
            selectDifficultyText.setPosition(10, windowSize / 2);
            selectDifficultyText.setFillColor(grassGreen);
            window.draw(selectDifficultyText);
        }

        else if (game.getGameState() == InProgress) {
            for (int i = 0; i < game.getGuessedWord().size(); i++) {
                sf::Text text;
                text.setFont(font);
                text.setString(std::string(1, game.getGuessedWord()[i]));
                text.setCharacterSize(24);
                text.setPosition(windowSize / 2 - game.getGuessedWord().size() * cellSize / 2 + i * cellSize, windowSize / 2);
                text.setFillColor(grassGreen);
                window.draw(text);
            }

            if (game.isHintRequested()) {
                sf::Text hintRequest;
                hintRequest.setFont(font);
                hintRequest.setString("Would you like a hint? Press 'h' for a hint.");
                hintRequest.setCharacterSize(24);
                hintRequest.setPosition(windowSize / 2-200, windowSize / 2 + 50);
                hintRequest.setFillColor(grassGreen);
                window.draw(hintRequest);
            }

            //draw guessed word DELETE THEN
            sf::Text guessedWordText;
            guessedWordText.setFont(font);
            guessedWordText.setString("Guessed Word: " + game.getSecretWord());
            guessedWordText.setCharacterSize(24);
            guessedWordText.setPosition(10, 10);
            guessedWordText.setFillColor(grassGreen);
            window.draw(guessedWordText);


            //draw attempts DELETE THEN
            sf::Text attemptsText;
            attemptsText.setFont(font);
            attemptsText.setString("Attempts left: " + std::to_string(game.getRemainingAttempts()));
            attemptsText.setCharacterSize(24);
            attemptsText.setPosition(windowSize - 200, 10);
            attemptsText.setFillColor(grassGreen);
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
            gameOverText.setFillColor(grassGreen);

            if (game.getGuessedWord() == game.getSecretWord()) {
                gameOverText.setString("Congratulations! You guessed the word! Press '0' to restart");
            }
            else {
                gameOverText.setString("Game over. The word was: " + game.getSecretWord() + " Press '0' to restart");
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
