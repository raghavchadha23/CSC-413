#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // I2C LCD library

// Initialize the LCD with the I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Address 0x27, 16 columns, 2 rows

// Ingredient switch pins and their names
const int switchPins[] = {8, 9, 10, 11, 12, 13};  // Six switches controlling six ingredients
String ingredients[] = {"Rice", "Chicken", "Garlic", "Onion", "Cheese", "Tomato"};
bool selectedIngredients[6] = {false, false, false, false, false, false};  // Track selected ingredients

// Potentiometer and LED pins for nutrient concentration
const int potPin = A5;   // Potentiometer pin for nutrient selection
const int lowLED = A1;   // Low concentration LED (always on)
const int mediumLED = 2; // Medium concentration LED
const int highLED = 3;   // High concentration LED

// Nutritional categories
const char* nutrients[] = {"Calories", "Proteins", "Fats", "Carbs"};

unsigned long lastActionTime;
const unsigned long actionInterval = 5000;  // 5 seconds for listening and displaying

enum State { LISTENING, DISPLAYING };
State currentState = LISTENING;

// Variables to track changes
String lastDishName = "";
int lastNutrientIndex = -1;
int lastConcentrationLevel = -1;

void setup() {
 Serial.begin(115200);  // Initialize serial for debugging
 lcd.begin(16, 2);      // Initialize LCD with 16 columns and 2 rows
 lcd.backlight();       // Turn on the backlight
 lcd.clear();
 delay(100);  // Delay after initializing and clearing
 lcd.setCursor(0, 0);
 lcd.print("Listening...");

 // Initialize switch pins as input with pull-up resistors
 for (int i = 0; i < 6; i++) {
   pinMode(switchPins[i], INPUT_PULLUP);
 }

 // Initialize LED pins as output
 pinMode(lowLED, OUTPUT);
 pinMode(mediumLED, OUTPUT);
 pinMode(highLED, OUTPUT);

 // Ensure the low LED is always on
 digitalWrite(lowLED, HIGH);

 lastActionTime = millis();  // Start the first listening period
 randomSeed(analogRead(0));  // Seed for randomness
}

void loop() {
 // Listen for switch states and update ingredient details
 listenForIngredients();

 // Check and display the nutrient and concentration information if it has changed
 displayNutritionalInfo();

 // Switch between listening and displaying states based on the timer
 if (currentState == LISTENING) {
   if (millis() - lastActionTime >= actionInterval) {
     currentState = DISPLAYING;
     String dishName = generateDishName();
     displayDishName(dishName);
     lastActionTime = millis();  // Reset the timer
   }
 } else if (currentState == DISPLAYING) {
   if (millis() - lastActionTime >= actionInterval) {
     lcd.clear();
     delay(100); // Allow the LCD to reset after clearing
     lcd.setCursor(0, 0);
     lcd.print("Listening...");
     Serial.println("LCD cleared and set to Listening...");
     currentState = LISTENING;
     lastActionTime = millis();  // Reset the timer
     resetIngredients();  // Clear previous ingredient selection
   }
 }
}

// Function to listen for switch states and update selected ingredients continuously
void listenForIngredients() {
 for (int i = 0; i < 6; i++) {
   if (digitalRead(switchPins[i]) == LOW && !selectedIngredients[i]) { // Assuming LOW means switch is ON
     selectedIngredients[i] = true;  // Mark ingredient as selected
     Serial.print("Switch Activated on Pin ");
     Serial.print(switchPins[i]);
     Serial.print(" - Ingredient: ");
     Serial.println(ingredients[i]);
     delay(100);  // Debounce delay
   } else if (digitalRead(switchPins[i]) == HIGH && selectedIngredients[i]) {
     selectedIngredients[i] = false;  // Deselect ingredient when switch is OFF
     Serial.print("Switch Deactivated on Pin ");
     Serial.print(switchPins[i]);
     Serial.print(" - Ingredient: ");
     Serial.println(ingredients[i]);
     delay(100);  // Debounce delay
   }
 }
}

// Function to display the selected dish name on the LCD and Serial Monitor
void displayDishName(String dishName) {
 lcd.clear();
 delay(100);  // Allow the LCD to reset after clearing
 if (dishName == "") {
   lcd.setCursor(0, 0);
   lcd.print("No Dish Selected");
   Serial.println("No Dish Selected on LCD");
 } else {
   lcd.setCursor(0, 0);
   lcd.print("Dish: ");
   lcd.setCursor(0, 1);
   lcd.print(dishName.c_str());  // Use c_str() to ensure null-termination
   Serial.print("Dish Selected: ");
   Serial.println(dishName);
  
   // Only print to serial if the dish has changed
   if (dishName != lastDishName) {
     Serial.print("Dish Selected: ");
     Serial.println(dishName);
     lastDishName = dishName;  // Update the last dish name
   }
 }
}

// Function to generate dish name based on selected ingredients with randomization
String generateDishName() {
 String dish = "Mixed Salad";  // Default if no ingredients are selected

 if (selectedIngredients[0] && selectedIngredients[1]) {
   String riceChickenDishes[] = {"Chicken Rice", "Rice Bowl with Chicken", "Chicken Fried Rice"};
   dish = riceChickenDishes[random(0, 3)];
 }
 else if (selectedIngredients[0]) {
   String riceDishes[] = {"Fried Rice", "Rice Bowl", "Rice Pilaf"};
   dish = riceDishes[random(0, 3)];
 }
 else if (selectedIngredients[1]) {
   String chickenDishes[] = {"Roasted Chicken", "Grilled Chicken", "Chicken Soup"};
   dish = chickenDishes[random(0, 3)];
 }
 else if (selectedIngredients[2] && selectedIngredients[5]) {
   String garlicTomatoDishes[] = {"Garlic Tomato Soup", "Tomato Garlic Pasta", "Garlic Tomato Bruschetta"};
   dish = garlicTomatoDishes[random(0, 3)];
 }
 else if (selectedIngredients[2]) {
   String garlicDishes[] = {"Garlic Bread", "Garlic Butter Shrimp", "Garlic Stir Fry"};
   dish = garlicDishes[random(0, 3)];
 }
 else if (selectedIngredients[3]) {
   String onionDishes[] = {"Onion Soup", "Caramelized Onion Tart", "Onion Rings"};
   dish = onionDishes[random(0, 3)];
 }
 else if (selectedIngredients[4] && selectedIngredients[5]) {
   String cheeseTomatoDishes[] = {"Cheese Tomato Bruschetta", "Tomato Cheese Pizza", "Cheesy Tomato Bake"};
   dish = cheeseTomatoDishes[random(0, 3)];
 }
 else if (selectedIngredients[4]) {
   String cheeseDishes[] = {"Cheese Toast", "Cheese Fondue", "Grilled Cheese"};
   dish = cheeseDishes[random(0, 3)];
 }
 else if (selectedIngredients[5]) {
   String tomatoDishes[] = {"Tomato Soup", "Stuffed Tomatoes", "Tomato Salad"};
   dish = tomatoDishes[random(0, 3)];
 }

 return dish;
}

// Function to determine and display nutrient concentration and type
void displayNutritionalInfo() {
 int potValue = analogRead(potPin);  // Read potentiometer value
 int nutrientIndex = map(potValue, 0, 1023, 0, 3);  // Map to one of 4 nutrients
 int concentrationLevel = map(potValue, 0, 1023, 0, 2);  // Map to low, medium, or high

 // Check if the nutrient or concentration level has changed
 if (nutrientIndex != lastNutrientIndex || concentrationLevel != lastConcentrationLevel) {
   // Display the updated nutrient and concentration level
   Serial.print("Nutrient: ");
   Serial.print(nutrients[nutrientIndex]);
   Serial.print(" - Concentration: ");
   if (concentrationLevel == 0) Serial.println("Low");
   else if (concentrationLevel == 1) Serial.println("Medium");
   else Serial.println("High");

   // Control LED lights based on concentration level
   digitalWrite(lowLED, concentrationLevel == 0 ? HIGH : LOW);
   digitalWrite(mediumLED, concentrationLevel == 1 ? HIGH : LOW);
   digitalWrite(highLED, concentrationLevel == 2 ? HIGH : LOW);

   // Update last printed values
   lastNutrientIndex = nutrientIndex;
   lastConcentrationLevel = concentrationLevel;
 }
}

// Reset selected ingredients for the next listening session
void resetIngredients() {
 for (int i = 0; i < 6; i++) {
   selectedIngredients[i] = false;
 }
}
