#include "game.h"

std::unique_ptr<Dictionary> ReadDictionary(std::string filepath) {
  std::ifstream text_file(filepath);
  Dictionary dict;
  for (std::string line; std::getline(text_file, line);) {
    dict.insert(line);
  }

  // A seg fault occurs if I try to insert into a unique_ptr.
  // To get around this the dict is converted after it has been populated.
  std::unique_ptr<std::unordered_set<std::string>> ptr_dict(new std::unordered_set(std::move(dict)));
  return ptr_dict;
}

void InitGame(std::unordered_map<dpp::snowflake, GameState> &games,
              const dpp::snowflake &user,
              const std::unique_ptr<Dictionary> &dictionary) {
  GameState state;
    
  std::mt19937 gen(std::random_device{}());
  std::string element;
  std::sample(dictionary->begin(), dictionary->end(), &element, 1, gen);
  
  state.word = element;
  games.insert({user, state});
}

// Check for the winner in here instead of elsewhere
void Guess(std::unordered_map<dpp::snowflake, GameState> &games, 
           dpp::snowflake user,
           std::string guess) {
  GameState game = games[user];
  std::string formatted_guess = ProcessGuess(guess, game);
  game.guesses.push_back(formatted_guess);
  games[user] = game;
}

std::string ProcessGuess(std::string guess, GameState game) {
  char correct = '#';
  char present = '*';
  char incorrect = '+';
  char outline_char;
  std::string formatted;
  char buf[3][16]; // We are able to count on this having a fixed size

  auto j = 0;
  for (auto i = 0; i < guess.length(); i++) {
    if (game.word[i] == guess[i]) {
      outline_char = correct;
    } else if (game.word[i] != guess[i] && game.word.find(guess[i]) != std::string::npos) {
      outline_char = present;
    } else {
      outline_char = incorrect;
    }
    // TODO: Turn this into a loop
    buf[0][j] = outline_char;
    buf[0][j+1] = outline_char;
    buf[0][j+2] = outline_char;

    buf[1][j] = outline_char;
    buf[1][j+1] = guess[i];
    buf[1][j+2] = outline_char;

    buf[2][j] = outline_char;
    buf[2][j+1] = outline_char;
    buf[2][j+2] = outline_char;

    buf[0][15] = '\n';
    buf[0][15] = '\n';
    buf[0][15] = '\n';

    j += 3;
  }

  for (auto row = 0; row < 3; row++) {
    for (auto col = 0; col < 16; col++) {
      formatted.push_back(buf[row][col]);
    }
  }
  return formatted;
}

bool Validate(std::string guess, const std::unique_ptr<Dictionary> &dictionary) {
  if (guess.length() != 5) {
    return false;
  } else if (dictionary->find(guess) == dictionary->end()) {
    return false;
  } else {
    return true;
  }
}

bool CheckWinner(std::unordered_map<dpp::snowflake, GameState> &games, const dpp::snowflake &user) {
  GameState state = games[user];
  if (state.word == state.guesses.back()) {
    return true;
  } else {
    return false;
  }
}

bool CheckLoser(std::unordered_map<dpp::snowflake, GameState> &games, const dpp::snowflake &user) {
  GameState game = games[user];
  if (game.guesses.size() >= 6) {
    std::cout << "You're out of guesses!" << std::endl;
    std::cout << "Game Over" << std::endl;
    return true;
  } else {
    return false;
  }
}
