import matplotlib.pyplot as plt
import numpy as np
from rosbags.highlevel import AnyReader
from pathlib import Path
import os

# Physical Constants
RHO = 1025.0
G = 9.81
MASS = 2.019667085
ADDED_MASS = 0.39272 
TOTAL_INERTIA = MASS + ADDED_MASS
F_GRAVITY = MASS * G

base_path = Path('~/Documents/Thesis/simulation/logging/').expanduser()
all_bags = [f for f in base_path.iterdir() if f.is_dir()]
if not all_bags:
    print("No bags found.")
    exit()

bag_path = max(all_bags, key=os.path.getmtime)
print(f"Reading bag: {bag_path}")

# Topics
piston_topic = '/piston_volume'
imu_topic = '/vbs_imu' 

p_times, p_vols = [], []
imu_times, imu_accel = [], []

with AnyReader([bag_path]) as reader:
    p_conns = [x for x in reader.connections if x.topic == piston_topic]
    imu_conns = [x for x in reader.connections if x.topic == imu_topic]
    
    try:
        all_msgs = reader.messages(connections=p_conns + imu_conns)
        _, t0, _ = next(all_msgs)
    except StopIteration:
        print("Required topics not found in bag.")
        exit()

    for conn, ts, raw in reader.messages(connections=p_conns + imu_conns):
        msg = reader.deserialize(raw, conn.msgtype)
        rel_time = (ts - t0) / 1e9

        if conn.topic == piston_topic:
            p_times.append(rel_time)
            p_vols.append(msg.data)
        elif conn.topic == imu_topic:
            imu_times.append(rel_time)
            imu_accel.append(msg.linear_acceleration.z - G)

# --- Alignment ---
common_time = np.linspace(max(p_times[0], imu_times[0]), 
                          min(p_times[-1], imu_times[-1]), 
                          len(imu_times))

vol_interp = np.interp(common_time, p_times, p_vols)
accel_interp = np.interp(common_time, imu_times, imu_accel)

# --- Force Calculations ---
buoyancy_f = RHO * vol_interp * G
net_f = TOTAL_INERTIA * accel_interp
drag_f = net_f - (buoyancy_f - F_GRAVITY)

# --- Plotting ---
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, sharex=True, figsize=(10, 12))

ax1.plot(common_time, buoyancy_f, label='Buoyancy ($F_b$)', color='teal')
ax1.axhline(y=F_GRAVITY, color='r', linestyle='--', label='Weight ($F_g$)')
ax1.set_ylabel('Force (N)')
ax1.legend(loc='upper right')
ax1.grid(True, alpha=0.3)
ax1.set_title('Hydrostatic Forces (from Piston Volume)')

ax2.plot(common_time, drag_f, label='Derived Drag ($F_d$)', color='orange')
ax2.set_ylabel('Force (N)')
ax2.legend(loc='upper right')
ax2.grid(True, alpha=0.3)
ax2.set_title('Dynamic Drag (Direct from IMU Acceleration)')

# Use raw string r'' to avoid LaTeX syntax warnings
ax3.plot(common_time, net_f, label=r'Net Force ($M_{total} \cdot a_z$)', color='purple')
ax3.set_ylabel('Force (N)')
ax3.set_xlabel('Time (s)')
ax3.legend(loc='upper right')
ax3.grid(True, alpha=0.3)
ax3.set_title('Resultant Force (Kinematic)')

plt.tight_layout()

# --- Save figure with auto-incrementing number ---
output_dir = Path('./figures')
output_dir.mkdir(exist_ok=True)

base_filename = "imu_force_plot"
extension = ".png"
counter = 1

while (output_dir / f"{base_filename}_{counter}{extension}").exists():
    counter += 1

final_path = output_dir / f"{base_filename}_{counter}{extension}"
plt.savefig(final_path, dpi=300)
print(f"Plot saved to: {final_path}")

plt.show()