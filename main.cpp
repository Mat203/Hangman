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

    void getHangmanParts(int parts) {
        hangmanParts = parts;
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

class HintManager {
private:
    std::string hint;
    bool hintDisplayed;
    bool hintRequested;
    bool hintUsed;

public:
    HintManager() : hintDisplayed(false), hintRequested(false), hintUsed(false) {}

    bool isHintRequested() {
        return hintRequested;
    }

    void makeHintDisplayed() {
        hintDisplayed = true;
    }

    bool isHintDisplayed() {
        return hintDisplayed;
    }

    bool isHintUsed() {
        return hintUsed;
    }

    void makeHintUsed() {
        hintUsed = true;
        hintDisplayed = false;
        hintRequested = false;
    }

    void requestHint() {
        hintRequested = true;
    }

    void reset() {
        hintUsed = false;
        hintDisplayed = false;
        hintRequested = false;
    }

    std::string getHint(const std::string& secretWord, const std::string& guessedWord) {
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

class HangmanGame {
public:
    HintManager hintManager;
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
        hintManager.reset();
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
        reset();
    }

    void guessLetter(char letter) {
        if (std::find(usedLetters.begin(), usedLetters.end(), letter) != usedLetters.end()) {
            return;
        }
        usedLetters.push_back(letter);
        if (secretWord.find(letter) == std::string::npos) {
            hangman.addHangmanParts();
            if (hangman.getHangmanParts() == 3 && !hintManager.isHintRequested() && !hintManager.isHintUsed()) {
                hintManager.requestHint();
            }
        }
        else {
            guessedWord = wordManager.updateGuessedWord(secretWord, guessedWord, letter);
        }
    }

    std::string getHint() {
        return hintManager.getHint(secretWord, guessedWord);
    }

    void saveGame() {
        std::ofstream file("savegame.txt");
        if (file.is_open()) {
            file << secretWord << std::endl;
            file << guessedWord << std::endl;
            file << static_cast<int>(difficulty) << std::endl;
            for (char letter : usedLetters) {
                file << letter << ' ';
            }
            file << std::endl;
            file << hangman.getHangmanParts() << std::endl;
            file.close();
        }
    }

    void loadGame() {
        std::ifstream file("savegame.txt");
        if (file.is_open()) {
            std::getline(file, secretWord);
            std::getline(file, guessedWord);
            int diff;
            file >> diff;
            difficulty = static_cast<Difficulty>(diff);
            file.ignore(); 
            std::string lettersLine;
            std::getline(file, lettersLine);
            std::istringstream iss(lettersLine);
            char letter;
            while (iss >> letter) {
                usedLetters.push_back(letter);
            }
            int hangmanParts;
            file >> hangmanParts;
            hangman.getHangmanParts(hangmanParts);
            file.close();
            hintManager.reset();
            gameState = InProgress;
        }
    }


};


class TextureManager {
private:
    sf::Texture treeTexture;
    sf::Texture headTexture;
    sf::Texture bodyTexture;
    sf::Texture rightArmTexture;
    sf::Texture leftArmTexture;
    sf::Texture legTexture;

public:
    TextureManager() {
        treeTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanTree.png");
        headTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanHead.png");
        bodyTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanOnlyBody.png");
        rightArmTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanRightHand.png");
        leftArmTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanLeftHand.png");
        legTexture.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanLeftLeg.png");
    }

    sf::Texture& getTreeTexture() { return treeTexture; }
    sf::Texture& getHeadTexture() { return headTexture; }
    sf::Texture& getBodyTexture() { return bodyTexture; }
    sf::Texture& getRightArmTexture() { return rightArmTexture; }
    sf::Texture& getLeftArmTexture() { return leftArmTexture; }
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
    sf::Sprite rightArmSprite;
    sf::Sprite leftArmSprite;
    sf::Sprite legSprite;
    sf::Image icon;
public:
    Renderer(HangmanGame& game) : game(game), windowSize(600), cellSize(30), window(sf::VideoMode(windowSize, windowSize), "Hangman Game"), grassGreen(124, 252, 0) {
        font.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\Arial.ttf");
        treeSprite.setTexture(textureManager.getTreeTexture());
        headSprite.setTexture(textureManager.getHeadTexture());
        bodySprite.setTexture(textureManager.getBodyTexture());
        rightArmSprite.setTexture(textureManager.getRightArmTexture());
        leftArmSprite.setTexture(textureManager.getLeftArmTexture());
        legSprite.setTexture(textureManager.getLegTexture());
        icon.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\HangmanParts\\HangmanIcon.png");
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
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
                    else if (event.text.unicode == '9') {
                        game.loadGame();
                    }
                    else if (game.getGameState() == SelectingDifficulty) {
                        handleDifficultyInput(game, static_cast<char>(event.text.unicode));
                    }
                    else if (event.text.unicode == 'h' && game.hintManager.isHintRequested()) {
                        game.hintManager.makeHintDisplayed();
                    }
                    else if (event.text.unicode == '8') {
                        game.saveGame();
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

    void render() {
        window.clear(sf::Color::White);

        if (game.getGameState() == SelectingDifficulty) {
            renderSelectDifficulty();
            renderLoadGameText();
            hintClock.restart();
        }
        else if (game.getGameState() == InProgress) {
            renderInProgress();
            renderSaveGameText();
        }
        else if (game.getGameState() == GameOver) {
            renderGameOver();
        }

        window.display();
    }

    void renderSelectDifficulty() {
        sf::Text selectDifficultyText;
        selectDifficultyText.setFont(font);
        selectDifficultyText.setString("Choose a difficulty: 1 - Easy, 2 - Medium, 3 - Hard");
        selectDifficultyText.setCharacterSize(24);
        selectDifficultyText.setPosition(10, windowSize / 2);
        selectDifficultyText.setFillColor(grassGreen);
        window.draw(selectDifficultyText);
    }

    void renderLoadGameText() {
        sf::Text loadGameText;
        loadGameText.setFont(font);
        loadGameText.setString("Press 9 to load a game");
        loadGameText.setCharacterSize(24);
        loadGameText.setPosition(10, windowSize / 2 + 30); 
        loadGameText.setFillColor(grassGreen);
        window.draw(loadGameText);
    }

    void renderSaveGameText() {
        sf::Text saveGameText;
        saveGameText.setFont(font);
        saveGameText.setString("Press 8 to save a game");
        saveGameText.setCharacterSize(24);
        saveGameText.setPosition(window.getSize().x - saveGameText.getGlobalBounds().width - 10, 10); 
        saveGameText.setFillColor(grassGreen);
        window.draw(saveGameText);
    }

    void renderInProgress() {
        renderGuessedWord();
        if (game.hintManager.isHintRequested()) {
            renderHintRequest();
        }
        if (game.hintManager.isHintDisplayed()) {
            renderHint();
        }
        drawHangman();
        if (game.isGameOver()) {
            game.setGameState(GameOver);
        }
    }

    void renderGuessedWord() {
        for (int i = 0; i < game.getGuessedWord().size(); i++) {
            sf::Text text;
            text.setFont(font);
            text.setString(std::string(1, game.getGuessedWord()[i]));
            text.setCharacterSize(24);
            text.setPosition(windowSize / 2 - game.getGuessedWord().size() * cellSize / 2 + i * cellSize, windowSize / 2);
            text.setFillColor(grassGreen);
            window.draw(text);
        }
    }

    void renderHintRequest() {
        sf::Text hintRequest;
        hintRequest.setFont(font);
        hintRequest.setString("Would you like a hint? Press 'h' for a hint.");
        hintRequest.setCharacterSize(24);
        hintRequest.setPosition(windowSize / 2 - 200, windowSize / 2 + 50);
        hintRequest.setFillColor(grassGreen);
        window.draw(hintRequest);
    }

    void renderHint() {
        if (hintClock.getElapsedTime().asSeconds() < 7) {
            displayHint();
        }
        else {
            game.hintManager.makeHintUsed();
        }
    }

    void renderGameOver() {
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
            rightArmSprite.setPosition(5, 5);
            window.draw(rightArmSprite);
        }

        if (hangmanParts > 3) {
            leftArmSprite.setPosition(5, 5);
            window.draw(leftArmSprite);
        }

        if (hangmanParts > 4) {
            legSprite.setPosition(5, 5);
            window.draw(legSprite);
        }

        if (hangmanParts > 5) {
            legSprite.setPosition(5, 5);
            window.draw(legSprite);
        }
    }
};



int main() {
    HangmanGame game(Medium);
    Renderer renderer(game);
    renderer.run();
    return 0;
}
