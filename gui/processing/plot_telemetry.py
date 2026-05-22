import matplotlib.pyplot as plt 
import os
import glob
import pandas as pd

# --- Configuration ---
TELEMETRY_PATH = "../logs/telemetry/*.csv"
POWER_PATH = "../logs/power/*.xlsx" 

start_crop = 70
end_crop = 130

def get_latest_file(path):
    list_of_files = glob.glob(path)
    if not list_of_files:
        return None
    return max(list_of_files, key=os.path.getctime)

latest_tel_file = get_latest_file(TELEMETRY_PATH)
latest_pwr_file = get_latest_file(POWER_PATH)

# --- 1. Process Telemetry Data (Keep 1s Averaging) ---
if latest_tel_file:
    df_tel = pd.read_csv(latest_tel_file, header=None, 
                         names=['time_str', 'depth', 'ref_depth', 'control_vol', 'piston_vol'])
    df_tel['datetime'] = pd.to_datetime(df_tel['time_str'])
    # Resample telemetry to clean up the staircase look
    df_tel = df_tel.set_index('datetime').resample('1s').mean(numeric_only=True).reset_index()
    df_tel = df_tel.sort_values('datetime')
else:
    print("No telemetry files found.")
    exit()

# --- 2. Process Power Data (NO RESAMPLING) ---
if latest_pwr_file:
    df_pwr = pd.read_excel(latest_pwr_file, header=0, usecols=[1, 2, 4, 5])
    df_pwr.columns = ['voltage', 'current', 'wh', 'time_str']
    df_pwr['datetime'] = pd.to_datetime(df_pwr['time_str'], format='%Y-%m-%d %H:%M:%S', errors='coerce')
    df_pwr = df_pwr.dropna(subset=['datetime']).sort_values('datetime')
    
    # Calculate raw power before any smoothing
    df_pwr['power_adj'] = (df_pwr['voltage'] * df_pwr['current']) - 0.85
    # Smooth the raw data directly
    df_pwr['power_smooth'] = df_pwr['power_adj'].rolling(window=10, center=True).mean()
else:
    print(f"No Excel power files found.")
    exit()

# --- 3. Synchronize / Intersection using merge_asof ---
# This aligns the power data to the nearest telemetry second without creating gaps
df_combined = pd.merge_asof(df_tel, df_pwr, on='datetime', direction='nearest')

# 1. Identify when the RAW telemetry started (before resampling)
# We use df_tel from before the .set_index().resample() lines
tel_actual_start = pd.to_datetime(pd.read_csv(latest_tel_file, header=None, usecols=[0])[0]).min()

# 2. Calculate Target Start
target_start_time = tel_actual_start + pd.Timedelta(seconds=start_crop)

# 3. Find index in the 10Hz data
# We re-read the raw time column specifically to find the high-res index
raw_times = pd.to_datetime(pd.read_csv(latest_tel_file, header=None, usecols=[0])[0])
target_idx_raw = (raw_times - target_start_time).abs().idxmin()

# 4. Result for MATLAB
matlab_start_idx = target_idx_raw + 1

print("-" * 30)
print(f"RAW 10Hz Telemetry Start: {tel_actual_start}")
print(f"MATLAB START INDEX (at 10Hz): {matlab_start_idx}")
print("-" * 30)

# Calculate Initial Elapsed Seconds
df_combined['seconds'] = (df_combined['datetime'] - df_combined['datetime'].iloc[0]).dt.total_seconds()

# --- 4. CROP TO WINDOW (70s to 130s) ---

mask = (df_combined['seconds'] >= start_crop) & (df_combined['seconds'] <= end_crop)
df_window = df_combined.loc[mask].copy()

if df_window.empty:
    print("Error: Window outside available range.")
    exit()

# FIX: Subtract the first value of the window to force the X-axis to start at 0
df_window['seconds_rel'] = df_window['seconds'] - df_window['seconds'].iloc[0]

# Keep energy relative to the start of the window
df_window['wh_rel'] = df_window['wh'] - df_window['wh'].iloc[0]

# --- 5. Plotting ---

# Figure 1: Depth
plt.figure(figsize=(10, 5))
plt.plot(df_window['seconds_rel'], df_window['depth'], color='b', label="Actual Depth")
plt.plot(df_window['seconds_rel'], df_window['ref_depth'], color='k', linestyle='--', label="Reference")
plt.ylabel('Depth (m)')
plt.xlabel('Time (s)')
plt.title('Depth Tracking')
plt.legend()
plt.grid(True, alpha=0.3)

# Figure 2: VBS Actuator
plt.figure(figsize=(10, 5))
plt.plot(df_window['seconds_rel'], df_window['control_vol'], color='b', label="Control (Target)")
plt.plot(df_window['seconds_rel'], df_window['piston_vol'], color='r', label="Piston (Actual)")
plt.ylabel('Volume (mL)')
plt.xlabel('Time (s)')
plt.title('VBS Actuator Position')
plt.legend()
plt.grid(True, alpha=0.3)

# Figure 3: Power (Smooth and Gap-free)
plt.figure(figsize=(10, 5))
plt.plot(df_window['seconds_rel'], df_window['power_smooth'], color='b', label="Net System Power")
plt.ylabel('Power (Watts)')
plt.xlabel('Time (s)')
plt.title('Power Consumption')
plt.ylim(bottom=0)
plt.grid(True, alpha=0.3)
plt.legend()

plt.figure(figsize=(10, 5))
plt.plot(df_window['seconds_rel'], df_window['wh_rel'], color='b', label="Energy Consumed")
plt.ylabel('Energy (Wh)')
plt.xlabel('Time (s)')
plt.title('Energy Consumption')
plt.grid(True, alpha=0.3)
plt.legend()

plt.show()