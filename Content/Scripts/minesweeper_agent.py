import sys
import os
import unreal
from langchain_community.llms import LlamaCpp

SCRIPT_DIR = os.path.join(unreal.Paths.project_plugins_dir(), "MinesweeperMind", "Content", "Scripts")
if SCRIPT_DIR not in sys.path:
    sys.path.append(SCRIPT_DIR)
    unreal.log(f"Added {SCRIPT_DIR} to Python module search path.")
try:
    from llm_manager import LLMManager
except ModuleNotFoundError as e:
    unreal.log_error(f"Failed to import llm_manager: {e}")

class MinesweeperAgent:
    """Loads and keeps the LLM in memory for Minesweeper decisions."""

    def __init__(self):
        unreal.log("Initializing Minesweeper Agent LLM...")
        self.llm_manager = LLMManager()
        self.model_path = self.llm_manager.get_model_path()
        
        self.llm = LlamaCpp(
            model_path=self.model_path,
            n_gpu_layers=1,
            n_batch=512,
            n_ctx=2048,
            f16_kv=True,
            verbose=True,
        )
        unreal.log("LLM successfully loaded into RAM.")

    def test_llm():
        """Send a test request to the LLM and log the response."""
        unreal.log("Testing LLM with sample query...")

        generator = GameDimensionGenerator()
        test_query = "Create an expert level Minesweeper grid."
        result = generator.generate_dimensions(test_query)

        unreal.log(f"LLM Response: {result}")

if __name__ == "__main__":
    agent = MinesweeperAgent()
