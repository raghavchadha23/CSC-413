#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // I2C LCD library

#include <stdlib.h> // For random()

// Initialize the LCD with the I2C address
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Address 0x27, 20 columns, 4 rows

// Define pins for each person (4 persons, 2 pins each)
int personPins[4][2] = {
  {2, 3},  // Ross Geller
  {5, 6},  // Rachel Green
  {9, 10}, // Chandler Bing
  {12, 13} // Joey Tribbiani
};

// Define participant names
const char *personNames[] = {"Ross", "Rachel", "Chandler", "Joey"};

// Number of persons and questions
const int numPersons = 4;
const int numQuestions = 5;  // Asking 5 questions

// Updated questions based on the document
const char *questions[] = {
  "How do you feel about this project?",       // Question 1
  "What is the biggest hurdle we face?",       // Question 2
  "What excites you about this challenge?",    // Question 3
  "What would help reduce current stress?",    // Question 4
  "How can we innovate further?"               // Question 5
};

// Updated plate options for each question
const char *plateOptions[][3] = {
  {"Stressed", "Neutral", "Excited"},          // Options for Question 1
  {"Time", "Resources", "Collaboration"},     // Options for Question 2
  {"Impact", "Innovation", "Growth"},         // Options for Question 3
  {"Clarity", "Support", "Break"},            // Options for Question 4
  {"Processes", "Ideas", "Tools"}             // Options for Question 5
};

// Emotional states based on responses
const char *emotionalStates[] = {"LowEnergy", "Optimistic", "Stressed", "Focused", "Excited"};

// Brainstorming prompts for each emotional state
const char *brainstormingPrompts[][3] = {
  {"What small wins can boost momentum?", "How can we recharge as a team?", "What tasks can we simplify?"},
  {"What bold ideas can push us further?", "Where can we innovate the most?", "How do we maximize potential?"},
  {"What clear steps can ease pressure?", "What immediate support do we need?", "How do we clarify priorities?"},
  {"How can we streamline key efforts?", "What are the most critical tasks?", "How can we reduce bottlenecks?"},
  {"What exciting opportunities lie ahead?", "How do we capitalize on our success?", "What motivates us most?"}
};

// Responses storage
int responses[numPersons][numQuestions]; // Stores responses for each person and question
bool personResponded[numPersons];        // Tracks whether a person has responded

// Question tracker
int currentQuestion = 0;

void setup() {
  Serial.begin(115200);
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Listening...");
  
  for (int i = 0; i < numPersons; i++) {
    pinMode(personPins[i][0], INPUT_PULLUP);
    pinMode(personPins[i][1], INPUT_PULLUP);
  }
  
  for (int i = 0; i < numPersons; i++) {
    for (int j = 0; j < numQuestions; j++) {
      responses[i][j] = 0;
    }
  }

  randomSeed(analogRead(0)); // Seed random generator
}

void loop() {
  for (currentQuestion = 0; currentQuestion < numQuestions; currentQuestion++) {
    for (int i = 0; i < numPersons; i++) {
      personResponded[i] = false;
    }
    Serial.print("Q");
    Serial.print(currentQuestion + 1);
    Serial.print(": ");
    Serial.println(questions[currentQuestion]);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Q" + String(currentQuestion + 1) + ":");
    lcd.setCursor(0, 1);
    lcd.print(questions[currentQuestion]);

    Serial.println("Options:");
    for (int j = 0; j < 3; j++) {
      Serial.print("  Option ");
      Serial.print(j + 1);
      Serial.print(": ");
      Serial.println(plateOptions[currentQuestion][j]);
    }

    while (!allResponded()) {
      for (int i = 0; i < numPersons; i++) {
        checkPersonResponse(i);
      }
    }

    Serial.println("All responded. Remove plates.");
    while (!allPlatesRemoved()) {}
    delay(500);
    Serial.println("----------------------------------");
  }

  Serial.println("Survey completed. Results:");
  int teamScore = 0;
  for (int i = 0; i < numPersons; i++) {
    Serial.print("Person: ");
    Serial.println(personNames[i]);
    int score = 0;
    for (int q = 0; q < numQuestions; q++) {
      int choice = responses[i][q];
      if (choice >= 1 && choice <= 3) {
        score += choice;
      }
    }
    teamScore += score;
  }

  const char *teamState;
  if (teamScore <= 27) {
    teamState = emotionalStates[0];
  } else if (teamScore <= 32) {
    teamState = emotionalStates[1];
  } else if (teamScore <= 37) {
    teamState = emotionalStates[2];
  } else if (teamScore <= 42) {
    teamState = emotionalStates[3];
  } else {
    teamState = emotionalStates[4];
  }

  Serial.print("Team Emotional State: ");
  Serial.println(teamState);

  Serial.print("PlayMusic:");
  Serial.println(teamState);  // Send command to play music

  String brainstormingText = getBrainstormingPrompt(teamState);
  Serial.print("Brainstorming Prompt: ");
  Serial.println(brainstormingText);

  int displayCount = 0;
  while (displayCount < 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Emotional State:");
    lcd.setCursor(0, 1);
    lcd.print(teamState);
    delay(2000);

    int textLength = brainstormingText.length();
    for (int i = 0; i < textLength; i += 40) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(brainstormingText.substring(i, min(i + 20, textLength)));

      if (i + 20 < textLength) {
        lcd.setCursor(0, 1);
        lcd.print(brainstormingText.substring(i + 20, min(i + 40, textLength)));
      }

      delay(2000);
    }
    displayCount++;
  }

  Serial.println("END");
  while (true) {}
}

String getBrainstormingPrompt(const char *state) {
  int stateIndex = 0;
  for (int i = 0; i < 5; i++) {
    if (strcmp(state, emotionalStates[i]) == 0) {
      stateIndex = i;
      break;
    }
  }
  int randomIndex = random(0, 3); // Pick one of the three prompts randomly
  return String(brainstormingPrompts[stateIndex][randomIndex]);
}

bool allResponded() {
  for (int i = 0; i < numPersons; i++) {
    if (!personResponded[i]) {
      return false;
    }
  }
  return true;
}

bool allPlatesRemoved() {
  for (int i = 0; i < numPersons; i++) {
    int pin1 = personPins[i][0];
    int pin2 = personPins[i][1];
    if (digitalRead(pin1) == LOW || digitalRead(pin2) == LOW) {
      return false;
    }
  }
  return true;
}

void checkPersonResponse(int personIndex) {
  if (!personResponded[personIndex]) {
    int pin1 = personPins[personIndex][0];
    int pin2 = personPins[personIndex][1];
    int state1 = digitalRead(pin1);
    int state2 = digitalRead(pin2);

    if (state1 == LOW && state2 == HIGH) {
      personResponded[personIndex] = true;
      Serial.print("Person ");
      Serial.print(personNames[personIndex]);
      Serial.print(" selected ");
      Serial.println(plateOptions[currentQuestion][0]);
      responses[personIndex][currentQuestion] = 1;
    } else if (state1 == HIGH && state2 == LOW) {
      personResponded[personIndex] = true;
      Serial.print("Person ");
      Serial.print(personNames[personIndex]);
      Serial.print(" selected ");
      Serial.println(plateOptions[currentQuestion][1]);
      responses[personIndex][currentQuestion] = 2;
    } else if (state1 == LOW && state2 == LOW) {
      personResponded[personIndex] = true;
      Serial.print("Person ");
      Serial.print(personNames[personIndex]);
      Serial.print(" selected ");
      Serial.println(plateOptions[currentQuestion][2]);
      responses[personIndex][currentQuestion] = 3;
    }
  }
}
