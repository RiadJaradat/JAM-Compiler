import argparse

# 1. Set up the argument parser
parser = argparse.ArgumentParser(description="A clean Python implementation of hexdump -C")

# 2. Add your arguments (positional argument for the file path)
parser.add_argument("filename", help="The path to the binary file you want to inspect")

# 3. Parse the command-line arguments
args = parser.parse_args()

offset = 0

# 4. Use the parsed argument (args.filename) to open the file
with open(args.filename, 'rb') as f:
    while True:
        chunk = f.read(16)
        if not chunk:
            break
            
        part1 = " ".join(f"{b:02x}" for b in chunk[:8])
        part2 = " ".join(f"{b:02x}" for b in chunk[8:])
        hex_string = f"{part1}  {part2}".strip()
        
        hex_padded = f"{hex_string:<48}"
        text_string = "".join(chr(b) if 32 <= b <= 126 else "." for b in chunk)
        
        print(f"{offset:08x}  {hex_padded}  |{text_string}|")
        offset += len(chunk)

print(f"{offset:08x}")