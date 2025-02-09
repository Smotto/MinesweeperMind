from game_dimension_generator import GameDimensionGenerator
import json

def get_minesweeper_dimensions():
    """Fetches game dimensions for Unreal Engine."""
    generator = GameDimensionGenerator()
    result = generator.generate_dimensions("Generate an expert Minesweeper grid")
    return json.dumps(result)

if __name__ == "__main__":
    print(get_minesweeper_dimensions())
