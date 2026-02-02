"""
Utility functions for HarryIA Python version.
Includes memory management, text processing, and AI interaction.
"""

import json
import re
import os
import requests
from time import sleep

def delay_print(text, delay_ms=10):
    """
    Prints text with a delay between characters for dramatic effect.
    """
    for char in text:
        print(char, end='', flush=True)
        sleep(delay_ms / 1000)
    print()

def cargar_memoria(config):
    """
    Loads memory from the memory file.
    If the memory file is empty or doesn't exist, fills it with content from start.txt.
    """
    memory_file = os.path.join(os.path.dirname(__file__), '..', config.MEMORY_FILE)
    start_file = os.path.join(os.path.dirname(__file__), '..', config.START_FILE)
    
    # Check if memory file exists and has content
    if os.path.exists(memory_file):
        with open(memory_file, 'r', encoding='utf-8') as f:
            content = f.read()
            if content.strip():  # If not empty
                return content
    
    # If memory is empty or doesn't exist, try to fill it with start.txt
    if os.path.exists(start_file):
        with open(start_file, 'r', encoding='utf-8') as f:
            start_content = f.read()
        # Write start content to memory file
        with open(memory_file, 'w', encoding='utf-8') as f:
            f.write(start_content)
        return start_content
    
    return ""

def guardar_memoria(text, config):
    """
    Appends text to the memory file.
    """
    memory_file = os.path.join(os.path.dirname(__file__), '..', config.MEMORY_FILE)
    with open(memory_file, 'a', encoding='utf-8') as f:
        f.write(text + '\n')

def interactuar_con_modelo(prompt, config):
    """
    Interacts with the AI model via HTTP POST.
    """
    url = config.API_URL + config.API_ENDPOINT
    payload = {
        "model": "local-model",
        "stream": False,
        "max_tokens": 300,
        "temperature": 0.7,
        "stop": ["</s>", "\\n", "\\r", "USUARIO:"],
        "repeat_last_n": 256,
        "repeat_penalty": 1.2,
        "top_k": 40,
        "top_p": 0.95,
        "min_p": 0.05,
        "tfs_z": 1,
        "typical_p": 1,
        "presence_penalty": 0,
        "frequency_penalty": 0,
        "mirostat": 0,
        "mirostat_tau": 5,
        "mirostat_eta": 0.1,
        "grammar": "",
        "n_probs": 0,
        "min_keep": 0,
        "cache_prompt": True,
        "api_key": "",
        "slot_id": -1,
        "prompt": prompt
    }
    try:
        response = requests.post(url, json=payload, timeout=600)
        response.raise_for_status()
        return response.json()
    except requests.RequestException as e:
        return {"error": "connection_error", "message": str(e)}

def procesar_json(response):
    """
    Processes the JSON response from the AI model.
    """
    if isinstance(response, dict) and 'content' in response:
        return response['content']
    elif isinstance(response, dict) and 'error' in response:
        return f"Error: {response['message']}"
    return str(response)

def limpiar_respuesta_ia(text, remove_newlines=True):
    """
    Cleans the AI response by removing unwanted patterns.
    """
    # Remove specific patterns if needed
    text = re.sub(r'HARRY:', '', text)
    text = re.sub(r'USUARIO:', '', text)
    if remove_newlines:
        text = text.replace('\n', ' ')
    return text.strip()

def normalize_text(text):
    """
    Normalizes text for TTS: removes punctuation, replaces accents if needed.
    """
    # Remove punctuation
    text = re.sub(r'[.,;!?¿¡]', ' ', text)
    # Remove quotes
    text = text.replace('"', '').replace("'", '')
    # Replace HARRY with JARRY
    text = text.replace('HARRY', 'JARRY')
    return text