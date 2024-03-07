import csv

def read_and_format_csv(filepath):
    # Open the CSV file
    with open(filepath, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        next(reader)
        # Extract every other value from the second column until we have ~200 values
        values = []
        for i, row in enumerate(reader):
            if i % 2 == 0:  # Skip every other row
                try:
                    values.append(float(row[1]))  # Convert to float and append
                    if len(values) >= 200:  # Stop when we have enough values
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
