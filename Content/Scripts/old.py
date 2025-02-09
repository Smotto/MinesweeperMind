import os
import hashlib
import requests
import unreal

def get_plugin_paths(plugin_name="MinesweeperMind"):
    """
    Retrieve key directories for the plugin.
    Returns a tuple: (plugin_root, scripts_dir, models_dir)
    """
    plugin_root = os.path.join(unreal.Paths.project_plugins_dir(), plugin_name)
    did_succeed, out_reason = unreal.Paths.validate_path(plugin_root)
    if not did_succeed:
        raise ValueError(f"Invalid plugin path: {out_reason}")

    scripts_dir = os.path.join(plugin_root, "Content", "Scripts")
    did_succeed, out_reason = unreal.Paths.validate_path(scripts_dir)
    if not did_succeed:
        raise ValueError(f"Invalid scripts path: {out_reason}")

    models_dir = os.path.join(plugin_root, "Content", "LargeLanguageModels")
    did_succeed, out_reason = unreal.Paths.validate_path(models_dir)
    if not did_succeed:
        raise ValueError(f"Invalid models path: {out_reason}")
    
    return plugin_root, scripts_dir, models_dir

def download_model_if_missing(model_url, target_path, expected_sha256=None):
    """
    Download a model from a secure URL if it doesn't already exist at target_path.
    
    Args:
        model_url (str): HTTPS URL for the model.
        target_path (str): Local file path where the model should be saved.
        expected_sha256 (str, optional): Expected SHA256 hash for file verification.
        
    Returns:
        bool: True if the file exists or was successfully downloaded (and verified), False otherwise.
    """
    if os.path.exists(target_path):
        unreal.log(f"Model file already exists at: {target_path}")
        return True

    unreal.log(f"Downloading model from {model_url} to {target_path}...")

    try:
        with requests.get(model_url, stream=True) as response:
            response.raise_for_status()
            with open(target_path, "wb") as f:
                for chunk in response.iter_content(chunk_size=8192):
                    f.write(chunk)
        unreal.log("Download complete.")
    except Exception as e:
        unreal.log_error(f"Error downloading model: {e}")
        return False

    # Verify file hash if an expected hash is provided.
    if expected_sha256:
        with open(target_path, "rb") as f:
            computed_hash = hashlib.sha256(f.read()).hexdigest()

        if computed_hash.lower() != expected_sha256.lower():
            unreal.log_error(f"SHA256 mismatch! Expected {expected_sha256.lower()}, but got {computed_hash.lower()}.")
            return False
        unreal.log("SHA256 hash verified successfully.")

    return True

def setup_global_llm_model():
    """
    Ensures the model file exists in the plugin's models directory.
    Returns the full path to the model file if successful, or None otherwise.
    """
    _, _, models_dir = get_plugin_paths()

    if not os.path.exists(models_dir):
        os.makedirs(models_dir)
        unreal.log(f"Created models directory at: {models_dir}")

    MODEL_URL = "https://huggingface.co/TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF/resolve/main/tinyllama-1.1b-chat-v1.0.Q4_K_M.gguf"
    TARGET_FILENAME = "tinyllama-1.1b-chat-v1.0.Q4_K_M.gguf"
    target_path = os.path.join(models_dir, TARGET_FILENAME)
    EXPECTED_SHA256 = "9fecc3b3cd76bba89d504f29b616eedf7da85b96540e490ca5824d3f7d2776a0"

    if download_model_if_missing(MODEL_URL, target_path, expected_sha256=EXPECTED_SHA256):
        unreal.log(f"Global LLM model is ready at: {target_path}")
        return target_path

    unreal.log_error("Failed to download or verify the global LLM model.")
    return None

GLOBAL_MODEL_PATH = None

def initialize_global_model():
    """
    Initializes the global model path if it hasn't been set.
    """
    global GLOBAL_MODEL_PATH
    if GLOBAL_MODEL_PATH is None:
        GLOBAL_MODEL_PATH = setup_global_llm_model()
    return GLOBAL_MODEL_PATH

def get_global_model_path():
    """
    Getter for the global model file path.
    """
    if GLOBAL_MODEL_PATH is None:
        unreal.log_warning("Global model path has not been initialized yet. Call initialize_global_model() first.")
    return GLOBAL_MODEL_PATH

# Ensure the global model is set up.
initialize_global_model()
model_file_path = get_global_model_path()
if not model_file_path:
    unreal.log_error("Global model path is not available!")
else:
    unreal.log("Using model file at: {}".format(model_file_path))

# --------------------------------------------------------------------------
# Set up the LLM using LlamaCpp
from langchain_community.llms import LlamaCpp
from langchain_core.callbacks import CallbackManager, StreamingStdOutCallbackHandler

GLOBAL_LLM = LlamaCpp(
    model_path=model_file_path,
    n_gpu_layers=1,
    n_batch=512,
    n_ctx=2048,
    f16_kv=True,
    callback_manager=CallbackManager([StreamingStdOutCallbackHandler()]),
    verbose=True,
)

# --------------------------------------------------------------------------
# Use an LLMChain to generate game dimensions based on a user's request.
from langchain.chains import LLMChain
from langchain.output_parsers import StructuredOutputParser, ResponseSchema
from langchain.prompts import ChatPromptTemplate

# Define the expected JSON output schema.
response_schemas = [
    ResponseSchema(name="rows", description="Number of rows in the Minesweeper grid."),
    ResponseSchema(name="columns", description="Number of columns in the Minesweeper grid."),
    ResponseSchema(name="mine_count", description="Number of mines to place on the grid."),
]
output_parser = StructuredOutputParser.from_response_schemas(response_schemas)
format_instructions = output_parser.get_format_instructions()

# Escape curly braces in format_instructions so they are treated literally.
escaped_format_instructions = format_instructions.replace("{", "{{").replace("}", "}}")

# Create a custom prompt that instructs the LLM to output strictly JSON with the required keys.
custom_prompt = ChatPromptTemplate.from_template(
    f"""You are a game dimension generator for Minesweeper.
Based on the user's request, extract the grid dimensions and mine count.
Your output must be a valid JSON object with exactly three keys: "rows", "columns", and "mines" (not "mine_count").
Do not include any additional text.
{escaped_format_instructions}

User request: {{input}}
Answer:"""
)

# Create the LLMChain
chain = LLMChain(llm=GLOBAL_LLM, prompt=custom_prompt)

def get_game_dimensions(query: str) -> dict:
    raw_output = chain.run({"input": query})
    try:
        parsed = output_parser.parse(raw_output)
        # Normalize the output by renaming 'mine_count' to 'mines'
        if "mine_count" in parsed:
            parsed["mines"] = parsed.pop("mine_count")
        return parsed
    except Exception as e:
        unreal.log_error(f"Error parsing output: {e}")
        return {}

# Example usage:
if __name__ == "__main__":
    user_query = "Create an expert level Minesweeper game"
    dimensions = get_game_dimensions(user_query)
    unreal.log(f"Extracted game dimensions: {dimensions}")
    print(dimensions)
    print()
    user_query = "Create an medium level Minesweeper game"
    dimensions = get_game_dimensions(user_query)
    unreal.log(f"Extracted game dimensions: {dimensions}")
    print(dimensions)
    print()
    user_query = "Create an easy level Minesweeper game"
    dimensions = get_game_dimensions(user_query)
    unreal.log(f"Extracted game dimensions: {dimensions}")
    print(dimensions)
    print()
    user_query = "10 10 15"
    dimensions = get_game_dimensions(user_query)
    unreal.log(f"Extracted game dimensions: {dimensions}")
    print(dimensions)
    print()
    user_query = "hey fam squad can you generate a 3x3 grid with 2 mines?"
    dimensions = get_game_dimensions(user_query)
    unreal.log(f"Extracted game dimensions: {dimensions}")
    print(dimensions)
    