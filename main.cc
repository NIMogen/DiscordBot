#include "game.h"
#include <iostream>

int main() {
  const char *token = std::getenv("WORDLE_TOKEN");
  if (!token) {
    std::cerr << "Empty token" << std::endl;
    return 1;
  }
  dpp::cluster bot(token);
  std::cout << "Token accepted..." << std::endl;
  

  bot.on_ready([&bot](const auto & event) {
    std::cout << "Logged in as " << bot.me.username << "!\n";
  });

// Initialize the games map and the dictionary set
  std::unordered_map<dpp::snowflake, GameState> games;
  std::unique_ptr<Dictionary> dictionary = ReadDictionary("sgb-words.txt");
  
  bot.on_message_create([&bot, &games, &dictionary](const auto & event) {
    dpp::snowflake user = event.msg.author.id;
    if (games.find(user) != games.end()) {
      if (event.msg.content == "!quit") {
        bot.message_create(dpp::message(event.msg.channel_id, "Quitting"));
        games.erase(user);
      }
      else if (Validate(event.msg.content, dictionary)) {
        Guess(games, user, event.msg.content);
        if (CheckWinner(games, user)) {
          bot.message_create(dpp::message(event.msg.channel_id, "You got it!"));
          games.erase(user);
        } else if (CheckLoser(games, user)) {
          bot.message_create(dpp::message(event.msg.channel_id, "Game over! The correct word was: "));
          bot.message_create(dpp::message(event.msg.channel_id, games[user].word));
          games.erase(user);
        } else {
          // Represent game to user
          GameState state = games[user];
          bot.message_create(dpp::message(event.msg.channel_id, "------"));
          for (auto word: state.guesses)
            bot.message_create(dpp::message(event.msg.channel_id, word));
          bot.message_create(dpp::message(event.msg.channel_id, "------"));
        }
      } else {
        bot.message_create(dpp::message(event.msg.channel_id, "Invalid guess"));
      }

    }
    else if (event.msg.content == "!play") {
      bot.message_create(dpp::message(event.msg.channel_id, "Game has been started! Send !quit to quit."));
      InitGame(games, user, dictionary);
    }
  });
  bot.start(false);
  return 0;
}
