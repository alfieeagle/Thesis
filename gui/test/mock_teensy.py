import os
import pty
import time
import struct
import sys
import math

sys.path.append(os.path.abspath("../messages"))

try:
    import messages_pb2  # pyright: ignore[reportMissingImports]
except ImportError:
    print("Error: Could not find messages_pb2.py. Did you run 'protoc'?")
    sys.exit(1)

def mock_teensy():
    # Create a virtual serial port pair
    master, slave = pty.openpty()
    port_name = os.ttyname(slave)
    print(f"--- Mock Teensy Active ---")
    print(f"Connect your dashboard to: {port_name}")

    t = 0
    while True:
        # 1. Create fake physics data (Sine wave for depth)
        status = messages_pb2.system_status()
        status.depth = 1.0 + 0.5 * (math.sin(t))
        status.ref_depth = 1.0
        status.status = True
        status.piston_pos = 50.0 + 10.0 * (math.cos(t))
        
        # 2. Encode to Protobuf
        payload = status.SerializeToString()
        length = len(payload)
        
        # 3. Frame it: [Start Byte 0xAA] [Length] [Payload]
        packet = struct.pack('BB', 0xAA, length) + payload
        
        os.write(master, packet)
        
        t += 0.1
        time.sleep(0.1) # 10Hz update rate

if __name__ == "__main__":
    import math
    mock_teensy()