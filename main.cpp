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
    bool hintUsed;

public:
    HangmanGame(Difficulty difficulty) : difficulty(difficulty), hintRequested(false), hintDisplayed(false), hintUsed(false) {
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
        return hangman.getHangmanParts();
    }

    void restart(Difficulty newDifficulty) {
        difficulty = newDifficulty;
        hintRequested = false;
        hintUsed = false;
        hintDisplayed = false;
        reset();
    }

    void guessLetter(char letter) {
        if (std::find(usedLetters.begin(), usedLetters.end(), letter) != usedLetters.end()) {
            return;
        }
        usedLetters.push_back(letter);
        if (secretWord.find(letter) == std::string::npos) {
            hangman.addHangmanParts();
            if (hangman.getHangmanParts() == 3 && !hintRequested && !hintUsed) {
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

    void makeHintDisplayed() {
        hintDisplayed = true;
    }

    bool isHintDisplayed() {
        return hintDisplayed;
    }

    void isHintUsed() {
        hintUsed = true;
        hintDisplayed = false;
        hintRequested = false;
    }

    std::string getHint() {
        std::string missingLetters = "";
        for (char c : secretWord) {
            if (guessedWord.find(c) == std::string::npos) {
                missingLetters += c;
            }
        }

        hint = std::string(1, missingLetters[1]);
        
        return hint;
    }
};

class TextureManager {
private:
    sf::Texture treeTexture;
    sf::Texture headTexture;
    sf::Texture bodyTexture;
    sf::Texture armTexture;
    sf::Texture legTexture;

public:
    TextureManager() {
        treeTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanTree.png");
        headTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanHead.png");
        bodyTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanOnlyBody.png");
        armTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanAll.png");
        legTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanAll.png");
    }

    sf::Texture& getTreeTexture() { return treeTexture; }
    sf::Texture& getHeadTexture() { return headTexture; }
    sf::Texture& getBodyTexture() { return bodyTexture; }
    sf::Texture& getArmTexture() { return armTexture; }
    sf::Texture& getLegTexture() { return legTexture; }
};

class Renderer {
private:
    const int windowSize;
    const int cellSize;
    sf::Font font;
    sf::RenderWindow window;
    sf::Color grassGreen;
    HangmanGame& game;
    TextureManager textureManager;
    sf::Clock hintClock;
    sf::Sprite treeSprite;
    sf::Sprite headSprite;
    sf::Sprite bodySprite;
    sf::Sprite armSprite;
    sf::Sprite legSprite;
public:
    Renderer(HangmanGame& game) : game(game), windowSize(600), cellSize(30), window(sf::VideoMode(windowSize, windowSize), "Hangman Game"), grassGreen(124, 252, 0) {
        bool bFontLoaded = false;
        if (!bFontLoaded)
        {
            bFontLoaded = true;
            font.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\Arial.ttf");
            treeSprite.setTexture(textureManager.getTreeTexture());
            headSprite.setTexture(textureManager.getHeadTexture());
            bodySprite.setTexture(textureManager.getBodyTexture());
            armSprite.setTexture(textureManager.getArmTexture());
            legSprite.setTexture(textureManager.getLegTexture());
        }
    }


    void run() {
        while (window.isOpen()) {
            processEvents();
            render();
        }
    }

    void processEvents() {
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
                    if (event.text.unicode == 'h' && game.isHintRequested()) {
                        game.makeHintDisplayed();
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

    void render() {
        window.clear(sf::Color::White);

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

            if (game.isHintDisplayed()) {
                if (hintClock.getElapsedTime().asSeconds() < 7) {
                    displayHint();
                }
                else {
                    game.isHintUsed();
                }

            }

            drawHangman();
            if (game.isGameOver()) {
                game.setGameState(GameOver);
            }
        }
        
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

    void displayHint() {
        sf::Text hint;
        hint.setFont(font);
        hint.setString("Hint: " + game.getHint());
        hint.setCharacterSize(24);
        hint.setPosition(windowSize / 2, windowSize / 2 + 100);
        hint.setFillColor(grassGreen);
        window.draw(hint);
    }

    void drawHangman() {
        int hangmanParts = game.getRemainingAttempts();

        if (hangmanParts == 0) {
            treeSprite.setPosition(5, 5);
            window.draw(treeSprite);
        }

        if (hangmanParts > 0) {
            headSprite.setPosition(5, 5);
            window.draw(headSprite);
        }

        if (hangmanParts > 1) {
            bodySprite.setPosition(5, 5);
            window.draw(bodySprite);
        }

        if (hangmanParts > 2) {
            sf::RectangleShape arm(sf::Vector2f(10, 20));
            arm.setPosition(windowSize / 2, windowSize / 2 - 20);
            arm.setFillColor(grassGreen);
            window.draw(arm);
        }

        if (hangmanParts > 3) {
            sf::RectangleShape arm(sf::Vector2f(10, 20));
            arm.setPosition(windowSize / 2 + 20, windowSize / 2 + 20);
            arm.setFillColor(grassGreen);
            window.draw(arm);
        }

        if (hangmanParts > 4) {
            sf::RectangleShape leg(sf::Vector2f(10, 20));
            leg.setPosition(windowSize / 2 + 10, windowSize / 2 + 40);
            leg.setFillColor(grassGreen);
            window.draw(leg);
        }

        if (hangmanParts > 5) {
            sf::RectangleShape leg(sf::Vector2f(10, 20));
            leg.setPosition(windowSize / 2 + 20, windowSize / 2 + 40);
            leg.setFillColor(grassGreen);
            window.draw(leg);
        }
    }
};



int main() {
    HangmanGame game(Medium);
    Renderer renderer(game);
    renderer.run();
    return 0;
}
