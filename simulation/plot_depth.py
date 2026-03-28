import matplotlib.pyplot as plt
from rosbags.highlevel import AnyReader
from pathlib import Path
import os

base_path = Path('~/Documents/Thesis/simulation/logging/').expanduser()

try:
    all_bags = [f for f in base_path.iterdir() if f.is_dir()]
    latest_bag = max(all_bags, key=os.path.getmtime)
    
    print(f"Loading most recent bag: {latest_bag.name}")
    bag_path = latest_bag
    
except (ValueError, FileNotFoundError):
    print("No bags found in the specified directory.")

# Define the topics
depth_topic = '/depth'
piston_topic = '/piston_volume'

# Lists for data
depth_times = []
depth_values = []

with AnyReader([bag_path]) as reader:
    # 1. Find the earliest timestamp for the piston volume
    piston_connections = [x for x in reader.connections if x.topic == piston_topic]
    
    # Get the timestamp of the very first message in the piston topic
    try:
        _, first_piston_timestamp, _ = next(reader.messages(connections=piston_connections))
        print(f"Piston volume detected at: {first_piston_timestamp}")
    except StopIteration:
        print("Error: /piston_volume topic never appeared in this bag.")
        first_piston_timestamp = None

    # 2. Now read the depth topic, but only keep messages >= first_piston_timestamp
    depth_connections = [x for x in reader.connections if x.topic == depth_topic]
    
    if first_piston_timestamp is not None:
        for connection, timestamp, rawdata in reader.messages(connections=depth_connections):
            if timestamp >= first_piston_timestamp:
                msg = reader.deserialize(rawdata, connection.msgtype)
                
                # Normalize time so the plot starts at 0 (the moment the piston started)
                depth_times.append((timestamp - first_piston_timestamp) / 1e9)
                depth_values.append(msg.position.z)

# Plotting with Matplotlib
plt.figure(figsize=(10, 6))
plt.plot(depth_times, depth_values, label='Depth', color='teal', linewidth=1)

plt.title(f'Step Response in Gazebo Simulation')
plt.xlabel('Time (seconds)')
plt.ylabel('Depth (m)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()

# Save figure
plt.savefig('./figures/depth.png', dpi=300)

plt.show()