import numpy as np
from scipy import ndimage
from PIL import Image
import sys

input_file = sys.argv[1] if len(sys.argv) > 1 else "provinces.bmp"
output_file1 = sys.argv[2] if len(sys.argv) > 2 else "gradient_border_channel.png"
output_file2 = sys.argv[3] if len(sys.argv) > 3 else "gradient_border_channel_inv.png"

print(f"Loading {input_file}...")
region_map = np.array(Image.open(input_file))

print(f"Map size: {region_map.shape}")

# Convert RGB to single index value
print("Converting RGB to region IDs...")
region_ids = region_map[:, :, 0].astype(np.uint32) * 65536 + \
             region_map[:, :, 1].astype(np.uint32) * 256 + \
             region_map[:, :, 2].astype(np.uint32)

unique_count = len(np.unique(region_ids))
print(f"Unique regions: {unique_count}")

# Create a border mask: pixel is 1 if it's on a border, 0 otherwise
print("Finding border pixels...")
height, width = region_ids.shape
borders = np.zeros((height, width), dtype=bool)

for y in range(height):
	if y % 100 == 0:
		print(f"  Scanning row {y}/{height}...")
	
	for x in range(width):
		current_id = region_ids[y, x]
		
		# Check 4 neighbors
		for dy, dx in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
			ny, nx = y + dy, x + dx
			
			# If neighbor is out of bounds or different region, it's a border
			if ny < 0 or ny >= height or nx < 0 or nx >= width:
				borders[y, x] = True
				break
			
			if region_ids[ny, nx] != current_id:
				borders[y, x] = True
				break

print(f"Found {np.sum(borders)} border pixels")

# Calculate distance from each pixel to nearest border
print("Generating distance field...")
distance_field = ndimage.distance_transform_edt(~borders)  # ~ inverts the mask

print("Normalizing...")
distance_field = (distance_field / distance_field.max() * 255).astype(np.uint8)

print(f"Saving Channel 1 to {output_file1}...")
Image.fromarray(distance_field).save(output_file1)

print(f"Saving Channel 2 to {output_file2}...")
inverted = 255 - distance_field
Image.fromarray(inverted).save(output_file2)

print("Done!")