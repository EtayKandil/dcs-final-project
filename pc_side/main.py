import PySimpleGUI as sg
import turtle_code
import serial as ser
import time
import helper_functions as hf


# globals:
color = [0 for x in range(120)]
rad = [0 for x in range(120)]
angle = [0 for x in range(120)]
state = '0'
sent_files = [] # To store info about sent files
pending_file_meta = None  # will hold {'type': 'script'|'text', 'name': str, 'slot': int}




def main():
    global pending_file_meta
    event = "first time"
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
              [sg.Text("Load up to 10 files (scripts or text).")],
              [sg.Button("Load File"), sg.InputText(key='-FILE-'), sg.FileBrowse(file_types=(("All Files", "*.*"),))],
              [sg.Button("Play Script (1-10)"), sg.InputText(key='ScriptNum')],
              [sg.Button("Exit")]]
    window = sg.Window(title = "events manu", layout= layout)

    count = 0
    count2 = 0
    while (event != "Exit"):
        # RX
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            if event == "Objects Detector System" or event == "Light Sources Detector System" or event == "Light Sources and Objects Detector System":
                lineByte = s.read_until(size=3)  # read  from the buffer until the terminator is received,
                if "xxx" == lineByte.decode("ascii"):
                    if length == 0:
                        turtle_code.draw_dot(color[count - 1], rad[count - 1], angle[count - 1])
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

            elif event == "Play Script (1-10)":
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
                decoded = lineByte.decode("ascii")
                if "xx" in decoded:
                    slot_char = decoded[-1]
                    print(f"file has loaded to slot {slot_char}")
                    try:
                        slot_num = int(slot_char)
                        if pending_file_meta is not None:
                            pending_file_meta['slot'] = slot_num
                            sent_files.append(pending_file_meta)
                    finally:
                        pending_file_meta = None
                else:
                    print("something went wrong send again :( ")
                    pending_file_meta = None
                event = "first time"
                enableTX = True
        # TX
        while s.out_waiting > 0 or enableTX:
            count = 0
            if event == "first time":
                event, val = window.read()
            if event == "Exit":
                break
            state = hf.menu_switch(event)
            if not state:
                event = "first time"
                continue

            bytetxMsg = bytes(state + '\n', 'ascii')
            if state == '8':
                try:
                    wanted_slot = int(val['ScriptNum'])
                    if not (1 <= wanted_slot <= 10):
                        sg.popup_error("Invalid slot. Please enter a number between 1 and 10.")
                        event = "first time"
                        continue

                    match = next((f for f in sent_files if f.get('slot') == wanted_slot), None)
                    if match is None:
                        sg.popup_error("No file in that slot.")
                        event = "first time"
                        continue
                    if match.get('type') != 'script':
                        sg.popup_error("That slot contains a text file. Cannot play.")
                        event = "first time"
                        continue

                except ValueError:
                    sg.popup_error("Invalid input. Please enter a valid slot number.")
                    event = "first time"
                    continue

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
                if len(sent_files) >= 10:
                    sg.popup("Maximum file limit (10) reached. Cannot send more files.")
                    event = "first time"
                    continue

                file_path = val['-FILE-']
                if not file_path:
                    event = "first time"
                    continue

                file_type = hf.get_file_type(file_path)
                file_name = hf.get_file_name(file_path)

                # Send file type
                file_type_code = '1' if file_type == 'script' else '2'
                s.write(bytes(file_type_code + '\n', 'ascii'))
                time.sleep(0.25)

                # Send file header (name)
                s.write(bytes(file_name + '\n', 'ascii'))
                time.sleep(0.25)

                file_content = hf.openFile(file_path)
                if file_type == "script":
                    content_to_send = hf.txtToHex(file_content)
                else:
                    content_to_send = file_content

                # Send file content
                bytetxVal = bytes(content_to_send + '\n', 'ascii')
                s.write(bytetxVal)
                time.sleep(0.25)

                pending_file_meta = {'type': file_type, 'name': file_name}
                print(f"File '{file_name}' ({file_type}) sent; waiting for MCU slot assignment…")

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
    main()