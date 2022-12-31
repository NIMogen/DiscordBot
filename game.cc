#include "game.h"

std::unique_ptr<Dictionary> ReadDictionary(std::string filepath) {
  std::ifstream text_file(filepath);
  Dictionary dict;
  for (std::string line; std::getline(text_file, line);) {
    dict.insert(line);
  }

  // A seg fault occurs if I try to insert into a unique_ptr,
  // so dict is converted after it has been populated.
  std::unique_ptr<std::unordered_set<std::string>> ptr_dict(new std::unordered_set(std::move(dict)));
  return ptr_dict;
}

void InitGame(std::unordered_map<dpp::snowflake, GameState> &games,
              const dpp::snowflake &user, const std::unique_ptr<Dictionary> &dictionary) {
  GameState state;
    
  std::mt19937 gen(std::random_device{}());
  std::string element;
  std::sample(dictionary->begin(), dictionary->end(), &element, 1, gen);
  
  state.word = element;
  games.insert({user, state});
}

void Guess(std::unordered_map<dpp::snowflake, GameState> &games, 
                  dpp::snowflake user, 
                  std::string guess) {
  GameState game = games[user];
  game.guesses.push_back(guess);
  games[user] = game;
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
