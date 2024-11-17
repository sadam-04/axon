import os

def generate_header(input_file, output_header):
    # Check if the input file exists
    if not os.path.isfile(input_file):
        print(f"Error: The file '{input_file}' does not exist.")
        return

    # Read the binary data from the input file
    with open(input_file, 'rb') as f:
        binary_data = f.read()

    # Convert binary data to a C++-compatible byte array
    byte_array = ', '.join(f'(char)0x{byte:02x}' for byte in binary_data)

    # Get the variable name based on the input file name
    variable_name = os.path.basename(input_file).replace('.', '_').replace('-', '_')

    # Generate the header content
    header_content = f"""\
#ifndef {variable_name.upper()}_H
#define {variable_name.upper()}_H

// Automatically generated from {input_file}
// Size of the binary data: {len(binary_data)} bytes
const char {variable_name}[] = {{
    {byte_array}
}};

const unsigned int {variable_name}_size = {len(binary_data)};

#endif // {variable_name.upper()}_H
"""

    # Write the header content to the output header file
    with open(output_header, 'w') as f:
        f.write(header_content)

    print(f"Header file '{output_header}' has been generated.")

# Example usage
if __name__ == "__main__":
    input_file = "input_file.bin"  # Replace with your input file path
    output_header = "output_header.h"  # Replace with your desired output header file name
    generate_header(input_file, output_header)

generate_header("GeistMono-SemiBold.ttf", "font.hpp")