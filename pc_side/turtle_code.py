import math
import turtle as t

t.pensize(2)
t.speed(0)
def draw_dot(color, rad ,angle):
    toPrint = "(" + str(angle) + chr(176) + "," + str(rad) + "cm)"
    # angle = 180 - angle
    t.color(color)
    x = 6 * rad * math.cos(math.radians(angle))
    y = 6 * rad * math.sin(math.radians(angle)) - 200
    t.penup()
    t.goto(x,y)
    t.pendown()
    t.dot(7)
    if color == "black":
        t.color("blue")
    else:
        t.color("red")
    t.penup()
    t.write(toPrint)
    t.home()
def draw_line(color,rad_start,angle_start,rad_end,angle_end):
    toPrint = "(" + str(angle_start) + chr(176) + "," + str(rad_start) + "cm) to (" + str(angle_end) + chr(176) + "," + str(rad_end) + "cm)"
    # angle = 180 - angle
    t.color(color)

    x_start = 6 * rad_start * math.cos(math.radians(angle_start))
    y_start = 6 * rad_start * math.sin(math.radians(angle_start)) - 200
    x_end = 6 * rad_end * math.cos(math.radians(angle_end))
    y_end = 6 * rad_end * math.sin(math.radians(angle_end)) - 200

    t.penup()
    t.goto(x_start, y_start)
    t.pendown()
    t.goto(x_end,y_end)
    if color == "black":
        t.color("blue")
    else:
        t.color("red")
    t.penup()
    t.write(toPrint)
    t.home()

def draw_half_circle():
    t.color("black")
    deg_180 = "180" + chr(176)
    deg_0 = "0" + chr(176)
    t.home()
    t.penup()
    t.right(90)
    t.forward(200)
    t.left(90)
    t.forward(300)
    t.color("red")
    t.write(deg_0)
    t.color("black")
    t.left(90)
    t.pendown()
    t.circle(300,180)
    t.right(90)
    t.penup()
    t.forward(20)
    t.color("red")
    t.write(deg_180, align= 'left')
    t.color("black")
    t.penup()
    t.home()
    t.pendown()

# draw_half_circle()
# draw_dot("black" , 70 , 30)
# draw_dot("yellow" , 50 , 90)
# draw_dot("black" , 65 , 40)
# draw_dot("black" , 20 , 78)
# draw_dot("yellow" , 25 , 80)
# draw_dot("yellow" , 50 , 0)
# draw_dot("black" , 50 , 120)
# t.exitonclick()
#
# draw_dot("black" , 50 , 150)
# draw_dot("black" , 50 , 180)
# t.exitonclick()
