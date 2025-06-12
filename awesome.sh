#!/usr/bin/env bash
test=$(awesome-client '
    local awful = require("awful")

    -- Used for debugging
    -- "return debug" is the only way to get something output!
    local debug=""


    for s in screen do
        -- debug = debug .. "Tag ".. tostring(s.selected_tag.name) .. ": "
        
        local selectedTag = s.selected_tag
        local clientTag = selectedTag:clients()

        for _, c in ipairs(clientTag) do
            debug = debug .. string.format(c.class) .. "\n"
        end
    end

    return debug
    ')
# running the awesome-client in bash will leave behind an empty line

readarray -t applications <<< $(echo "$test" | sed 's/string//g' | sed 's/"//g' | sed 's/ //g')

# This loop will print every element in the array

if [[ ${#applications[@]} -eq 0 ]]; then
    echo "No applications found or array is empty."
else
    for app in "${applications[@]}"; do
        echo "$app"
    done
fi
