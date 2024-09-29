const int pinAdc = A0; // Pin connected to the Grove Sound Sensor unsigned long lastCheckTime = 0; // Time of the last averaged check (1 second) unsigned long lastSampleTime = 0; // Time of the last individual sample unsigned long lastAverageCheckTime = 0; // Time of the last 10-second average check const unsigned long checkInterval = 1000; // 1-second interval for averaging const unsigned long averageInterval = 10000; // 10-second interval for calculating average 
const unsigned long sampleInterval = 50; // Sampling sound every 50ms for better accuracy 
float secondSoundSum = 0; // Sum of sound intensities for averaging in 1 second int secondSampleCount = 0; // Count of samples for 1-second averaging float soundSum = 0; // Sum of sound intensities for averaging over 10 seconds int sampleCount = 0; // Count of samples for 10-second averaging 
// RGB LED pin assignments for seamless intensity color (every second) const int redPin1 = 6; 
const int greenPin1 = 3; 
const int bluePin1 = 5; 
// RGB LED pin assignments for average intensity color (every 10 seconds) const int redPin2 = 11; 
const int greenPin2 = 9; 
const int bluePin2 = 10; 
// Fade speed control (dynamically changes based on sound intensity) float fadeSpeed = 0.05; // Default speed 
// Current RGB values for both sets of LEDs 
float currentRed1 = 0, currentGreen1 = 0, currentBlue1 = 0; 
float currentRed2 = 0, currentGreen2 = 0, currentBlue2 = 0; 
// Helper function to set RGB color for LEDs 
void setLEDColor(int redPin, int greenPin, int bluePin, float redValue, float greenValue, float blueValue) { 
analogWrite(redPin, 255 - int(redValue)); // Invert value for common cathode LEDs analogWrite(greenPin, 255 - int(greenValue)); 
analogWrite(bluePin, 255 - int(blueValue)); 
} 
// Logarithmic mapping function to scale intensity 
float logarithmicMap(float value, float minInput, float maxInput, float minOutput, float maxOutput) {
value = constrain(value + 1, minInput, maxInput); // Avoid log(0) 
// Calculate logarithmic scaling (logarithm base 10) 
float logMin = log10(minInput + 1); 
float logMax = log10(maxInput + 1); 
float logValue = log10(value); 
// Linearly map the logarithmic value to the output range 
return minOutput + (logValue - logMin) * (maxOutput - minOutput) / (logMax - logMin); 
} 
// Function to map sound intensity to target RGB values using a logarithmic scale void getTargetColorFromIntensity(float intensity, float &redValue, float &greenValue, float &blueValue) { 
// Map the sound intensity using logarithmic scaling 
redValue = logarithmicMap(intensity, 0, 150, 0, 255); // Red for low intensity greenValue = logarithmicMap(intensity, 151, 300, 0, 255); // Green for medium intensity 
redValue = logarithmicMap(intensity, 250, 400, 255, 0); // Red for high intensity blueValue = logarithmicMap(intensity, 301, 500, 0, 255); // Blue for high intensity } 
// Dynamically adjust fade speed based on sound intensity 
void adjustFadeSpeed(float intensity) { 
fadeSpeed = map(intensity, 0, 500, 0.05, 0.5); // Faster fade for higher intensities 
fadeSpeed = constrain(fadeSpeed, 0.05, 0.5); // Set boundaries for the fade speed } 
// Function to gradually fade between current and target color values void fadeToTargetColor(float &currentRed, float &currentGreen, float &currentBlue, float targetRed, float targetGreen, float targetBlue) { 
currentRed += (targetRed - currentRed) * fadeSpeed; 
currentGreen += (targetGreen - currentGreen) * fadeSpeed; 
currentBlue += (targetBlue - currentBlue) * fadeSpeed; 
} 
// Setup function to initialize the system 
void setup() { 
Serial.begin(115200); // Begin serial communication at 115200 baud rate
// Initialize the RGB LED pins as output for both sets of LEDs 
pinMode(redPin1, OUTPUT); 
pinMode(greenPin1, OUTPUT); 
pinMode(bluePin1, OUTPUT); 
pinMode(redPin2, OUTPUT); 
pinMode(greenPin2, OUTPUT); 
pinMode(bluePin2, OUTPUT); 
Serial.println("System started. Monitoring sound intensity."); 
} 
void loop() { 
unsigned long currentMillis = millis(); 
// Variables for target colors 
float targetRed1, targetGreen1, targetBlue1; 
float targetRed2, targetGreen2, targetBlue2; 
// Continuously sample sound every 50 ms to accumulate readings within the second if (currentMillis - lastSampleTime >= sampleInterval) { 
lastSampleTime = currentMillis; 
// Read the sound intensity 
float soundIntensity = analogRead(pinAdc); 
// Accumulate sound for the current second 
secondSoundSum += soundIntensity; 
secondSampleCount++; 
} 
// Every second, calculate the average intensity for the past second if (currentMillis - lastCheckTime >= checkInterval) { 
lastCheckTime = currentMillis; // Update the last check time 
// Calculate average sound intensity over the previous second 
if (secondSampleCount > 0) { 
float secondAverageIntensity = secondSoundSum / secondSampleCount; Serial.print("Average Sound Intensity over 1 second: "); 
Serial.println(secondAverageIntensity); 
// Adjust fade speed based on intensity 
adjustFadeSpeed(secondAverageIntensity);
// Set target color based on this average intensity for first set of LEDs getTargetColorFromIntensity(secondAverageIntensity, targetRed1, 
targetGreen1, targetBlue1); 
// Accumulate for 10-second average 
soundSum += secondAverageIntensity; 
sampleCount++; 
} 
// Reset second-sampling accumulators for the next second 
secondSoundSum = 0; 
secondSampleCount = 0; 
} 
// Check the average sound intensity every 10 seconds (for second LED set) if (currentMillis - lastAverageCheckTime >= averageInterval) { 
lastAverageCheckTime = currentMillis; // Update the last check time for average 
if (sampleCount > 0) { // Ensure we don't divide by zero 
// Calculate the average sound intensity over the last 10 seconds float averageIntensity = soundSum / sampleCount; 
Serial.print("Average Sound Intensity over 10 seconds: "); 
Serial.println(averageIntensity); 
// Set target color based on average sound intensity for second set of LEDs getTargetColorFromIntensity(averageIntensity, targetRed2, targetGreen2, targetBlue2); 
// Reset sound sum and sample count for the next 10-second interval soundSum = 0; 
sampleCount = 0; 
} else { 
Serial.println("No samples collected in the last 10 seconds."); 
} 
} 
// Gradually fade both sets of LEDs to their respective target colors fadeToTargetColor(currentRed1, currentGreen1, currentBlue1, targetRed1, targetGreen1, targetBlue1);
setLEDColor(redPin1, greenPin1, bluePin1, currentRed1, currentGreen1, currentBlue1); 
fadeToTargetColor(currentRed2, currentGreen2, currentBlue2, targetRed2, targetGreen2, targetBlue2); 
setLEDColor(redPin2, greenPin2, bluePin2, currentRed2, currentGreen2, currentBlue2); 
delay(10); // Small delay to stabilize readings and make the fading effect smooth }
