import matplotlib.pyplot as plt
from rosbags.highlevel import AnyReader
from pathlib import Path

bag_path = Path('~/Documents/Thesis/simulation/logging/five_meter_dive/').expanduser() 
topic_name = '/piston_volume' 

times = []
values = []

# Read the Bag
with AnyReader([bag_path]) as reader:
    # Find the connection for the specific topic
    connections = [x for x in reader.connections if x.topic == topic_name]
    
    # Start time for relative timestamping (t=0)
    start_time = None
    
    for connection, timestamp, rawdata in reader.messages(connections=connections):
        # Deserialize the message (handles standard ROS 2 types)
        msg = reader.deserialize(rawdata, connection.msgtype)
        
        if start_time is None:
            start_time = timestamp
        
        # Calculate time in seconds from start
        # Timestamp is in nanoseconds
        times.append((timestamp - start_time) / 1e9)
        
        # Extract the data (assuming it's a Double/Float message)
        values.append(msg.data)

# Plotting with Matplotlib
plt.figure(figsize=(10, 6))
plt.plot(times, values, label='Piston Volume', color='teal', linewidth=1)

plt.title(f'Actuator Effort in Gazebo Simulation')
plt.xlabel('Time (seconds)')
plt.ylabel('Piston Volume (m^3)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()

# Save figure
plt.savefig('./figures/piston_volume.png', dpi=300)

plt.show()