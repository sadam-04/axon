from PIL import Image
import numpy as np

def convert_icon_to_rgba(icon_path, output_header, size=(32, 32)):
    # Open the icon file
    icon = Image.open(icon_path)
    
    # Resize to the desired size if necessary
    icon = icon.resize(size, Image.LANCZOS)
    
    # Convert to RGBA
    icon = icon.convert("RGBA")
    
    # Extract pixel data
    pixel_data = np.array(icon).flatten()
    
    # Write the byte array to a C++ header
    with open(output_header, 'w') as header_file:
        header_file.write("// Auto-generated RGBA icon data\n")
        header_file.write(f"const int ICON_WIDTH = {size[0]};\n")
        header_file.write(f"const int ICON_HEIGHT = {size[1]};\n")
        header_file.write("const unsigned char ICON_RGBA[] = {\n")
        
        # Write pixel data as a comma-separated list
        header_file.write(",\n".join(
            ", ".join(str(value) for value in pixel_data[i:i + 12])
            for i in range(0, len(pixel_data), 12)
        ))
        header_file.write("\n};\n")

    print(f"Header file generated: {output_header}")

# Example usage
convert_icon_to_rgba("icon.ico", "rgba_axon_icon.h")
