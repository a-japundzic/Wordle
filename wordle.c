#include "wordle.h"
#include <string.h>
#include <stdbool.h>
#include "cs136-trace.h"
#include <assert.h>

const int LOWERCASE_A = 97;    // Starting point for a in ascii
const int LOWERCASE_Z = 122;   // Starting point for z in ascii
const int UPPERCASE_A = 65;    // Same but A
const int UPPERCASE_Z = 90;    // Same but Z
const int LOWER_UPPER_DIFF = 32;  // Diff between lower and uppper case
const int ALPHABET_SHIFT = 97;   // Diff between 0-27 and a-z
const char PERIOD = '.';         // Period!
const int ALPHA_LEN = 26;        // Length of alphabet

// see wordle.h
bool evaluate_guess(const char *secret, const char *guess, char *result) {
  assert(secret);
  assert(guess);
  assert(result);
  
  const int word_len = strlen(secret);
  int guess_len = strlen(guess);
  
  assert(guess_len == word_len);
  
  int equal_char = 0;
  int alphabet[27] = {0};
  
  for (int i = 0; i < word_len; i++) {
    alphabet[secret[i] - ALPHABET_SHIFT] = 1;
  }
  
  for (int i = 0; i < word_len; i++) {
    if (guess[i] == secret[i]) {
      result[i] = guess[i] - LOWER_UPPER_DIFF;
      equal_char++;
    } else if (alphabet[guess[i] - ALPHABET_SHIFT]) {
      result[i] = guess[i];
    }  else {
      result[i] = PERIOD;
    }
  }
  
  if (equal_char == word_len) {
    return 1;
  } else {
    return 0;
  }
  
}


// see wordle.h
int find_in_list(const char *guess, char *word_list[], int num_words) {
  assert(guess);
  assert(word_list);
  
  int guess_len = strlen(guess);
  int low = 0;
  int high = num_words - 1;
  int mid = 0;
  
  for (int i = 0; i < guess_len; i++) {
    while (low <= high) {
      mid = (low + high) / 2;
      if (word_list[mid][i] == guess[i]) {
        break;
      } else if (word_list[mid][i] < guess[i]) {
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }
  }
  
  if (strcmp(guess, word_list[mid]) == 0) {
    return mid;
  } else {
    return -1;
  }
  
}


// see wordle.h
void available_letters(char *guesses[], char *results[], int num_guesses,
                       char *alphabet) {
  assert(guesses);
  assert(results);
  assert(alphabet);
  assert(num_guesses >= 0);
  
  const int guess_length = strlen(guesses[0]);
  
  for (int i = 0; i < num_guesses; i++) {
    for (int j = 0; j < guess_length; j++) {
      if (results[i][j] == PERIOD) {
        alphabet[guesses[i][j] - ALPHABET_SHIFT] = PERIOD;
      } else if (results[i][j] >= UPPERCASE_A && results[i][j] <= UPPERCASE_Z) {
        alphabet[(results[i][j] + LOWER_UPPER_DIFF) - ALPHABET_SHIFT] = 
          results[i][j];
      } else {
        alphabet[results[i][j] - ALPHABET_SHIFT] = 
          results[i][j] - LOWER_UPPER_DIFF;
      }
    }
  }
  


  

  char alpha_start = LOWERCASE_A;
  
  for (int i = 0; i < ALPHA_LEN + 1; i++) {
    if (alphabet[i] != (alpha_start - LOWER_UPPER_DIFF) &&
        alphabet[i] != PERIOD) {
      alphabet[i] = alpha_start;
    }
    
    alpha_start++;
  }
  
  alphabet[ALPHA_LEN] = '\0';
}


// see wordle.h
bool valid_hard_guess(char *guesses[], char *results[], int num_guesses,
                      const char *next_guess) {
  assert(guesses);
  assert(results);
  assert(next_guess);
  assert(num_guesses >= 0);
  
  int guess_len = strlen(guesses[0]);
  int next_guess_len = strlen(next_guess);
  
  assert(guess_len == next_guess_len);
  
  bool valid = true;
  bool in_word = false; 
  
  for (int i = 0; i < num_guesses; i++) {
    for (int j = 0; j < guess_len; j++) {
      if (results[i][j] >= UPPERCASE_A && results[i][j] <= UPPERCASE_Z) {
        if (next_guess[j] != (results[i][j] + LOWER_UPPER_DIFF)) {
          valid = false; 
          return valid;
        }
        
        continue;
      } else if (results[i][j] != PERIOD) {
        if (results[i][j] == next_guess[j]) {
          valid = false;
          return valid;
        }
      }
      for (int m = 0; m < guess_len; m++) {
        if (results[i][j] == '.') {
          if (guesses[i][j] == next_guess[m]) {
            valid = false;
            return valid;
          }
        } else if (results[i][j] == next_guess[m]) {
            in_word = true; 
          }
      }
      
      if (in_word == false && results[i][j] >= LOWERCASE_A) {
        valid = false;
        return valid;
      }
     
      in_word = false; 
    }
  }
  
  return valid;
}


// see wordle.h
int find_solution(char *guesses[], char *results[], int num_guesses,
                  char *word_list[], int num_words,
                  char *solutions[], int max_solutions) {
  
  assert(guesses);
  assert(results);
  assert(word_list);
  assert(solutions);
  assert(max_solutions >= 1);
  assert(num_guesses >= 0);
  
  int guess_len = strlen(guesses[0]);
  bool valid_word = true;
  int n = 0;
  int unusable_alphabet[27] = {0};
  int must_use_alphabet[27] = {0};
  const int alpha_len = 27;
  bool in_word = false; 
  
  for (int i = 0; i < num_guesses; i++) {
    for (int j = 0; j < guess_len; j++) {
      if (results[i][j] == PERIOD) {
          unusable_alphabet[guesses[i][j] - ALPHABET_SHIFT] = 1;
      } else if (results[i][j] >= LOWERCASE_A && results[i][j] <= LOWERCASE_Z) {
          must_use_alphabet[results[i][j] - ALPHABET_SHIFT] = 1;
        }
    }
  }
  
  for (int i = 0; i < num_words; i++) {
    valid_word = true;
    for (int j = 0; j < num_guesses; j++) {
      for (int m = 0; m < guess_len; m++) {
        if (results[j][m] >= UPPERCASE_A && results[j][m] <= UPPERCASE_Z) {
          if (word_list[i][m] != results[j][m] + LOWER_UPPER_DIFF) {
            valid_word = false;
            break;
          }
        } else if (results[j][m] >= LOWERCASE_A && 
                   results[j][m] <= LOWERCASE_Z) {
          if (word_list[i][m] == results[j][m]) {
            valid_word = false; 
            break;
          }
        }
        
        if(unusable_alphabet[word_list[i][m] - ALPHABET_SHIFT]) {
          valid_word = false;
          break;
        } 
      }
          
      for (int a = 0; a < alpha_len; a++) {
        in_word = false; 
        if(must_use_alphabet[a]) {
          for (int m = 0; m < guess_len; m++) {
            if (word_list[i][m] == (a + ALPHABET_SHIFT)) {
              in_word = true;
            }
          }
          
          if (!in_word) {
            valid_word = false;
            break;
          } 
        }
      }    
    }

    if (valid_word && n <= max_solutions) {
      solutions[n] = word_list[i];
      n++;
    }
  }
  
  return n;
}
