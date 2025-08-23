import PySimpleGUI as sg
import turtle_code
import serial as ser
import time
import os
def decTohex(str_in):
    temp = int(str_in)
    if temp > 15:
        return hex(temp)[2:]
    else:
        return "0" + hex(temp)[2:]
def txtToHex(txt):
    toReturn = ""
    lines = txt.split('\n')
    words_by_line = [line.split(' ') for line in lines]

    for line in words_by_line:
        if len(line) == 1:
            if line[0] == "clear_lcd":
                toReturn += "05"
            elif line[0] == "sleep":
                toReturn += "08"
        elif len(line) == 2:
            if line[0] == "inc_lcd":
                toReturn += "01"
            elif line[0] == "dec_lcd":
                toReturn += "02"
            elif line[0] == "set_delay":
                toReturn += "04"
            elif line[0] == "servo_deg":
                toReturn += "06"
            if line[0] == "servo_scan":
                toReturn += "07"
                nums = line[1].split(',')
                toReturn += decTohex(nums[0])
                toReturn += decTohex(nums[1])
            else:
                if line[0] == "rra_lcd":
                    toReturn += "03"
                    toReturn += line[1]
                else:
                    toReturn += decTohex(line[1])
    return toReturn
def openFile(file_path):
    with open(file_path, 'r') as file:
        return file.read()
def get_file_type(file_path):
    """
    Determines the file type by checking for script-specific keywords.
    """
    content = openFile(file_path)
    script_keywords = ["inc_lcd", "dec_lcd", "rra_lcd", "set_delay", "clear_lcd", "servo_deg", "servo_scan", "sleep"]
    if any(keyword in content for keyword in script_keywords):
        return "script"
    else:
        return "text"
def get_file_name(file_path):
    """
    Extracts the file name from a full path.
    """
    return os.path.basename(file_path)
def menu_switch(event):
    """
    Maps GUI events to state numbers for serial communication.
    """
    if event == "Objects Detector System":
        return '1'
    elif event == "Telemeter":
        return '2'
    elif event == "Light Sources Detector System":
        return '3'
    elif event == "Light Sources and Objects Detector System":
        return '4'
    elif event == "Load File":
        return '5'
    elif event == "set mask distance (0-200)cm (def == 100)":
        return '6'
    elif event == "LDR config":
        return '7'
    elif event == "Play Script (1/2/3)":
        return '8'
    return None
