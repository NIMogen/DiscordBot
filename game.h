#ifndef GAME_H
#define GAME_H
#include <dpp/dpp.h>
#include <string>
#include <random>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <algorithm>

typedef std::unordered_set<std::string> Dictionary;

std::unique_ptr<Dictionary> ReadDictionary(std::string filepath);

struct GameState {
  std::string word;
  std::vector<std::string> guesses;
};

// Start game and init vars used for game
void InitGame(std::unordered_map<dpp::snowflake, GameState> &games, 
		const dpp::snowflake &user, const std::unique_ptr<Dictionary> &dictionary);

// Add guess to Vector of guesses in GameState
void Guess(std::unordered_map<dpp::snowflake, GameState> &games,
            dpp::snowflake user, 
            std::string guess);

// Check if recieved guess was valid
bool Validate(std::string guess, const std::unique_ptr<Dictionary> &dictionary);

// Check if the user Guessed the correct word
bool CheckWinner(std::unordered_map<dpp::snowflake, GameState> &games, const dpp::snowflake &user);

// Check if the player lost 
bool CheckLoser(std::unordered_map<dpp::snowflake, GameState> &games, const dpp::snowflake &user);

// Listen for a new message; runs in a while loop
bool Listen(std::unordered_map<dpp::snowflake, GameState> &games, 
		        const dpp::snowflake &user, 
            const std::unique_ptr<Dictionary> &dictionary);

#endif
