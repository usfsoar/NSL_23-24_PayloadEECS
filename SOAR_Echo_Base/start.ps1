# Define the script actions
$scriptBlock = {
    # Build the Docker image
    docker build -t soar_echo_base .

    # Run docker system prune
    docker system prune -f | Out-Null

    # Start docker-compose services
    docker-compose up
}

# Execute the script actions
Invoke-Command -ScriptBlock $scriptBlock
