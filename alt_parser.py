import csv
import random

def read_and_format_csv(filepath):
    # Open the CSV file
    with open(filepath, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        next(reader)  # Skip the second row
        # Extract every other value from the second column until we have ~200 values
        values = []
        for i, row in enumerate(reader):
            if i % 1 == 0:  # Skip every other row
                try:
                    value = float(row[1]) / 3.281 # Convert to float
                    value = round(value,3)
                    # Introduce noise and outliers
                    if random.random() < 0.05:  # 20% chance of introducing noise or outlier
                        value += random.uniform(-5, 5)  # Add random noise between -50 and 50
                    if random.random() < 0.02:  # 5% chance of introducing an outlier
                        value *= random.uniform(10, 20)  # Multiply value by a random factor between 10 and 20
                    values.append(value)  # Append the value
                    if len(values) >= 1000:  # Stop when we have enough values
                        break
                except ValueError:
                    # Handle the case where conversion to float fails
                    print(f"Skipping row {i+2}: cannot convert '{row[1]}' to float")
        
    return values

def format_as_cpp_array(values):
    # Format the values as a C++ array
    cpp_array = 'float myArray[] = {' + ', '.join(map(str, values)) + '};'
    return cpp_array

# Example usage
filepath = 'Jan_flight1.csv'  # Replace with your actual file path
values = read_and_format_csv(filepath)
cpp_array_str = format_as_cpp_array(values)
print(cpp_array_str)
