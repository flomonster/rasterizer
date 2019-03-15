points = [
((261.622, 73.1353),(108.958, 182.512),(-20.1957, 56.2955),),
((261.622, 73.1353),(-20.1957, 56.2955),(132.468, -53.0817),),
((265.226, 183.162),(136.072, 56.9452),(-137.527, 171.425),),
((265.226, 183.162),(-137.527, 171.425),(112.562, 292.539),),
((261.622, 73.1353),(265.226, 183.162),(112.562, 292.539),),
((261.622, 73.1353),(112.562, 292.539),(108.958, 182.512),),
((108.958, 182.512),(112.562, 292.539),(-137.527, 171.425),),
((108.958, 182.512),(-137.527, 171.425),(-20.1957, 56.2955),),
((-20.1957, 56.2955),(-137.527, 171.425),(136.072, 56.9452),),
((-20.1957, 56.2955),(136.072, 56.9452),(132.468, -53.0817),),
((265.226, 183.162),(261.622, 73.1353),(132.468, -53.0817),),
((265.226, 183.162),(132.468, -53.0817),(136.072, 56.9452),),
]

scale = 1
import turtle
turtle.speed(100)
def sc(v):
    x, y = v
    return x * scale, y * scale

t = turtle.Turtle()
t.up()
for a, b, c in points:
    t.goto(*sc(a))
    t.down()
    t.goto(*sc(b))
    t.goto(*sc(c))
    t.goto(*sc(a))
    t.up()
input()