import os
import shutil
import fileinput

def create_backup(original_folder, backup_folder):
    # Check if the backup folder already exists
    if os.path.exists(backup_folder):
        # If it exists, remove it to create a fresh backup
        shutil.rmtree(backup_folder)

    # Copy the original folder to create a backup
    shutil.copytree(original_folder, backup_folder)

    # Rename the .ino file in the backup folder
    original_ino = os.path.join(backup_folder, "deploy_module_main.ino")
    backup_ino = os.path.join(backup_folder, "deploy_module_backup.ino")
    os.rename(original_ino, backup_ino)

def update_ino_file(file_path, replacements):
    # Perform the replacements in the .ino file
    with fileinput.FileInput(file_path, inplace=True) as file:
        for line in file:
            for old, new in replacements.items():
                line = line.replace(f'{old}', f'{new}')
            print(line, end='')

if __name__ == "__main__":
    replacements = {
        '#define stepPin 10': '#define stepPin 5',
        '#define dirPin 11': '#define dirPin 4',
        '#define buzzerPin 1': '#define buzzerPin 10'
    }
    # Define the original and backup folder names
    original_folder = "deploy_module_main"
    backup_folder = "deploy_module_backup"

    # Create a backup of the original folder and rename the .ino file
    create_backup(original_folder, backup_folder)

    # Define the path to the .ino file in the backup folder
    ino_file_path = os.path.join(backup_folder, "deploy_module_backup.ino")

    # Update the .ino file in the backup folder
    update_ino_file(ino_file_path, replacements)

    print("Backup created and modifications applied.")
