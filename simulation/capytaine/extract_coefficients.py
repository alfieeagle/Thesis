import numpy as np
import xarray as xr

# 1. Load your dataset
ds = xr.open_dataset("capy_dataset.nc")

# 2. Find the index for omega = 1.0 (or your target frequency)
target_omega = 1.0
omegas = ds.omega.values
idx = (np.abs(omegas - target_omega)).argmin()

# 3. List of the 6 Degrees of Freedom
dofs = ['Surge', 'Sway', 'Heave', 'Roll', 'Pitch', 'Yaw']

print(f"6x6 Added Mass Matrix at {omegas[idx]:.2f} rad/s")
print("-" * 60)

# Create an empty 6x6 numpy array to store values
am_matrix = np.zeros((6, 6))

# 4. Nested Loop to fill the matrix
for i, radiating in enumerate(dofs):
    for j, influenced in enumerate(dofs):
        try:
            val = ds.added_mass.isel(omega=idx).sel(
                radiating_dof=radiating, 
                influenced_dof=influenced
            ).values
            am_matrix[i, j] = val
        except KeyError:
            # Fallback if names are 1, 2, 3 instead of strings
            am_matrix[i, j] = ds.added_mass.isel(omega=idx, radiating_dof=i, influenced_dof=j).values

# 5. Print the matrix in a clean format
# This uses a header for clarity
header = " ".join([f"{d:>10}" for d in dofs])
print(f"{' ':<10} {header}")

for i, row_label in enumerate(dofs):
    row_str = " ".join([f"{val:10.4f}" for val in am_matrix[i]])
    print(f"{row_label:<10} {row_str}")