"""
Speech recognition module using Google Speech Recognition.
For more advanced use, consider whisper.
"""

import speech_recognition as sr

def listen():
    """
    Listens to microphone input and returns transcribed text.
    """
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        print("Escuchando...")
        try:
            audio = recognizer.listen(source, timeout=5, phrase_time_limit=10)
            text = recognizer.recognize_google(audio, language='es-ES')
            print(f"Transcrito: {text}")
            return text
        except sr.WaitTimeoutError:
            print("No se detectó audio.")
            return ""
        except sr.UnknownValueError:
            print("No se pudo entender el audio.")
            return ""
        except sr.RequestError as e:
            print(f"Error en el servicio de reconocimiento: {e}")
            return ""