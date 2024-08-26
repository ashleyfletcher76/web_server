#!/bin/bash

# Function to convert pages to MB
pages_to_mb()
{
	echo "$(( $1 * 4096 / 1048576 ))"
}

# Function to get current memory usage
get_memory_usage()
{
	local vm_stats=$(vm_stat)
	local free_pages=$(echo "$vm_stats" | grep "Pages free" | awk '{print $3}' | sed 's/\.//')
	local active_pages=$(echo "$vm_stats" | grep "Pages active" | awk '{print $3}' | sed 's/\.//')
	local inactive_pages=$(echo "$vm_stats" | grep "Pages inactive" | awk '{print $3}' | sed 's/\.//')
	local speculative_pages=$(echo "$vm_stats" | grep "Pages speculative" | awk '{print $3}' | sed 's/\.//')
	local wired_pages=$(echo "$vm_stats" | grep "Pages wired down" | awk '{print $4}' | sed 's/\.//')
	local purgeable_pages=$(echo "$vm_stats" | grep "Pages purgeable" | awk '{print $3}' | sed 's/\.//')

	echo "$free_pages $active_pages $inactive_pages $speculative_pages $wired_pages $purgeable_pages"
}

#Function to display memory usage
display_memory_usage()
{
	local usage=($1)
	local free_mb=$(pages_to_mb ${usage[0]})
	local active_mb=$(pages_to_mb ${usage[1]})
	local inactive_mb=$(pages_to_mb ${usage[2]})
	local speculative_mb=$(pages_to_mb ${usage[3]})
	local wired_mb=$(pages_to_mb ${usage[4]})
	local purgeable_mb=$(pages_to_mb ${usage[5]})

	echo "Memory usage (in mb):"
	echo " Free: $free_mb MB"
	echo " Active: $active_mb MB"
	echo " Inactive: $inactive_mb MB"
	echo " Speculative: $speculative_mb MB"
	echo " Wired: $wired_mb MB"
	echo " Purgeable: $purgeable_mb MB"
}

# Function to display memory changes
display_memory_changes()
{
	local initial=($1)
	local current=($2)

	local change_free=$((${current[0]} - ${initial[0]}))
	local change_active=$((${current[1]} - ${initial[1]}))
	local change_inactive=$((${current[2]} - ${initial[2]}))
	local change_speculative=$((${current[3]} - ${initial[3]}))
	local change_wired=$((${current[4]} - ${initial[4]}))
	local change_purgeable=$((${current[5]} - ${initial[5]}))

	echo "Memory changes (in pages):"
	echo " Free: $change_free"
	echo " Active: $change_active"
	echo " Inactive: $change_inactive"
	echo " Speculative: $change_speculative"
	echo " Wired: $change_wired"
	echo " Purgeable: $change_purgeable"
}

# Get inital memory usage
initial_usage=$(get_memory_usage)

# Infinite loop
while true; do
	clear
	current_usage=$(get_memory_usage)
	display_memory_usage "$current_usage"
	echo "---------------------"
	display_memory_changes "$initial_usage" "$current_usage"
	sleep 1
done
