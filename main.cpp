#include <SFML/Graphics.hpp>

enum class Player
{
    None,
    X,
    O
};

class TicTacToe {
public:
    TicTacToe() : actionsNumber(0), currentPlayer(Player::X), winner(Player::None)
    {
        resetBoard();
    }

    void reset()
    {
        resetBoard();
        actionsNumber = 0;
        currentPlayer = Player::X;
        winner = Player::None;
    }

    bool isGameOver() const
    {
        return winner != Player::None || actionsNumber >= 9;
    }

    Player getCurrentPlayer() const
    {
        return currentPlayer;
    }

    Player getWinner() const
    {
        return winner;
    }

    void makeMove(int row, int col)
    {
        if (!isGameOver() && row >= 0 && row < gridSize && col >= 0 && col < gridSize && board[row][col] == Player::None)
        {
            board[row][col] = currentPlayer;
            checkForWinner(row, col);
            currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
            actionsNumber++;
            return;
        }

        reset();
    }

    Player getCell(int row, int col) const
    {
        return board[row][col];
    }

private:
    const int gridSize = 3;
    int actionsNumber = 0;
    Player currentPlayer;
    Player winner;
    std::vector<std::vector<Player>> board;

    void resetBoard()
    {
        board = std::vector<std::vector<Player>>(gridSize, std::vector<Player>(gridSize, Player::None));
    }

    void checkForWinner(int lastMoveRow, int lastMoveCol)
    {
        // Check row
        if (board[lastMoveRow][0] == board[lastMoveRow][1] && board[lastMoveRow][1] == board[lastMoveRow][2])
        {
            winner = board[lastMoveRow][0];
        }

        // Check column
        if (board[0][lastMoveCol] == board[1][lastMoveCol] && board[1][lastMoveCol] == board[2][lastMoveCol])
        {
            winner = board[0][lastMoveCol];
        }

        // Check diagonals
        if ((lastMoveRow == lastMoveCol || lastMoveRow + lastMoveCol == gridSize - 1) &&
            ((board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
                (board[0][2] == board[1][1] && board[1][1] == board[2][0])))
        {
            winner = board[1][1];
        }
    }
};

class Renderer {
public:
    Renderer() : windowSize(300), cellSize(windowSize / 3)
    {
        initWindow();
    }

    void run(TicTacToe& game)
    {
        while (window.isOpen())
        {
            processEvents(game);
            render(game);
        }
    }

private:
    const int windowSize;
    const int cellSize;

    sf::RenderWindow window;

    void initWindow()
    {
        window.create(sf::VideoMode(windowSize, windowSize), "Tic-Tac-Toe");
        window.setFramerateLimit(60);
    }

    void processEvents(TicTacToe& game)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    handleMouseClick(game, event.mouseButton.x, event.mouseButton.y);
                }
            }
        }
    }

    void handleMouseClick(TicTacToe& game, int mouseX, int mouseY)
    {
        int row = mouseY / cellSize;
        int col = mouseX / cellSize;
        game.makeMove(row, col);
    }

    void render(const TicTacToe& game)
    {
        window.clear();

        // Draw grid
        for (int i = 1; i < 3; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(i * cellSize, 0)),
                sf::Vertex(sf::Vector2f(i * cellSize, windowSize)),
                sf::Vertex(sf::Vector2f(0, i * cellSize)),
                sf::Vertex(sf::Vector2f(windowSize, i * cellSize))
            };
            window.draw(line, 4, sf::Lines);
        }

        // Draw X and O based on the state of the board
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (game.getCell(row, col) == Player::X) {
                    drawX(row, col);
                }
                else if (game.getCell(row, col) == Player::O) {
                    drawO(row, col);
                }
            }
        }

        // Draw winner message
        if (game.getWinner() != Player::None || game.isGameOver()) {
            static sf::Font font;
            bool bFontLoaded = false;
            if (!bFontLoaded)
            {
                font.loadFromFile("C:\\Users\\User\\Desktop\\IT\\PP\\Hangman\\Arial.ttf"); // Change the font file path as needed
                bFontLoaded = true;
            }

            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(30);
            text.setFillColor(sf::Color::White);
            sf::String Answer = (game.getWinner() == Player::X ? "Player X" : "Player O") + std::string(" wins!\nClick to restart.");
            if (game.getWinner() == Player::None)
            {
                Answer = "Draw!\nClick to restart.";
            }

            text.setString(Answer);
            text.setPosition(10, 10);
            window.clear();
            window.draw(text);
        }

        window.display();
    }

    void drawX(int row, int col)
    {
        sf::Vertex line1[] = {
            sf::Vertex(sf::Vector2f(col * cellSize + 10, row * cellSize + 10)),
            sf::Vertex(sf::Vector2f((col + 1) * cellSize - 10, (row + 1) * cellSize - 10))
        };
        sf::Vertex line2[] = {
            sf::Vertex(sf::Vector2f((col + 1) * cellSize - 10, row * cellSize + 10)),
            sf::Vertex(sf::Vector2f(col * cellSize + 10, (row + 1) * cellSize - 10))
        };

        window.draw(line1, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
    }

    void drawO(int row, int col)
    {
        sf::CircleShape circle(cellSize / 2 - 10);
        circle.setPosition(col * cellSize + 10, row * cellSize + 10);
        circle.setOutlineThickness(10);
        circle.setOutlineColor(sf::Color::White);

        window.draw(circle);
    }
};

int main()
{
    TicTacToe game;
    Renderer renderer;
    renderer.run(game);

    return 0;
}