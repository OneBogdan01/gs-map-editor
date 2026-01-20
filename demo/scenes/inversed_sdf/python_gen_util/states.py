import numpy as np
from scipy import ndimage
from PIL import Image
import sys

input_file = sys.argv[1] if len(sys.argv) > 1 else "states.png"
output_file1 = sys.argv[2] if len(sys.argv) > 2 else "state_gradient_border_channel.png"
output_file2 = sys.argv[3] if len(sys.argv) > 3 else "state_gradient_border_channel_inv.png"

print(f"Loading {input_file}...")
img = Image.open(input_file).convert("RGB")
region_map = np.array(img)

# 1. Identify Yellow (RGB: 255, 255, 0)
# We create a boolean mask where True means "This is the background"
yellow_mask = (region_map[:,:,0] == 255) & (region_map[:,:,1] == 255) & (region_map[:,:,2] == 0)

# 2. Convert RGB to single index value
region_ids = region_map[:, :, 0].astype(np.uint32) * 65536 + \
             region_map[:, :, 1].astype(np.uint32) * 256 + \
             region_map[:, :, 2].astype(np.uint32)

# 3. FAST BORDER DETECTION (NumPy Vectorized)
print("Finding border pixels (excluding yellow)...")
# Shift the array in 4 directions to find where IDs change
# We compare the ID map to itself shifted by 1 pixel
borders = np.zeros(region_ids.shape, dtype=bool)

# Check vertical and horizontal differences
diff_h = region_ids[:, 1:] != region_ids[:, :-1]
diff_v = region_ids[1:, :] != region_ids[:-1, :]

# Apply differences to border mask
borders[:, :-1] |= diff_h
borders[:, 1:]  |= diff_h
borders[:-1, :] |= diff_v
borders[1:, :]  |= diff_v

# 4. EXCLUDE YELLOW FROM DISTANCE FIELD
# We don't want the distance field to calculate inside the yellow areas.
# We set yellow areas to be "True" in the mask so the distance field 
# treats them as a wall/boundary, then we will black them out later.
search_mask = ~borders
search_mask[yellow_mask] = False # Tell the EDT that yellow is "outside"

print("Generating distance field...")
distance_field = ndimage.distance_transform_edt(search_mask)

# 5. Normalizing
print("Normalizing...")
max_dist = distance_field.max()
if max_dist > 0:
    distance_field = (distance_field / max_dist * 255).astype(np.uint8)
else:
    distance_field = distance_field.astype(np.uint8)

# 6. Apply Yellow Mask to Output
# This ensures the background remains black (0) in the final image
distance_field[yellow_mask] = 0

print(f"Saving to {output_file1} and {output_file2}...")
Image.fromarray(distance_field).save(output_file1)
Image.fromarray(255 - distance_field).save(output_file2)

print("Done!")