import os
from PIL import Image

def convert_rgba_to_rgb(input_file, output_file):
    # Open the input image
    rgba_image = Image.open(input_file)

    # Convert RGBA to RGB
    rgb_image = rgba_image.convert('RGB')

    # Save the output image
    rgb_image.save(output_file)

# Example usage

for filename in os.listdir('./'):
	if filename.split('.')[-1] == "png":
		full_path = os.path.join('./', filename)
		convert_rgba_to_rgb(full_path, full_path)

