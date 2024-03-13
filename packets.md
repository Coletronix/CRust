# Packet types

- Host->Device: H2D
- Device->Host: D2H

# Packet structure

1. Car Position (D2H, H2D)
    - type
    - x
    - y
    - angle

2. Encoder Positions (H2D)
    - type
    - left (radians)
    - right (radians)

3. Camera Data (D2H_d, H2D)
    - type
    - 128 element u16 array

4. Track Layout (H2D)
    - type
    - track dimension x
    - track dimension y
    - block grid

5. Motor Commands (D2H)
    - type
    - left power
    - right power
