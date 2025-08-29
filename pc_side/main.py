import PySimpleGUI as sg
import turtle_code
import serial as ser
import time


# globals:
color = [0 for x in range(120)]
rad = [0 for x in range(120)]
angle = [0 for x in range(120)]
state = '0'




def menu_switch(event):
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
    elif event == "Play Script (1...10)":
        return '8'
    elif event == "Delete All Files":
        return '9'
def decTohex(str):
    temp = int(str)
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


def remove_newlines(text: str) -> str:
    """Return the input text with all newline characters (CR and LF) removed."""
    if text is None:
        return ""
    return text.replace('\n', ' ')


def main():
    event = "first time"
    scriptNum = 0
    start_dot = [0, 0]
    end_dot = [0, 0]
    length = 0
    s = ser.Serial('COM2', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)   # timeout of 1 sec so that the read and write operations are blocking,
                                # when the timeout expires the program will continue
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    enableTX = True
    layout = [[sg.Text("press a button: ")],
              [sg.Button("set mask distance (0-200)cm (def == 100)"), sg.InputText(key='maskDist')],
              [sg.Button("LDR config")],
              [sg.Button("Objects Detector System")],
              [sg.Button("Telemeter"),sg.InputText(key='TELangel')],
              [sg.Button("Light Sources Detector System")],
              [sg.Button("Light Sources and Objects Detector System")],
              [sg.Text("first 10 scripts will load to 1, 2, 3 by order")],
              [sg.Button("Load File"), sg.InputText(key='-FILE-'), sg.FileBrowse()],
              [sg.Button("Play Script (1...10)"), sg.InputText(key='ScriptNum')],
              [sg.Button("Delete All Files")],
              [sg.Button("Exit")]]
    window = sg.Window(title = "Events Menu", layout= layout)

    count = 0
    count2 = 0
    while (event != "Exit"):
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            if event == "Objects Detector System" or event == "Light Sources Detector System" or event == "Light Sources and Objects Detector System":
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                if "xxx" == lineByte.decode("ascii"):
                    if length == 0:
                        # turtle_code.draw_dot(color[count - 1], rad[count - 1], angle[count - 1])
                        smart = False
                        itay = smart
                    else:
                        turtle_code.draw_line(color[count - 1], start_dot[0], start_dot[1], end_dot[0], end_dot[1])
                    count = 0
                    s.reset_input_buffer()
                    event = "first time"
                    enableTX = True
                    break
                if count == 0:
                    turtle_code.t.clear()
                    turtle_code.draw_half_circle()

                angle[count] = 2 * int(lineByte.decode("ascii"))   # int(lineByte.decode("ascii"))
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                rad[count] = int(lineByte.decode("ascii"))
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                tempColor = int(lineByte.decode("ascii"))
                if tempColor == 1:
                    color[count] = "yellow"
                else:
                    color[count] = "black"
                if count > 0:
                    if abs(rad[count]-rad[count-1]) < 3 and angle[count] == angle[count-1]+2 and color[count] == color[count - 1]:
                        end_dot = [rad[count], angle[count]]
                        length = length + 1
                    else:
                        if length == 0:
                            turtle_code.draw_dot(color[count-1], rad[count-1], angle[count-1])
                        else:
                            turtle_code.draw_line(color[count-1], start_dot[0], start_dot[1], end_dot[0], end_dot[1])
                        length = 0
                        start_dot = [rad[count], angle[count]]
                else:
                    start_dot = [rad[count], angle[count]]
                count += 1

            elif event == "Telemeter":
                event = "__TIMEOUT__"
                # GUI layout2
                layout2 = [
                    [sg.Text("distance: ", key="-DYNAMIC_TEXT-")],
                ]
                window2 = sg.Window("Telemeter", layout2, finalize=True)
                while event == "__TIMEOUT__":
                    lineByte = s.read(size=3)
                    dist = lineByte.decode("ascii")
                    if(len(dist) == 3):
                        if dist[0] == '0':
                            if dist[1] == '0':
                                dist = dist[2]
                            else:
                                dist = dist[1:]
                        event, values = window2.read(timeout=100)  # Add timeout to update the GUI every 100 milliseconds
                        if event == sg.WIN_CLOSED:
                            break
                        window2["-DYNAMIC_TEXT-"].update(f"distance:  {dist}")
                        event, val = window.read(timeout=100)
                window2.close()
                s.reset_input_buffer()
                enableTX = True
                break

            elif event == "Play Script (1...10)":
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                if "zzz" == lineByte.decode("ascii"):
                    if count > 0:
                        count2 = -1
                    s.reset_input_buffer()
                    event = "first time"
                    enableTX = True
                    break
                if "xxx" == lineByte.decode("ascii"):
                    if count > 0:
                        if length == 0:
                            turtle_code.draw_dot(color[count - 1], rad[count - 1], angle[count - 1])
                        else:
                            turtle_code.draw_line(color[count - 1], start_dot[0], start_dot[1], end_dot[0], end_dot[1])
                        count = 0
                    break
                if count == 0:
                    turtle_code.t.clear()
                    turtle_code.draw_half_circle()
                angle[count] = 2 * int(lineByte.decode("ascii"))  # int(lineByte.decode("ascii"))
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                rad[count] = int(lineByte.decode("ascii"))
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                tempColor = int(lineByte.decode("ascii"))
                if tempColor == 1:
                    color[count] = "yellow"
                else:
                    color[count] = "black"
                if count > 0:
                    if abs(rad[count] - rad[count - 1]) < 3 and angle[count] == angle[count - 1] + 2 and color[count] == \
                            color[count - 1]:
                        end_dot = [rad[count], angle[count]]
                        length = length + 1
                    else:
                        if length == 0:
                            turtle_code.draw_dot(color[count - 1], rad[count - 1], angle[count - 1])
                        else:
                            turtle_code.draw_line(color[count - 1], start_dot[0], start_dot[1], end_dot[0], end_dot[1])
                        length = 0
                        start_dot = [rad[count], angle[count]]
                else:
                    start_dot = [rad[count], angle[count]]
                count += 1

            elif event == "Load File":
                lineByte = s.read_until(size=3)
                if lineByte.decode("ascii") == "xx1":
                    print("File has loaded to slot 1")
                elif lineByte.decode("ascii") == "xx2":
                    print("File has loaded to slot 2")
                elif lineByte.decode("ascii") == "xx3":
                    print("File has loaded to slot 3")
                elif lineByte.decode("ascii") == "xx4":
                    print("File has loaded to slot 4")
                elif lineByte.decode("ascii") == "xx5":
                    print("File has loaded to slot 5")
                elif lineByte.decode("ascii") == "xx6":
                    print("File has loaded to slot 6")
                elif lineByte.decode("ascii") == "xx7":
                    print("File has loaded to slot 7")
                elif lineByte.decode("ascii") == "xx8":
                    print("File has loaded to slot 8")
                elif lineByte.decode("ascii") == "xx9":
                    print("File has loaded to slot 9")
                elif lineByte.decode("ascii") == "xx:":
                    print("File has loaded to slot 10")
                else:
                    print("something went wrong send again :( ")
                event = "first time"
                enableTX = True

            elif event == "Delete All Files":
                lineByte = s.read_until(size=3)
                if "xxx" == lineByte.decode("ascii"):
                    print("All files deleted")
                    scriptNum = 0
                    enableTX = True

                event = "first time"
        # TX
        while s.out_waiting > 0 or enableTX:
            count = 0
            if event == "first time":
                event, val = window.read()
            if event == "Exit":
                break
            state = menu_switch(event)
            bytetxMsg = bytes(state + '\n', 'ascii')
            if state == '8':
                if int(val['ScriptNum']) != 1 and int(val['ScriptNum']) != 2 and int(val['ScriptNum']) != 3 and int(val['ScriptNum']) != 4 and int(val['ScriptNum']) != 5 and int(val['ScriptNum']) != 6 and int(val['ScriptNum']) != 7 and int(val['ScriptNum']) != 8 and int(val['ScriptNum']) != 9 and int(val['ScriptNum']) != 10:
                    print("not a valid number please insert again")
                    event = "first time"
                    break
            if state == '2':
                if int(val['TELangel']) < 0 or int(val['TELangel']) > 180 :
                    print("not a valid number please insert again")
                    event = "first time"
                    break
            if state == '6':
                if int(val['maskDist']) < 0 or int(val['maskDist']) > 200:
                    print("not a valid number please insert again")
                    event = "first time"
                    break

            s.write(bytetxMsg)
            time.sleep(0.25)  # delay for accurate read/write operations on both ends
            if s.out_waiting == 0 and state == '2':                               #to delete
                bytetxVal = bytes(val['TELangel'] + '\n', 'ascii')
                s.write(bytetxVal)
                time.sleep(0.25)  # delay for accurate read/write operations on both ends
                if s.out_waiting == 0:
                    enableTX = False
            elif s.out_waiting == 0 and state == '6':
                bytetxVal = bytes(val['maskDist'] + '\n', 'ascii')
                s.write(bytetxVal)
                event = "first time"
                time.sleep(0.25)  # delay for accurate read/write operations on both ends
            elif s.out_waiting == 0 and state == '5':
                if scriptNum == 10:
                    print("delete the scripts to load more")
                    state = '0'
                #     layout3 = [[sg.Text("pick a script to replace:")],
                #     # [sg.Button("1")],[sg.Button("2")],[sg.Button("3")],[sg.Button("4")],[sg.Button("5")],[sg.Button("6")],[sg.Button("1")],[sg.Button("2")],[sg.Button("3")]]
                #     window3 = sg.Window(title="events manu", layout=layout3)
                #     event3, val3 = window3.read()
                #     bytetxVal = bytes(event3 + '\n', 'ascii')
                #     s.write(bytetxVal)
                #     time.sleep(0.25)
                else:
                    content = openFile(val['-FILE-'])
                    file_path = val.get('-FILE-', '')
                    i = max(file_path.rfind('/'), file_path.rfind('\\'))
                    filename = file_path[i + 1:] if i != -1 else file_path  # e.g. "script1_code.txt"
                    name = filename.rsplit('.', 1)[0]  # e.g. "script1_code"
                    scriptNum += 1
                    if (name.startswith("text")):
                        bytetxVal = bytes('t' + '\n', 'ascii')
                    else:
                        bytetxVal = bytes('s' + '\n', 'ascii')

                    s.write(bytetxVal)
                    time.sleep(0.25)

                    content = remove_newlines(content)
                    bytetxVal = bytes(name + '\n', 'ascii')
                    s.write(bytetxVal)
                    time.sleep(0.25)
                    if (name.startswith("text")):
                        bytetxVal = bytes(content  + '\n', 'ascii')

                    else:
                        bytetxVal = bytes(txtToHex(content)  + '\n', 'ascii')

                    k = 0
                    if (len(bytetxVal) > 200):
                        tmp = len(bytetxVal) // 200
                        while (k < tmp):
                            part = bytetxVal[k * 200:(k + 1) * 200] + b'\n'
                            s.write(part)
                            time.sleep(0.25)
                            k += 1
                        if (len(bytetxVal) % 200 != 0):
                            part = bytetxVal[k * 200:]
                            s.write(part)
                            time.sleep(0.25)
                    else:
                        s.write(bytetxVal)
                        time.sleep(0.25)  # delay for accurate read/write operations on both end

                if s.out_waiting == 0:
                    enableTX = False
            elif s.out_waiting == 0 and state == '8':
                bytetxVal = bytes(val['ScriptNum'] + '\n', 'ascii')
                s.write(bytetxVal)
                time.sleep(0.25)  # delay for accurate read/write operations on both ends
                if s.out_waiting == 0:
                    enableTX = False
            elif s.out_waiting == 0 and state == '7':
                event = "first time"
            elif s.out_waiting == 0 :
                enableTX = False

    window.close()



if __name__ == '__main__':
    scriptNum = 0
    main()