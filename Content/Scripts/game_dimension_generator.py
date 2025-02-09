from langchain_community.llms import LlamaCpp
from langchain_core.callbacks import CallbackManager, StreamingStdOutCallbackHandler
from langchain.chains import LLMChain
from langchain.output_parsers import StructuredOutputParser, ResponseSchema
from langchain.prompts import ChatPromptTemplate
from llm_manager import LLMManager
import json
import unreal

class GameDimensionGenerator:
    """Handles querying the LLM to generate Minesweeper dimensions."""
    
    def __init__(self):
        self.llm_manager = LLMManager()
        self.model_path = self.llm_manager.get_model_path()
        
        self.llm = LlamaCpp(
            model_path=self.model_path,
            n_gpu_layers=1,
            n_batch=512,
            n_ctx=2048,
            f16_kv=True,
            callback_manager=CallbackManager([StreamingStdOutCallbackHandler()]),
            verbose=True,
        )

        # Define JSON response schema
        self.response_schemas = [
            ResponseSchema(name="rows", description="Number of rows in the grid."),
            ResponseSchema(name="columns", description="Number of columns in the grid."),
            ResponseSchema(name="mines", description="Number of mines."),
        ]
        self.output_parser = StructuredOutputParser.from_response_schemas(self.response_schemas)
        self.format_instructions = self.output_parser.get_format_instructions()

        # Prompt template
        self.custom_prompt = ChatPromptTemplate.from_template(
            f"""You are a game dimension generator for Minesweeper.
            Extract grid dimensions and mine count.
            Return only valid JSON with keys: "rows", "columns", "mines".
            {self.format_instructions}
            User request: {{input}}
            Answer:"""
        )

        self.chain = LLMChain(llm=self.llm, prompt=self.custom_prompt)

    def generate_dimensions(self, query: str) -> dict:
        """Queries the LLM and returns parsed JSON."""
        raw_output = self.chain.run({"input": query})
        try:
            parsed = self.output_parser.parse(raw_output)
            return parsed
        except Exception as e:
            unreal.log_error(f"Parsing failed: {e}")
            return {}

if __name__ == "__main__":
    generator = GameDimensionGenerator()
    print(json.dumps(generator.generate_dimensions("Create an expert level Minesweeper game"), indent=4))
