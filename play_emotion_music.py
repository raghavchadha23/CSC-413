import serial
import time
import vlc

# Define emotional states and corresponding music files
music_files = {
    "LowEnergy": "Calm_Supportive.mp3",
    "Optimistic": "Upbeat_Energizing.mp3",
    "Stressed": "Relaxing_Calming.mp3",
    "Focused": "Steady_Inspired.mp3",
    "Excited": "Celebratory_Uplifting.mp3"
}

# Initialize serial communication
serial_port = '/dev/cu.usbmodem1101'  # Replace with your Arduino port
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate, timeout=1)

print(f"Connected to {serial_port} at {baud_rate} baud.")
print("Listening for data from Arduino...\n")

def play_music(state):
    """Play the music corresponding to the emotional state."""
    if state in music_files:
        print(f"\nPlaying music for state: {state}")
        player = vlc.MediaPlayer(music_files[state])
        player.play()
        time.sleep(60)  # Play for 1 minute
        player.stop()
    else:
        print(f"No music file found for state: {state}")

# Dictionary to store person states and collective state
person_states = {}
team_state = ""
brainstorming_prompt = ""

# Read data from Arduino
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        print(f"Received: {line}")

        if line.startswith("PlayMusic:"):
            # Extract the state and play music
            state = line.split(":")[1]
            play_music(state)

        # Parse the data for person and state
        if line.startswith("Person:"):
            parts = line.split(", State: ")
            if len(parts) == 2:
                person = parts[0].split(": ")[1]
                state = parts[1]
                person_states[person] = state

        # Check for "Team Emotional State"
        if "Team Emotional State:" in line:
            team_state = line.split(": ")[1]

        # Check for "Brainstorming Prompt"
        if "Brainstorming Prompt:" in line:
            brainstorming_prompt = line.split(": ")[1]
            print(f"Brainstorming Prompt: {brainstorming_prompt}")

        # Check for "END" to signal all data is sent
        if line == "END":
            break  # Exit the loop when we receive "END"

# After receiving "END", display the results
print("\nSurvey completed. Here are the results:")
final_states = []
for idx, (person, state) in enumerate(person_states.items(), start=1):
    print(f"{idx}. {person}: {state}")
    final_states.append((person, state))

print(f"Team Emotional State: {team_state}")
print(f"Brainstorming Prompt: {brainstorming_prompt}")
