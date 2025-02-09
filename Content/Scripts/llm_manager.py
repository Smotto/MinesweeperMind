import os
import hashlib
import requests
import unreal

class LLMManager:
    """Handles LLM Model Setup, Download, and Management."""

    MODEL_URL = "https://huggingface.co/TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF/resolve/main/tinyllama-1.1b-chat-v1.0.Q4_K_M.gguf"
    TARGET_FILENAME = "tinyllama-1.1b-chat-v1.0.Q4_K_M.gguf"
    EXPECTED_SHA256 = "9fecc3b3cd76bba89d504f29b616eedf7da85b96540e490ca5824d3f7d2776a0"

    def __init__(self, plugin_name="MinesweeperMind"):
        self.plugin_root = os.path.join(unreal.Paths.project_plugins_dir(), plugin_name)
        self.models_dir = os.path.join(self.plugin_root, "Content", "LargeLanguageModels")

        if not os.path.exists(self.models_dir):
            os.makedirs(self.models_dir)
            unreal.log(f"Created models directory: {self.models_dir}")

        self.model_path = os.path.join(self.models_dir, self.TARGET_FILENAME)
        self.ensure_model_exists()

    def ensure_model_exists(self):
        """Ensures that the LLM model file exists and is verified."""
        if os.path.exists(self.model_path):
            unreal.log(f"Model file already exists: {self.model_path}")
            return
        
        unreal.log(f"Downloading LLM model from {self.MODEL_URL}...")
        try:
            with requests.get(self.MODEL_URL, stream=True) as response:
                response.raise_for_status()
                with open(self.model_path, "wb") as f:
                    for chunk in response.iter_content(chunk_size=8192):
                        f.write(chunk)
            unreal.log("Download complete.")
        except Exception as e:
            unreal.log_error(f"Model download failed: {e}")
            return
        
        # Verify SHA256 hash
        with open(self.model_path, "rb") as f:
            computed_hash = hashlib.sha256(f.read()).hexdigest()
        if computed_hash.lower() != self.EXPECTED_SHA256.lower():
            unreal.log_error("SHA256 mismatch! Downloaded model is corrupted.")
            os.remove(self.model_path)
            return
        unreal.log("SHA256 verification successful.")

    def get_model_path(self):
        """Returns the LLM model path."""
        return self.model_path
