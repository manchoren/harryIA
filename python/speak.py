"""
Text-to-speech module using pyttsx3 (Python TTS library).
"""

import pyttsx3
import threading
from utils import normalize_text

# Initialize the TTS engine
engine = pyttsx3.init()

# Configure voice (try to set Spanish if available)
voices = engine.getProperty('voices')
spanish_voice = None
for voice in voices:
    if 'spanish' in voice.name.lower() or 'español' in voice.name.lower() or 'es' in voice.name.lower():
        spanish_voice = voice
        break
    elif 'microsoft' in voice.name.lower() and ('spanish' in voice.name.lower() or 'es' in voice.name.lower()):
        spanish_voice = voice
        break

if spanish_voice:
    engine.setProperty('voice', spanish_voice.id)
    print(f"Usando voz en español: {spanish_voice.name}")
else:
    print("No se encontró una voz en español. Usando voz predeterminada.")

# Configure speech rate and volume for better pronunciation
engine.setProperty('rate', 150)  # Adjust speed (default is around 200, lower is slower)
engine.setProperty('volume', 0.9)  # Volume level (0.0 to 1.0)

# Lock to prevent multiple simultaneous speaks
speak_lock = threading.Lock()

def speak_async(text):
    """
    Speaks the text asynchronously using pyttsx3.
    """
    with speak_lock:
        normalized = normalize_text(text)
        engine.say(normalized)
        engine.runAndWait()

def speak(text):
    """
    Starts speaking in a separate thread.
    """
    thread = threading.Thread(target=speak_async, args=(text,))
    thread.daemon = True
    thread.start()