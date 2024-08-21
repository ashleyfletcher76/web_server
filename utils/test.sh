#! /bin/bash
if [ -z "$1" ]; then
    echo "Usage: $0 <program_name>"
    exit 1
fi
PROGRAM_NAME=$1
get_memory_usage()
{
    local pid=$(pgrep -x "$PROGRAM_NAME")
    if [ -z "$pid" ]; then
        echo "Program $PROGRAM_NAME not running."
        exit 1
    fi
    local mem_usage=$(ps -o rss= -p "$pid")
    echo "$mem_usage"
}
display_memory_usage()
{
    local mem_usage_kb=$1
    local mem_usage_mb=$(echo "scale=2; $mem_usage_kb / 1024" | bc)
    echo "Memory usage of $PROGRAM_NAME (in KB): $mem_usage_kb KB"
    echo "Memory usage of $PROGRAM_NAME (in MB): $mem_usage_mb MB"
}
display_memory_changes()
{
    local initial=$1
    local current=$2
    local change=$((current - initial))
    local change_mb=$(echo "scale=2; $change / 1024" | bc)
    echo "Memory change of $PROGRAM_NAME (in KB): $change KB"
    echo "Memory change of $PROGRAM_NAME (in MB): $change_mb MB"
}
initial_usage=$(get_memory_usage)
while true; do
    clear
    current_usage=$(get_memory_usage)
    display_memory_usage "$current_usage"
    display_memory_changes "$initial_usage" "$current_usage"
    sleep 1
done